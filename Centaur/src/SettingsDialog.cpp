////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "SettingsDialog.hpp"
#include "../ui/ui_SettingsDialog.h"
#include "AddUserDialog.hpp"
#include "CentaurApp.hpp"
#include "DAL.hpp"
#include "Globals.hpp"
#include "Logger.hpp"
#include "LoginDialog.hpp"
#include "PixmapMemCacheDialog.hpp"
#include "QRCodeDialog.hpp"
#include "TOTP.hpp"
#include <QCheckBox>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QGraphicsBlurEffect>
#include <QMessageBox>
#include <QPainter>
#include <QProgressDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QStyledItemDelegate>
#include <QUuid>
#include <QtConcurrent>

#include <limits>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>

#include <zip.h>

/**
 * \brief
 * This object will handle all related to the settings
 * And very importantly how the session is started
 **/

BEGIN_CENTAUR_NAMESPACE

namespace
{

    constexpr int _settings_plugin_enable_magic = 0x70454545;

    using updatePtr = void (*)(const QString &, bool);
    using statusPtr = bool (*)(const QString &);

    std::string toBase32(const std::string &data)
    {
        const auto size       = data.size();
        const auto mod        = size % 5;
        const auto iterations = [&]() {
            auto iter = size / 5;
            return mod ? iter + 1 : iter;
        }();

        if (size == 0)
            return {};

        static constexpr std::array<char, 33> base32Table = {
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
        };
        static constexpr std::array<uint64_t, 5> index   = { 0, 2, 4, 5, 7 };
        static constexpr std::array<uint64_t, 5> padding = { 0, 6, 4, 3, 1 };

        std::string out;
        out.reserve(iterations * 8);
        for (auto i = 0ull; i < iterations; ++i)
        {
            const auto copy = (i == iterations - 1 && mod) ? mod : 5ull;

            uint64_t block { 0 };
            memmove(&block, data.substr(i * 5, copy).c_str(), copy);
            uint64_t block_be = 0xffffffffff & ((block << 32) | (block << 16 & 0xFF000000) | (block << 0 & 0x00FF0000) | (block >> 16 & 0x0000FF00) | (block >> 32 & 0x000000FF));

            for (int k = 7; k >= 0; --k)
            {
                const uint64_t idx = (block_be >> 5 * k) & 0x1F;
                out.push_back(base32Table[idx]);
            }
        }
        if (mod)
        {
            for (auto k = (iterations - 1) * 8 + index[mod], i = 0ull; i < padding[mod]; ++i, ++k)
                out[k] = '=';
        }

        return out;
    }

    template <typename T>
    concept integer = std::numeric_limits<T>::is_integer;

    auto bytesSizeMaxUnit(integer auto bytes) -> int
    {
        decltype(bytes) mem = bytes / 1000;
        int maxUnit         = 0;
        while (mem && maxUnit < 4)
        {
            ++maxUnit;
            mem /= 1000;
        }

        return maxUnit;
    }

