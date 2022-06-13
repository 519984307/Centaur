/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 09/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "NetworkAddressDialog.hpp"
#include "BinanceSPOT.hpp"
#include <QClipboard>
#include <QSettings>
#include <QToolTip>
#include <fmt/core.h>

cen::NetworkAddressDialog::NetworkAddressDialog(BINAPI_NAMESPACE::BinanceAPISpot *api, binapi::AllCoinsInformation *data, cen::interface::IConfiguration *config, QString coin, QWidget *parent) :
    QDialog { parent },
    m_coin { std::move(coin) },
    m_api { api },
    m_config { config },
    m_ui { new Ui::NetworkAddress }
{
    m_ui->setupUi(this);

    restoreInterfaceState();

    auto copyClipboard = [](QLineEdit *edit) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(edit->text());
        QToolTip::showText(QCursor::pos(), "Copied to the clipboard");
    };

    connect(m_ui->btnClose, &QPushButton::released, this, [&](){saveInterfaceState(); accept(); });
    connect(m_ui->networks, &QComboBox::currentIndexChanged, this, &NetworkAddressDialog::onNetworkChanged);
    connect(m_ui->btnCopyAddress, &QPushButton::released, this, [&]() { copyClipboard(m_ui->lineEditAddress); });
    connect(m_ui->btnCopyTag, &QPushButton::released, this, [&]() { copyClipboard(m_ui->lineEditTag); });
    connect(m_ui->btnCopyURL, &QPushButton::released, this, [&]() { copyClipboard(m_ui->lineEditURL); });

    QPixmap img;
    if (m_config->getSymbolImage(64, 0, m_coin.toUpper(), &img))
        m_ui->image->setPixmap(img);

    m_ui->coinName->setText(m_coin);

    BINAPI_NAMESPACE::SPOT::CoinInformation info = (*data)[m_coin.toStdString()];

    for (const auto &ntwks : info.networkList)
        m_ui->networks->addItem(QString::fromStdString(ntwks.network));

    m_ui->networks->setCurrentIndex(0);
}

cen::NetworkAddressDialog::~NetworkAddressDialog() = default;

void cen::NetworkAddressDialog::onNetworkChanged(int index) noexcept
{
    if (index == -1)
        return;
    if (m_api == nullptr)
        return;

    try
    {
        auto data = m_api->depositAddress(m_coin.toStdString(), m_ui->networks->itemText(index).toStdString());

        m_ui->lineEditAddress->setText(QString::fromStdString(data.address));
        m_ui->lineEditTag->setText(QString::fromStdString(data.tag));
        m_ui->lineEditURL->setText(QString::fromStdString(data.url));
    } catch (const BINAPI_NAMESPACE::APIException &ex)
    {
        switch (ex.type())
        {
            case BINAPI_NAMESPACE::APIException::Type::request:
                {
                    auto request = ex.request();
                    qDebug() << "BinanceSPOTPlugin" << fmt::format("REQUEST.\nCode: {}. URL: {}\n{}\n", std::get<0>(request), std::get<1>(request), std::get<2>(request)).c_str();
                }
                break;
            case BINAPI_NAMESPACE::APIException::Type::limits:
                qDebug() << "BinanceSPOTPlugin" << fmt::format("LIMITS.\n{}\n", std::get<0>(ex.limits())).c_str();
                break;
            case BINAPI_NAMESPACE::APIException::Type::http1:
                {
                    auto http1 = ex.http1();
                    qDebug() << "BinanceSPOTPlugin" << fmt::format("HTTPa.\nCode: {}\nURL: {}\n{}\n", std::get<0>(http1), std::get<1>(http1), std::get<2>(http1)).c_str();
                }
                break;
            case BINAPI_NAMESPACE::APIException::Type::http2:
                {
                    auto http2 = ex.http2();
                    qDebug() << "BinanceSPOTPlugin" << fmt::format("HTTPb.\nStatus: {}. Code: {}\nURL: {}.\n{}\n", std::get<0>(http2), std::get<1>(http2), std::get<2>(http2), std::get<4>(http2)).c_str();
                }
                break;
            case BINAPI_NAMESPACE::APIException::Type::json:
                {
                    auto json = ex.json();
                    qDebug() << "BinanceSPOTPlugin" << fmt::format("JSON.\nURL: {}\n{}\n", std::get<0>(json), std::get<1>(json)).c_str();
                }
                break;
            case BINAPI_NAMESPACE::APIException::Type::api:
                {
                    auto api1 = ex.api();
                    qDebug() << "BinanceSPOTPlugin" << fmt::format("API.\nCode: {}\nURL: {}\n{}", std::get<0>(api1), std::get<1>(api1), std::get<2>(api1)).c_str();
                }
                break;
            case BINAPI_NAMESPACE::APIException::Type::schema:
                {
                    auto schema = ex.schema();
                    qDebug() << "BinanceSPOTPlugin" << fmt::format("SCHEMA.\nURL: {}\n{}\n", std::get<0>(schema), std::get<1>(schema)).c_str();
                }
                break;
        }
    }
}

void cen::NetworkAddressDialog::saveInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("NetworkAddressDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void cen::NetworkAddressDialog::restoreInterfaceState() noexcept
{
    QSettings settings("CentaurProject", "BinanceSPOT");

    settings.beginGroup("NetworkAddressDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}