    auto bytesToString(integer auto bytes, int unit = -1, bool appendUnit = true) -> QString
    {
        switch (unit == -1 ? bytesSizeMaxUnit(bytes) : unit)
        {
            case 1:
                return QString("%1%2").arg(static_cast<qreal>(bytes) / 1'000., 0, 'f', 3).arg(appendUnit ? " KB" : "");
            case 2:
                return QString("%1%2").arg(static_cast<qreal>(bytes) / 1'000'000., 0, 'f', 4).arg(appendUnit ? " MB" : "");
            case 3:
                return QString("%1%2").arg(static_cast<qreal>(bytes) / 1'000'000'000., 0, 'f', 5).arg(appendUnit ? " GB" : "");
            case 0:
            default:
                return QString("%1%2").arg(bytes).arg(appendUnit ? " bytes" : "");
        }
    }

    class DelegateItem : public QStyledItemDelegate
    {
    public:
        explicit DelegateItem(QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option4, const QModelIndex &index) const override;
    };
} // namespace

DelegateItem::DelegateItem(QObject *parent) :
    QStyledItemDelegate { parent }
{
}

void DelegateItem::paint(QPainter *painter, const QStyleOptionViewItem &option4, const QModelIndex &index) const
{
    QStyleOptionViewItem options { option4 };
    initStyleOption(&options, index);

    if (options.state & QStyle::State_Selected)
        QStyledItemDelegate::paint(painter, options, index);
    else
    {
        painter->save();
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

        painter->restore();
    }
}

struct UserInformationTableWidgetItems final
{
    Q_DECLARE_TR_FUNCTIONS(UserInformationTableWidgetItems)

public:
    inline UserInformationTableWidgetItems() :
        _name { new QTableWidgetItem(QIcon(":/login/u_table/person"), tr("User display name")) },
        _user { new QTableWidgetItem(QIcon(":/login/u_table/user"), tr("Log in name")) },
        _psw { new QTableWidgetItem(QIcon(":/login/u_table/psw"), tr("User password")) },
        _email { new QTableWidgetItem(QIcon(":/login/u_table/email"), tr("User e-mail")) },
        _picture { new QTableWidgetItem(QIcon(":/login/u_table/pic"), tr("User picture")) },
        _2fa { new QTableWidgetItem(QIcon(":/login/u_table/lock"), tr("Allow Two-factor authentication")) },
        _nameValue { new QTableWidgetItem(tr("")) },
        _userValue { new QTableWidgetItem(tr("")) },
        _pswValue { new QTableWidgetItem(tr("")) },
        _emailValue { new QTableWidgetItem(tr("")) },
        _pictureValue { new QTableWidgetItem(tr("")) },
        _checkBoxWidget { new QWidget },
        _checkBox { new QCheckBox }
    {
        // "Trick" to make the checkbox H-centered
        auto *checkBoxLayout = new QHBoxLayout(_checkBoxWidget);
        checkBoxLayout->addWidget(_checkBox);
        checkBoxLayout->setAlignment(Qt::AlignCenter);
        checkBoxLayout->setContentsMargins(0, 0, 0, 0);
        _checkBoxWidget->setLayout(checkBoxLayout);
    }

public:
    QTableWidgetItem *_name;
    QTableWidgetItem *_user;
    QTableWidgetItem *_psw;
    QTableWidgetItem *_email;
    QTableWidgetItem *_picture;
    QTableWidgetItem *_2fa;
    QTableWidgetItem *_nameValue;
    QTableWidgetItem *_userValue;
    QTableWidgetItem *_pswValue;
    QTableWidgetItem *_emailValue;
    QTableWidgetItem *_pictureValue;
    QWidget *_checkBoxWidget;
    QCheckBox *_checkBox;
};

struct AdvancedWidgetItems final
{
    Q_DECLARE_TR_FUNCTIONS(AdvancedWidgetItems)

public:
    inline AdvancedWidgetItems() :
        _pixmapCache { new QStandardItem(QIcon(":/ui/settings/advanced/table/image_cache"), tr("Image Cache Memory")) },
        _pixmapCacheValue { new QStandardItem() }
    {
        _pixmapCache->setFlags(_pixmapCacheValue->flags() & ~Qt::ItemIsEditable);
        _pixmapCacheValue->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        _pixmapCacheValue->setText(bytesToString(QPixmapCache::cacheLimit()));
    }

public:
    QStandardItem *_pixmapCache;
    QStandardItem *_pixmapCacheValue;
};

struct SettingsDialog::Impl
{
    inline Impl() :
        ui { new Ui::SettingsDialog }
    {
    }
    inline ~Impl() = default;

    QWidget *currencySettingsWidget { nullptr };
    QTreeWidgetItem *sessionItem { nullptr };
    QTreeWidgetItem *advancedItem { nullptr };
    QTreeWidgetItem *pluginsItem { nullptr };
    UserInformationTableWidgetItems userInfoTableItems;
    AdvancedWidgetItems advancedTableItems;

    std::unique_ptr<Ui::SettingsDialog> ui;

    bool forceShowPluginWarning { false };
};

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog { parent },
    _impl { new Impl }
{
    ui()->setupUi(this);
    connect(ui()->acceptButton, &QPushButton::released, this, &SettingsDialog::onAccept);
    connect(ui()->treeWidget, &QTreeWidget::itemPressed, this, &SettingsDialog::treeItemPressed);

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir directory(path);
    if (!directory.exists())
    {
        logInfo("settings", "Application path does not exist.");
        if (directory.mkpath(path))
            logInfo("settings", "Application path was created");
        else
            logError("settings", "Application path was not created");
    }

    ui()->titleFrame->overrideParent(this);
    ui()->mainSettingsFrame->overrideParent(this);

#ifdef Q_OS_MAC
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    ui()->closeButton->setButtonClass(SystemPushButton::ButtonClass::override);
    connect(ui()->closeButton, &SystemPushButton::systemPressed, this, [&]() {
        reject();
    });

    ui()->warnLabel->setVisible(false);
    ui()->warnIconLabel->setVisible(false);

    initializeTree();
    initSessionWidget();
    initAdvancedWidget();
    initPluginsWidget();

    restoreInterface();

    ui()->treeWidget->setCurrentItem(_impl->sessionItem);
    setPage(Pages::Session);
}

SettingsDialog::~SettingsDialog() = default;

Ui::SettingsDialog *SettingsDialog::ui()
{
    return _impl->ui.get();
}

void SettingsDialog::showWarningLabels() noexcept
{
    ui()->warnLabel->setVisible(true);
    ui()->warnIconLabel->setVisible(true);
}

void SettingsDialog::hideWarningLabels() noexcept
{
    ui()->warnLabel->setVisible(false);
    ui()->warnIconLabel->setVisible(false);
}

bool SettingsDialog::isFirstTimeStarted() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("__Session__");
    const auto value = settings.value("__time__").toBool();
    settings.endGroup();
    return !value;
}

void SettingsDialog::onAccept() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("SettingsDialog");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter", ui()->splitter->saveState());
    settings.endGroup();

    settings.beginGroup("infoUserTable");
    settings.setValue("geometry", ui()->userTable->saveGeometry());
    settings.setValue("h-geometry", ui()->userTable->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->userTable->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("memTableWidget");
    settings.setValue("geometry", ui()->memTableView->saveGeometry());
    settings.setValue("h-geometry", ui()->memTableView->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->memTableView->horizontalHeader()->saveState());
    settings.endGroup();

    settings.beginGroup("pluginsTableWidget");
    settings.setValue("geometry", ui()->pluginsTableWidget->saveGeometry());
    settings.setValue("h-geometry", ui()->pluginsTableWidget->horizontalHeader()->saveGeometry());
    settings.setValue("state", ui()->pluginsTableWidget->horizontalHeader()->saveState());
    settings.endGroup();

    accept();
}

void SettingsDialog::restoreInterface() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("SettingsDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    ui()->splitter->restoreState(settings.value("splitter").toByteArray());
    settings.endGroup();

    settings.beginGroup("infoUserTable");
    ui()->userTable->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->userTable->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->userTable->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("memTableWidget");
    ui()->memTableView->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->memTableView->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->memTableView->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    settings.beginGroup("pluginsTableWidget");
    ui()->pluginsTableWidget->restoreGeometry(settings.value("geometry").toByteArray());
    ui()->pluginsTableWidget->horizontalHeader()->restoreGeometry(settings.value("h-geometry").toByteArray());
    ui()->pluginsTableWidget->horizontalHeader()->restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void SettingsDialog::initializeTree() noexcept
{
    ui()->treeWidget->setIndentation(20);
    ui()->treeWidget->setColumnCount(1);

    _impl->sessionItem  = new QTreeWidgetItem({ tr("Session") });
    _impl->advancedItem = new QTreeWidgetItem({ tr("Advanced") });
    _impl->pluginsItem  = new QTreeWidgetItem({ tr("Plugins") });

    ui()->treeWidget->insertTopLevelItems(0, { _impl->sessionItem, _impl->advancedItem, _impl->pluginsItem });
    ui()->treeWidget->expandItem(_impl->sessionItem);
    ui()->treeWidget->expandItem(_impl->advancedItem);
    ui()->treeWidget->expandItem(_impl->pluginsItem);
}

void SettingsDialog::treeItemPressed(QTreeWidgetItem *item, C_UNUSED int column) noexcept
{
    if (item == _impl->sessionItem)
    {
        setPage(Pages::Session);
    }
    else if (item == _impl->advancedItem)
    {
        setPage(Pages::Advanced);
    }
    else if (item == _impl->pluginsItem)
    {
        setPage(Pages::Plugins);
    }
}

void SettingsDialog::initSessionWidget() noexcept
{
    connect(ui()->addUserBtn, &QPushButton::released, this, &SettingsDialog::onAddUser);
    connect(_impl->userInfoTableItems._checkBox, &QCheckBox::clicked, this, &SettingsDialog::onAllow2FA);

    // Info user table data
    ui()->userTable->setColumnCount(2);
    ui()->userTable->setRowCount(6);
    ui()->userTable->setHorizontalHeaderLabels({ tr("Option"), tr("Value") });
    ui()->userTable->setItem(0, 0, _impl->userInfoTableItems._name);
    ui()->userTable->setItem(1, 0, _impl->userInfoTableItems._user);
    ui()->userTable->setItem(2, 0, _impl->userInfoTableItems._psw);
    ui()->userTable->setItem(3, 0, _impl->userInfoTableItems._email);
    ui()->userTable->setItem(4, 0, _impl->userInfoTableItems._picture);
    ui()->userTable->setItem(5, 0, _impl->userInfoTableItems._2fa);
    ui()->userTable->setItem(0, 1, _impl->userInfoTableItems._nameValue);
    ui()->userTable->setItem(1, 1, _impl->userInfoTableItems._userValue);
    ui()->userTable->setItem(2, 1, _impl->userInfoTableItems._pswValue);
    ui()->userTable->setItem(3, 1, _impl->userInfoTableItems._emailValue);
    ui()->userTable->setItem(4, 1, _impl->userInfoTableItems._pictureValue);
    ui()->userTable->setCellWidget(5, 1, _impl->userInfoTableItems._checkBoxWidget);

    // First time started
    if (isFirstTimeStarted())
    {

        QIcon addIcon;
        addIcon.addFile(QString::fromUtf8(":/buttons/add"), QSize(), QIcon::Normal, QIcon::Off);
        ui()->addUserBtn->setIcon(addIcon);
        ui()->addUserBtn->setText(tr("Add"));
    }
    else
    {
        QIcon editIcon;
        editIcon.addFile(QString::fromUtf8(":/options/edit"), QSize(), QIcon::Normal, QIcon::Off);
        ui()->addUserBtn->setIcon(editIcon);
        ui()->addUserBtn->setText(tr("Edit"));

        // Open the database to fill the data
        openUserInformationData();
    }
}

void SettingsDialog::onAddUser() noexcept
{
    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("__Session__data");

    bool editingMode = false;
    AddUserDialog dlg(this);
    if (!g_globals->session.user.isEmpty())
    {
        editingMode = true;
        dlg.enableEditOnly(
            _impl->userInfoTableItems._userValue->text(),
            _impl->userInfoTableItems._nameValue->text(),
            _impl->userInfoTableItems._emailValue->text(),
            _impl->userInfoTableItems._pictureValue->data(Qt::UserRole).value<QImage>());
    }

    if (dlg.exec() == QDialog::Accepted)
    {
        // The database will be created
        AddUserDialog::UserInformation ifo = dlg.getUserInformation();

        // Initialize the data in the table
        _impl->userInfoTableItems._userValue->setText(ifo.user);
        _impl->userInfoTableItems._nameValue->setText(ifo.name);
        _impl->userInfoTableItems._emailValue->setText(ifo.mail);

        g_globals->session.user    = ifo.user;
        g_globals->session.display = ifo.name;
        g_globals->session.email   = ifo.mail;

        if (!_impl->userInfoTableItems._pswValue->text().isEmpty())
        {
            _impl->userInfoTableItems._pswValue->setText(QString("*").repeated(15));
            _impl->userInfoTableItems._pswValue->setData(Qt::UserRole, ifo.psw);

            const QString tfaFile = []() -> QString {
                QString data = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
                return QString("%1/f6110cb58f3b").arg(data);
            }();

            QFile file(tfaFile);
            if (!file.open(QIODeviceBase::ReadOnly) && g_globals->session.tfa)
            {
                // Code: 0x00000001 is encryption failed
                // Code: 0x00000002 is a file failure
                QMessageBox::critical(this, tr("Error"), QString(tr("2FA can not be updated.\n2FA will be disabled.\nError: 0x0000001")),
                    QMessageBox::Ok);
                _impl->userInfoTableItems._checkBox->setChecked(false);
            }
            else
            {
                QTextStream stream(&file);
                QString fileData = stream.readAll();
                file.close();

                g_globals->session.userTFA = AESSym::decrypt(fileData, ifo.prevPsw.toLocal8Bit());

                if (!QFile::remove(tfaFile))
                {
                    QMessageBox::critical(this, tr("Error"), QString(tr("2FA can not be updated.\n2FA will be disabled.\nError: 0x0000002")),
                        QMessageBox::Ok);
                    _impl->userInfoTableItems._checkBox->setChecked(false);
                }
                else
                {
                    auto result = AESSym::encrypt(g_globals->session.userTFA.toLocal8Bit(), ifo.psw.toLocal8Bit());
                    QFile file(tfaFile);
                    if (!file.open(QIODeviceBase::WriteOnly) || result.isEmpty())
                    {
                        // Code: 0x00000001 is: encryption failed
                        // Code: 0x00000002 is: a file failure
                        QMessageBox::critical(this, tr("Error"),
                            QString(tr("2FA can not be updated.\n2FA will be disabled.\nError: 0x0000003")),
                            QMessageBox::Ok);
                        _impl->userInfoTableItems._checkBox->setChecked(false);
                    }
                    else
                    {
                        // Store the data
                        QTextStream stream(&file);
                        stream << result;
                        file.close();
                    }
                }
            }
        }

        if (ifo.photoUpdate)
        {
            _impl->userInfoTableItems._pictureValue->setData(Qt::DecorationRole, QPixmap::fromImage(ifo.photograph).scaled(100, 100, Qt::KeepAspectRatio));
            _impl->userInfoTableItems._pictureValue->setData(Qt::UserRole, ifo.photograph);
            g_globals->session.image = ifo.photograph;
            ui()->userTable->setRowHeight(4, 100);
        }

        QIcon editIcon;
        editIcon.addFile(QString::fromUtf8(":/options/edit"), QSize(), QIcon::Normal, QIcon::Off);
        ui()->addUserBtn->setIcon(editIcon);
        ui()->addUserBtn->setText(tr("Edit"));

        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__Session__data");
        settings.setValue("__user__", g_globals->session.user);
        settings.setValue("__display__", g_globals->session.display);
        settings.setValue("__email__", g_globals->session.email);
        if (!editingMode || ifo.pswUpdate)
            settings.setValue("__sec__", QString::fromUtf8(QCryptographicHash::hash(QByteArrayView(ifo.psw.toUtf8()), QCryptographicHash::RealSha3_512).toBase64()));
        settings.endGroup();

        settings.beginGroup("__Session__");
        settings.setValue("__time__", true);
        settings.endGroup();
    }
}

void SettingsDialog::resetTablesValues() noexcept
{
    _impl->userInfoTableItems._nameValue->setText("");
    _impl->userInfoTableItems._userValue->setText("");
    _impl->userInfoTableItems._pswValue->setText("");
    _impl->userInfoTableItems._emailValue->setText("");
    _impl->userInfoTableItems._pictureValue->setText("");
    _impl->userInfoTableItems._checkBox->setChecked(false);

    QIcon addIcon;
    addIcon.addFile(QString::fromUtf8(":/buttons/add"), QSize(), QIcon::Normal, QIcon::Off);
    ui()->addUserBtn->setIcon(addIcon);
    ui()->addUserBtn->setText(tr("Add"));
}

void SettingsDialog::openUserInformationData() noexcept
{
    QSettings settings("CentaurProject", "Centaur");

    const QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    QDir pathDir;
    pathDir.setPath(dataPath);

    settings.beginGroup("__Session__data");
    g_globals->session.user    = settings.value("__user__").toString();
    g_globals->session.display = settings.value("__display__").toString();
    g_globals->session.email   = settings.value("__email__").toString();
    g_globals->session.tfa     = settings.value("__2fa__").toBool();
    settings.endGroup();

    const QString imageFilePath = QString("%1/e30dfd91071c.image.data").arg(dataPath);

    _impl->userInfoTableItems._userValue->setText(g_globals->session.user);
    _impl->userInfoTableItems._nameValue->setText(g_globals->session.display);
    _impl->userInfoTableItems._emailValue->setText(g_globals->session.email);

    g_globals->session.image.load(imageFilePath);
    if (g_globals->session.image.isNull())
        logWarn("session", "user image does not exists");
    else
    {
        _impl->userInfoTableItems._pictureValue->setData(Qt::DecorationRole, QPixmap::fromImage(g_globals->session.image).scaled(100, 100, Qt::KeepAspectRatio));
        _impl->userInfoTableItems._pictureValue->setData(Qt::UserRole, g_globals->session.image);
    }

    ui()->userTable->setRowHeight(4, 100);

    _impl->userInfoTableItems._checkBox->setChecked(g_globals->session.tfa);
}

void SettingsDialog::onAllow2FA(bool checked) noexcept
{
    const QString tfaFile = []() -> QString {
        QString data = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        return QString("%1/f6110cb58f3b").arg(data);
    }();

    if (checked)
    {
        if (_impl->userInfoTableItems._emailValue->text().isEmpty())
        {
            QMessageBox::warning(this, tr("Warning"), tr("e-mail user field could not be empty"), QMessageBox::Ok);
            _impl->userInfoTableItems._checkBox->setChecked(false);
            return;
        }

        if (tfaFile.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("There is an error in the user identification. 2FA is not available"), QMessageBox::Ok);
            _impl->userInfoTableItems._checkBox->setChecked(false);
            return;
        }
        else
        {
            if (QFile::exists(tfaFile))
            {
                int res = QMessageBox::warning(this, tr("Warning"),
                    tr("Currently, there is 2FA registered and the data can not be retrieved for security.\nWould you like to repeat the process?.\nSelecting no the 2FA will be disabled"),
                    QMessageBox::Yes | QMessageBox::No);
                if (res == QMessageBox::Yes)
                {
                    // Remove and continue
                    QFile::remove(tfaFile);
                }
                else
                {
                    // Do not continue and set the checked flag
                    _impl->userInfoTableItems._checkBox->setChecked(false);
                    return;
                }
            }
        }

        // Create a 120-bits random key, shuffled 1000 times
        auto generatedKey = AESSym::createUniqueId(1'000, 15);
        auto base32       = QString::fromStdString(toBase32(generatedKey));

        // Store in the file the key encrypted with the password
        // Since this operation can fail and return, write the file first and if it not fails, just then, show the QR code dialog
        auto userPsw = _impl->userInfoTableItems._pswValue->data(Qt::UserRole).toString();
        if (userPsw.isEmpty())
        {
            LoginDialog dlg(this);
            dlg.setPasswordMode();
            if (dlg.exec() == QDialog::Accepted)
            {
                userPsw          = dlg.userPassword;
                dlg.userPassword = QString::fromStdString(AESSym::createUniqueId(10, 8)); // clear the data with random digits
            }
            else
            {
                QMessageBox::critical(this, "Critical", QString(tr("In order to provide 2FA you must set your password")));
                _impl->userInfoTableItems._checkBox->setChecked(false);
                return;
            }
        }

        auto result = AESSym::encrypt(QString::fromStdString(generatedKey).toLocal8Bit(), userPsw.toLocal8Bit());
        QFile file(tfaFile);
        if (!file.open(QIODeviceBase::WriteOnly) || result.isEmpty())
        {
            // Code: 0x00000001 is: encryption failed
            // Code: 0x00000002 is: a file failure
            QMessageBox::critical(this, tr("Error"),
                QString(tr("There is an error in the user identification. 2FA is not available.\nCode: %1")).arg(result.isEmpty() ? "0x00000001" : "0x00000002"),
                QMessageBox::Ok);
            _impl->userInfoTableItems._checkBox->setChecked(false);
            return;
        }

        // Store the data
        QTextStream stream(&file);
        stream << result;
        file.close();

        // Show the code
        QRCodeDialog qrCode(_impl->userInfoTableItems._emailValue->text(), base32, this);
        qrCode.exec();

        g_globals->session.tfa = true;
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__Session__data");
        settings.setValue("__2fa__", g_globals->session.tfa);
        settings.endGroup();
    }
    else
    {
        g_globals->session.tfa = false;
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__Session__data");
        settings.setValue("__2fa__", g_globals->session.tfa);
        settings.endGroup();
    }
}

void SettingsDialog::setPage(SettingsDialog::Pages page) noexcept
{
    switch (page)
    {
        case Pages::Session: ui()->stackedWidget->setCurrentWidget(ui()->sessionPage); break;
        case Pages::Advanced: ui()->stackedWidget->setCurrentWidget(ui()->advancedPage); break;
        case Pages::Plugins: ui()->stackedWidget->setCurrentWidget(ui()->pluginsPage); break;
    }
}

void SettingsDialog::initAdvancedWidget() noexcept
{
    ui()->memTableView->initialize(ui()->editAdvanceSearch, 3, 0, 2, nullptr, 1);

    ui()->memTableView->sortByColumn(0, Qt::AscendingOrder);
    ui()->memTableView->getModel()->setHorizontalHeaderLabels({ tr("Option"), tr("Value"), { "" } });
    ui()->memTableView->horizontalHeader()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);

    ui()->memTableView->setButtons(OptionsWidget::edit);

    ui()->memTableView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    ui()->memTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui()->memTableView->setGridStyle(Qt::NoPen);
    ui()->memTableView->setSortingEnabled(true);
    ui()->memTableView->sortByColumn(0, Qt::AscendingOrder);

    ui()->memTableView->insertRowWithOptions(0, { _impl->advancedTableItems._pixmapCache, _impl->advancedTableItems._pixmapCacheValue, new QStandardItem() }, false);

    connect(ui()->memTableView, &OptionsTableWidget::editItemPressed, this,
        [&](QStandardItem *item) {
            if (item == _impl->advancedTableItems._pixmapCacheValue)
            {
                int unit = bytesSizeMaxUnit(QPixmapCache::cacheLimit());
                PixmapMemCacheDialog dlg(this);

                dlg.setData(bytesToString(QPixmapCache::cacheLimit(), unit, false), unit);

                if (dlg.exec() == QDialog::Accepted)
                {
                    _impl->advancedTableItems._pixmapCacheValue->setText(bytesToString(dlg.memory));

                    QSettings settings("CentaurProject", "Centaur");
                    settings.beginGroup("advancedSettings-PixmapCache");
                    settings.setValue("size", dlg.memory);
                    settings.endGroup();

                    QPixmapCache::setCacheLimit(dlg.memory);
                }
            }
        });
}

void SettingsDialog::initPluginsWidget() noexcept
{
    using namespace dal;

#ifdef NO_PLUGIN_CHECKSUM_CHECK
    ui()->warningChecksumIconLabel->setVisible(true);
    ui()->warningChecksumLabel->setVisible(true);
#else
    ui()->warningChecksumIconLabel->setVisible(false);
    ui()->warningChecksumLabel->setVisible(false);
#endif

    ui()->pluginsTableWidget->setColumnCount(7);

    ui()->pluginsTableWidget->setItemDelegate(new DelegateItem);

    ui()->pluginsTableWidget->setHorizontalHeaderLabels(
        { QString(tr("Name")),
            QString(tr("Version")),
            QString(tr("Manufacturer")),
            QString(tr("UI Min. Version")),
            QString(tr("Identification")),
            QString(tr("Protected")),
            QString(tr("Loaded")) });

    ui()->pluginsTableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui()->pluginsTableWidget->horizontalHeaderItem(6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    auto plInfo = DataAccess::pluginInformation();

    if (!plInfo.has_value())
    {
        QMessageBox::critical(this, tr("Error"),
            QString(tr("The plugin information could not be retrieved")),
            QMessageBox::Ok);
    }
    else
    {
        for (const auto &data : *plInfo)
        {
            auto row = ui()->pluginsTableWidget->rowCount();

            ui()->pluginsTableWidget->insertRow(row);

            auto *itemName      = new QTableWidgetItem(QIcon(":/ui/settings/plugins/plugin"), data.name);
            auto *itemVersion   = new QTableWidgetItem(data.version);
            auto *itemManu      = new QTableWidgetItem(data.man);
            auto *itemUIVer     = new QTableWidgetItem(data.centaur_uuid);
            auto *itemUUID      = new QTableWidgetItem(data.uuid);
            auto *itemProtected = new QTableWidgetItem(data.protect ? "Yes" : "No");
            auto *itemLoaded    = new QTableWidgetItem(tr("Not Loaded"));

            itemName->setBackground(QColor(255, 0, 0, 25));
            itemVersion->setBackground(QColor(255, 0, 0, 25));
            itemManu->setBackground(QColor(255, 0, 0, 25));
            itemUIVer->setBackground(QColor(255, 0, 0, 25));
            itemUUID->setBackground(QColor(255, 0, 0, 25));
            itemProtected->setBackground(QColor(255, 0, 0, 25));
            itemLoaded->setBackground(QColor(255, 0, 0, 25));

            itemName->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            itemVersion->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemManu->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUIVer->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUUID->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemProtected->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemLoaded->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            itemName->setCheckState(data.enabled ? Qt::Checked : Qt::Unchecked);

            itemName->setData(Qt::UserRole + 1, _settings_plugin_enable_magic);
            itemName->setData(Qt::UserRole + 2, data.uuid);

            itemLoaded->setData(Qt::UserRole + 1, 0);

            itemName->setToolTip(tr("Check for enable the plugin\nUncheck for disabling the plugin"));

            for (auto &loadedPlugins : g_app->getPluginBase())
            {
                if (loadedPlugins->getPluginUUID() != data.uuid)
                {
                    itemLoaded->setText(tr("Loaded"));
                    itemLoaded->setData(Qt::UserRole + 1, 1);

                    itemName->setBackground(QColor(0, 255, 0, 25));
                    itemVersion->setBackground(QColor(0, 255, 0, 25));
                    itemManu->setBackground(QColor(0, 255, 0, 25));
                    itemUIVer->setBackground(QColor(0, 255, 0, 25));
                    itemUUID->setBackground(QColor(0, 255, 0, 25));
                    itemProtected->setBackground(QColor(0, 255, 0, 25));
                    itemLoaded->setBackground(QColor(0, 255, 0, 25));

                    break;
                }
            }

            ui()->pluginsTableWidget->setItem(row, 0, itemName);
            ui()->pluginsTableWidget->setItem(row, 1, itemVersion);
            ui()->pluginsTableWidget->setItem(row, 2, itemManu);
            ui()->pluginsTableWidget->setItem(row, 3, itemUIVer);
            ui()->pluginsTableWidget->setItem(row, 4, itemUUID);
            ui()->pluginsTableWidget->setItem(row, 5, itemProtected);
            ui()->pluginsTableWidget->setItem(row, 6, itemLoaded);
        }
    }

    connect(ui()->pluginsTableWidget, &QTableWidget::itemSelectionChanged, this, [this]() {
        if (ui()->pluginsTableWidget->selectedItems().size() != 1)
        {
            ui()->uninstallButton->setDisabled(true);
            return;
        }
        ui()->uninstallButton->setDisabled(false);
    });

    connect(ui()->pluginsTableWidget, &QTableWidget::itemChanged, this, &SettingsDialog::pluginsTableItemChanged);

    connect(ui()->installButton, &QPushButton::released, this, &SettingsDialog::installPlugin);
    connect(ui()->uninstallButton, &QPushButton::released, this, &SettingsDialog::uninstallPlugin);
}

void SettingsDialog::initShortcutsWidget() noexcept
{
}

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_PUSH()
CENTAUR_WARN_OFF("-Wnullable-to-nonnull-conversion")
#endif

void cen::SettingsDialog::installPlugin() noexcept
{
    using namespace dal;

    QFileDialog dlg(this);

    QSettings settings("CentaurProject", "Centaur");
    settings.beginGroup("settings.Plugins");
    auto lastDir = settings.value("Install.LastDirectory", QString()).toString();
    settings.endGroup();

    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setViewMode(QFileDialog::Detail);

    if (!lastDir.isEmpty())
    {
        dlg.setDirectory(lastDir);
    }

    dlg.setNameFilter("Plugin files (*.cpk *.zip)");

    if (dlg.exec() == QDialog::Accepted)
    {
        settings.beginGroup("settings.Plugins");
        settings.setValue("Install.LastDirectory", dlg.directory().absolutePath());
        settings.endGroup();

        QProgressDialog prgDlg(this);

        prgDlg.setAutoClose(true);
        prgDlg.setRange(0, static_cast<int>(dlg.selectedFiles().size()));
        prgDlg.setValue(0);
        prgDlg.setCancelButton(nullptr);

        QFutureWatcher<void> watcher;

        connect(&watcher, &QFutureWatcher<void>::finished, &prgDlg, &QProgressDialog::reset);
        connect(&watcher, &QFutureWatcher<void>::progressValueChanged, &prgDlg, &QProgressDialog::setValue);
        connect(&watcher, &QFutureWatcher<void>::progressTextChanged, &prgDlg, &QProgressDialog::setLabelText);

        QList<QPair<QString, PluginData>> toTableData;
        QStringList errorStrings;

        watcher.setFuture(
            QtConcurrent::run(QThreadPool::globalInstance(),
                [&]() {
                    const QString &resPath         = g_globals->paths.resPath;
                    const QString schemaJSONPlugin = resPath + "/Schema/plugins.schema.json";
                    // Open the plugin schema
                    namespace json = rapidjson;

                    QFile file(schemaJSONPlugin);
                    if (!file.open(QIODevice::ReadOnly))
                    {
                        errorStrings.push_back(QString(tr("An internal file is missing. Reinstalling the application might solve the problem")));
                        return;
                    }

                    QTextStream textStream(&file);

                    json::Document schemaJSONDoc;
                    schemaJSONDoc.Parse(textStream.readAll().toUtf8().constData());
                    if (schemaJSONDoc.HasParseError())
                    {
                        errorStrings.push_back(QString(tr("An internal file is corrupted. Reinstalling the application might solve the problem")));
                        return;
                    }

                    json::SchemaDocument schemaDoc(schemaJSONDoc);
                    json::SchemaValidator schemaValidator(schemaDoc);

                    int ipv = 0;
                    for (const auto &file : dlg.selectedFiles())
                    {
                        emit watcher.progressTextChanged(file);

                        int z_error      = ZIP_ER_OK;
                        auto zip_archive = zip_open(file.toLatin1().constData(), ZIP_RDONLY, &z_error);

                        if (z_error != ZIP_ER_OK)
                        {
                            errorStrings.push_back(QString(tr("The file %1 is not a valid plugin package")).arg(file));
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto json_index = zip_name_locate(zip_archive, "plugin.json", ZIP_FL_NOCASE | ZIP_FL_ENC_UTF_8);

                        if (json_index == -1)
                        {
                            errorStrings.push_back(QString(tr("%1\ndoes not contain any plugin metadata information")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        zip_stat_t json_stats;

                        auto json_st   = zip_stat_index(zip_archive, static_cast<zip_uint64_t>(json_index), ZIP_STAT_SIZE, &json_stats);
                        auto json_file = zip_fopen_index(zip_archive, static_cast<zip_uint64_t>(json_index), 0);

                        if (json_st == -1 || json_stats.size <= 0)
                        {
                            errorStrings.push_back(QString(tr("%1\nmetadata file is not available")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QScopedArrayPointer<char> _file_bytes(new char[json_stats.size + 1]);
                        auto bytes_read                    = zip_fread(json_file, _file_bytes.get(), json_stats.size);
                        _file_bytes.get()[json_stats.size] = 0;

                        if (bytes_read < 0)
                        {
                            errorStrings.push_back(QString(tr("%1\nmetadata file was not read")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        json::Document jsonDoc;
                        jsonDoc.Parse(_file_bytes.get());
                        if (jsonDoc.HasParseError())
                        {
                            errorStrings.push_back(QString(tr("%1\nmetadata file is not valid")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        if (!jsonDoc.Accept(schemaValidator))
                        {
                            errorStrings.push_back(QString(tr("%1\nmetadata file contain an invalid format")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QString plVersion { jsonDoc["version"].GetString() };

                        // Currently there's only version 0.1.0
                        if (plVersion != "0.1.0")
                        {
                            errorStrings.push_back(QString(tr("%1\nfile format version is not supported")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto exists = DataAccess::pluginExists(QString { jsonDoc["identification"]["uuid"].GetString() });
                        if (exists)
                        {
                            errorStrings.push_back(QString(tr("%1\nhas an unready installed plugin")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto min_ui_version = QString { jsonDoc["identification"]["ui-version"]["min-uuid"].GetString() };

                        // Currently there is only version 0.1.0
                        if (min_ui_version != "a15c48b4-460b-4a79-a0a8-8ece90603f85")
                        {
                            errorStrings.push_back(QString(tr("%1\nis not supported by this version of the UI")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        // Check for the Data integrity
                        auto hash = QString { jsonDoc["identification"]["checksum"].GetString() };

                        auto dynlib_index = zip_name_locate(zip_archive, jsonDoc["identification"]["dynamic"].GetString(), ZIP_FL_NOCASE | ZIP_FL_ENC_UTF_8);

                        if (dynlib_index == -1)
                        {
                            errorStrings.push_back(QString(tr("%1\ndoes not contain any plugin")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        zip_stat_t dynlib_stats;
                        auto dynlib_st   = zip_stat_index(zip_archive, static_cast<zip_uint64_t>(dynlib_index), ZIP_STAT_SIZE, &dynlib_stats);
                        auto dynlib_file = zip_fopen_index(zip_archive, static_cast<zip_uint64_t>(dynlib_index), 0);

                        if (dynlib_st == -1 || dynlib_stats.size <= 0)
                        {
                            errorStrings.push_back(QString(tr("%1\nplugin file is not available")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QScopedArrayPointer<unsigned char> _plugin_bytes(new unsigned char[dynlib_stats.size + 1ull]);
                        bytes_read = zip_fread(dynlib_file, _plugin_bytes.get(), dynlib_stats.size);

                        if (bytes_read < 0)
                        {
                            errorStrings.push_back(QString(tr("%1\nplugin file was not read")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        auto calcHash = QCryptographicHash::hash({ _plugin_bytes.get(), static_cast<qsizetype>(dynlib_stats.size) }, QCryptographicHash::Sha224).toHex();

                        if (calcHash != hash)
                        {
                            errorStrings.push_back(QString(tr("%1\nplugin file is corrupted")).arg(file));
                            zip_close(zip_archive);
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        zip_close(zip_archive);

                        const QString pluginFile = [&]() -> QString {
                            QString data = g_globals->paths.pluginsPath;
                            return QString("%1/%2").arg(data, QString { jsonDoc["identification"]["dynamic"].GetString() });
                        }();

                        QFile toDiscPluginFile(pluginFile);
                        if (!toDiscPluginFile.open(QIODeviceBase::NewOnly | QIODeviceBase::WriteOnly))
                        {
                            errorStrings.push_back(QString(tr("%1\nthe plugin file could not be opened to write")).arg(file));
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        QDataStream dataStream(&toDiscPluginFile);
                        auto nWritten = dataStream.writeRawData(reinterpret_cast<const char *>(_plugin_bytes.get()), static_cast<int>(dynlib_stats.size));
                        if (nWritten < static_cast<int>(dynlib_stats.size))
                        {
                            errorStrings.push_back(QString(tr("%1\nthe plugin file could not be written")).arg(file));
                            emit watcher.progressValueChanged(++ipv);
                            continue;
                        }

                        toDiscPluginFile.close();

                        QFileInfo nfo(pluginFile);
                        toTableData.push_back({
                            pluginFile, {jsonDoc["identification"]["name"].GetString(),
                                         jsonDoc["identification"]["version"].GetString(),
                                         jsonDoc["identification"]["manufacturer"].GetString(),
                                         jsonDoc["identification"]["uuid"].GetString(),
                                         jsonDoc["identification"]["ui-version"]["min-uuid"].GetString(),
                                         hash,
                                         nfo.fileName(),
                                         true,
                                         jsonDoc["identification"]["protected"].GetBool()}
                        });

                        emit watcher.progressValueChanged(++ipv);
                    }
                }));

        prgDlg.exec();

        watcher.waitForFinished();

        if (!errorStrings.empty())
        {
            int i = 0;
            QString errors;
            for (const auto &er : errorStrings)
                errors += QString("%1. %2\n\n").arg(++i).arg(er);

            QMessageBox::critical(this, tr("Error"),
                errors,
                QMessageBox::Ok);
        }

        for (const auto &[pluginFile, data] : toTableData)
        {
            auto insertionResult = DataAccess::insertPlugin(
                data.name,
                data.version,
                data.man,
                data.uuid,
                data.centaur_uuid,
                data.checksum,
                data.dynamic,
                true,
                data.protect);

            if (!insertionResult.has_value())
            {
                QFile::remove(pluginFile);
                QMessageBox::critical(this, tr("Error"),
                    tr("%1\n Could not be installed. Query Error").arg(pluginFile),
                    QMessageBox::Ok);

                continue;
            }

            if (!*insertionResult)
            {

                QMessageBox::critical(this, tr("Error"),
                    tr("%1\nPlugin was not installed").arg(pluginFile),
                    QMessageBox::Ok);

                QFile::remove(pluginFile);
                continue;
            }

            auto row = ui()->pluginsTableWidget->rowCount();

            ui()->pluginsTableWidget->insertRow(row);

            auto uiVersion      = DataAccess::fromUUIDtoVersion(data.centaur_uuid);
            auto *itemName      = new QTableWidgetItem(QIcon(":/ui/settings/plugins/plugin"), data.name);
            auto *itemVersion   = new QTableWidgetItem(data.version);
            auto *itemManu      = new QTableWidgetItem(data.man);
            auto *itemUIVer     = new QTableWidgetItem(uiVersion.has_value() ? *uiVersion : data.centaur_uuid);
            auto *itemUUID      = new QTableWidgetItem(data.uuid);
            auto *itemProtected = new QTableWidgetItem(data.protect ? "Yes" : "No");
            auto *itemLoaded    = new QTableWidgetItem(tr("Not Loaded"));

            itemName->setBackground(QColor(255, 255, 0, 25));
            itemVersion->setBackground(QColor(255, 255, 0, 25));
            itemManu->setBackground(QColor(255, 255, 0, 25));
            itemUIVer->setBackground(QColor(255, 255, 0, 25));
            itemUUID->setBackground(QColor(255, 255, 0, 25));
            itemProtected->setBackground(QColor(255, 255, 0, 25));
            itemLoaded->setBackground(QColor(255, 255, 0, 25));

            itemName->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            itemVersion->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemManu->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUIVer->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemUUID->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemProtected->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemLoaded->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            itemName->setCheckState(data.enabled ? Qt::Checked : Qt::Unchecked);

            itemName->setData(Qt::UserRole + 1, _settings_plugin_enable_magic);
            itemName->setData(Qt::UserRole + 2, data.uuid);

            itemLoaded->setData(Qt::UserRole + 1, -1);

            itemName->setToolTip(tr("Check for enable the plugin\nUncheck for disabling the plugin"));

            ui()->pluginsTableWidget->setItem(row, 0, itemName);
            ui()->pluginsTableWidget->setItem(row, 1, itemVersion);
            ui()->pluginsTableWidget->setItem(row, 2, itemManu);
            ui()->pluginsTableWidget->setItem(row, 3, itemUIVer);
            ui()->pluginsTableWidget->setItem(row, 4, itemUUID);
            ui()->pluginsTableWidget->setItem(row, 5, itemProtected);
            ui()->pluginsTableWidget->setItem(row, 6, itemLoaded);

            _impl->forceShowPluginWarning = true;
            showWarningLabels();
        }
    }
}

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_POP()
#endif

void cen::SettingsDialog::uninstallPlugin() noexcept
{
    using namespace dal;

    auto res = QMessageBox::question(this,
        tr("Uninstall"),
        tr("Are you sure you want to uninstall this plugin?"),
        QMessageBox::Yes | QMessageBox::No);

    if (res == QMessageBox::No)
        return;

    const auto &selectedItem = ui()->pluginsTableWidget->selectedItems().front();

    // Get the UUID item
    auto uuidItem = selectedItem->column() == 4 ? selectedItem : ui()->pluginsTableWidget->item(selectedItem->row(), 4);

    const QString uuid = uuidItem->text();

    const auto dynFile = DataAccess::getDynamicFieldPlugin(uuid);
    if (!dynFile.has_value())
    {
        QMessageBox::critical(this,
            tr("Error"),
            tr("The plugin can not be uninstalled"),
            QMessageBox::Yes);
        return;
    }

    const auto remResult = DataAccess::removePlugin(uuid);

    if (!remResult.has_value() || !*remResult)
    {
        QMessageBox::critical(this,
            tr("Error"),
            tr("The plugin could not be removed from the database"),
            QMessageBox::Yes);
        return;
    }

    ui()->pluginsTableWidget->removeRow(uuidItem->row());
    showWarningLabels();
    _impl->forceShowPluginWarning = true;

    if (!QFile::remove(g_globals->paths.pluginsPath + "/" + *dynFile))
    {
        QMessageBox::critical(this,
            tr("Error"),
            tr("The plugin file could not be deleted from the filesystem.\nReinstalling this plugin can't be done unless the file is removed manually"),
            QMessageBox::Yes);
        return;
    }
}

void cen::SettingsDialog::pluginsTableItemChanged(QTableWidgetItem *item) noexcept
{
    using namespace dal;

    bool valid;
    auto isEnablingItem = item->data(Qt::UserRole + 1).toInt(&valid);

    if (!valid)
        return;

    if (isEnablingItem != _settings_plugin_enable_magic)
        return;

    const auto &uuid = item->data(Qt::UserRole + 2).toString();

    auto result = DataAccess::updateEnabledState(uuid, item->checkState() == Qt::Checked ? true : false);

    if (!result.has_value())
    {
        QMessageBox::critical(this, tr("Error"),
            QString(tr("The action failed to execute")),
            QMessageBox::Ok);

        return;
    }

    if (!*result)
    {
        QMessageBox::critical(this, tr("Error"),
            QString(tr("The plugin information could not be updated")),
            QMessageBox::Ok);

        return;
    }

    auto isLoadedItem = ui()->pluginsTableWidget->item(item->row(), 5);
    if (isLoadedItem == nullptr)
        return;

    if ((isLoadedItem->data(Qt::UserRole + 1).toInt() == 1 && item->checkState() == Qt::Unchecked)
        || (isLoadedItem->data(Qt::UserRole + 1).toInt() == 0 && item->checkState() == Qt::Checked)
        || isLoadedItem->data(Qt::UserRole + 1).toInt() == -1)
    {
        for (int i = 0; i <= 5; ++i)
        {
            auto selItem = ui()->pluginsTableWidget->item(item->row(), i);
            if (selItem != nullptr)
            {
                selItem->setBackground(QColor(255, 255, 0, 25));
            }
        }
    }

    if (isLoadedItem->data(Qt::UserRole + 1).toInt() == 1 && item->checkState() == Qt::Checked)
    {
        for (int i = 0; i <= 5; ++i)
        {
            auto selItem = ui()->pluginsTableWidget->item(item->row(), i);
            if (selItem != nullptr)
            {
                selItem->setBackground(QColor(0, 255, 0, 25));
            }
        }
    }

    if (isLoadedItem->data(Qt::UserRole + 1).toInt() == 0 && item->checkState() == Qt::Unchecked)
    {
        for (int i = 0; i <= 5; ++i)
        {
            auto selItem = ui()->pluginsTableWidget->item(item->row(), i);
            if (selItem != nullptr)
            {
                selItem->setBackground(QColor(255, 0, 0, 25));
            }
        }
    }

    for (int i = 0; i < ui()->pluginsTableWidget->rowCount(); ++i)
    {
        auto selItem = ui()->pluginsTableWidget->item(i, 0);
        if (selItem)
        {
            if (selItem->background().color() == QColor(255, 255, 0, 25))
            {
                showWarningLabels();
                return;
            }
        }
    }

    hideWarningLabels();

    if (_impl->forceShowPluginWarning)
        showWarningLabels();
}

END_CENTAUR_NAMESPACE
