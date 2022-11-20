/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 19/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "PortfolioWidget.hpp"
#include "PortfolioModel.hpp"
#include <QApplication>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>

BEGIN_CENTAUR_NAMESPACE

namespace
{
    class PortfolioItemDelegate : public QStyledItemDelegate
    {
    public:
        explicit PortfolioItemDelegate(QObject *parent = nullptr) :
            QStyledItemDelegate(parent),
            symbolNameFont { QApplication::font() },
            symbolSourceFont { QApplication::font() },
            symbolShortNameFont { QApplication::font() },
            balanceFont { QApplication::font() },
            linkFont { QApplication::font() },
            symbolNameMetrics { symbolNameFont },
            sourceMetrics { symbolSourceFont },
            shortNameMetrics { symbolShortNameFont },
            symbolNameFontHeight { 13 },
            symbolShortNameFontHeight { 13 },
            redPen { QColor(215, 98, 52) },
            greenPen { QColor(0, 255, 0) },
            greenDiffPen { QColor(0, 255, 0, 200) },
            redDiffPen { QColor(255, 129, 112, 200) },
            grayPen { QColor(238, 238, 238) },
            yellowPen { QColor(255, 215, 0) },
            selectedPen { QColor(185, 214, 238) }
        {
            symbolNameFont.setWeight(QFont::Weight::Black);
            symbolNameFont.setPointSize(15);

            symbolShortNameFont.setWeight(QFont::Weight::Medium);
            symbolShortNameFont.setPointSize(12);

            symbolSourceFont.setWeight(QFont::Weight::Light);
            symbolSourceFont.setPointSize(11);

            balanceFont.setWeight(QFont::Weight::Medium);
            balanceFont.setPointSize(12);

            linkFont.setWeight(QFont::Weight::Medium);
            linkFont.setPointSize(11);

            symbolNameMetrics    = QFontMetrics(symbolNameFont);
            symbolNameFontHeight = symbolNameMetrics.height();

            sourceMetrics = QFontMetrics(symbolSourceFont);

            shortNameMetrics          = QFontMetrics(symbolShortNameFont);
            symbolShortNameFontHeight = shortNameMetrics.height();
        }

    public:
        void paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const override
        {
            QStyleOptionViewItem option = opt;
            initStyleOption(&option, index);

            painter->setRenderHint(QPainter::Antialiasing);

            // qDebug() << option.rect << option.text << option.icon;
            painter->save();

            bool useSelectedPen = false;
            // Draw the hover item
            if (option.state & QStyle::State_MouseOver)
            {
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(255, 255, 255, 25));
                painter->drawRoundedRect(option.rect, 5, 5);
                painter->restore();
            }

            if (option.state & QStyle::State_Selected)
            {
                painter->save();
                painter->setPen(Qt::NoPen);
                if (option.state & QStyle::State_Active)
                {
                    painter->setBrush(QColor(255, 255, 255, 35));
                    useSelectedPen = true;
                }
                else
                    painter->setBrush(QColor(255, 255, 255, 20));
                painter->drawRoundedRect(option.rect, 5, 5);
                painter->restore();
            }

            // Set the item margin
            option.rect = option.rect.marginsRemoved(QMargins(5, 5, 5, 5));

            // Draw the icon
            QRect iconRect = option.rect;
            iconRect.setHeight(32);
            iconRect.setWidth(32);

            // Move the rect to skip the icon, even if not exists
            option.rect.setLeft(iconRect.width() + 8 + 3); // leave a space between the text and the icon

            // Calculate all rects
            QRect mainSymbolNameRect  = option.rect,
                  symbolSourceRect    = option.rect,
                  symbolShortNameRect = option.rect,
                  balanceRect         = option.rect,
                  balanceLinkRect     = option.rect;

            // Main name rect
            const QString symbolNameString   = index.data(PortfolioModel::SymbolNameRole).toString();
            const int symbolNameDisplacement = symbolNameMetrics.horizontalAdvance(symbolNameString);
            mainSymbolNameRect.setRight(mainSymbolNameRect.left() + symbolNameDisplacement);
            mainSymbolNameRect.setHeight(symbolNameFontHeight);

            // Balance rect top and height
            const QString balanceString = index.data(PortfolioModel::BalanceRole).toString();
            balanceRect.setRight(balanceRect.right() - 5);
            balanceRect.setLeft(mainSymbolNameRect.right() + 5);
            balanceRect.setBottom(mainSymbolNameRect.bottom());
            balanceRect.setTop(mainSymbolNameRect.top());

            // Short name rect
            const QString shortNameString   = index.data(PortfolioModel::SymbolShortRole).toString();
            const int shortNameDisplacement = shortNameMetrics.horizontalAdvance(shortNameString);
            symbolShortNameRect.setTop(mainSymbolNameRect.bottom() + 5);
            symbolShortNameRect.setLeft(symbolShortNameRect.left() + 5);
            symbolShortNameRect.setHeight(symbolShortNameFontHeight);
            symbolShortNameRect.setRight(symbolShortNameRect.left() + shortNameDisplacement);

            // Source name rect
            const QString sourceNameString   = index.data(PortfolioModel::SourceRole).toString();
            const int sourceNameDisplacement = sourceMetrics.horizontalAdvance(sourceNameString);
            symbolSourceRect.setLeft(symbolShortNameRect.right() + 8);
            symbolSourceRect.setRight(symbolSourceRect.left() + sourceNameDisplacement + 4);
            symbolSourceRect.setBottom(symbolShortNameRect.bottom());
            symbolSourceRect.setTop(symbolShortNameRect.top());

            // Link rect
            const QString linkString = index.data(PortfolioModel::LinkRole).toString();
            balanceLinkRect.setRight(balanceLinkRect.right() - 5);
            balanceLinkRect.setLeft(symbolSourceRect.right() + 5);
            balanceLinkRect.setTop(symbolShortNameRect.top());
            balanceLinkRect.setBottom(symbolShortNameRect.bottom());

            auto icon = index.data(PortfolioModel::IconRole).value<QPixmap>();
            if (!icon.isNull())
                painter->drawPixmap(iconRect, icon);

            painter->save();
            painter->setFont(symbolNameFont);
            // painter->setBrush(QColor(0, 80, 80));
            // painter->drawRect(mainSymbolNameRect);
            if (useSelectedPen)
                painter->setPen(selectedPen);
            painter->drawText(mainSymbolNameRect, index.data(Qt::DisplayRole).toString());

            painter->setFont(symbolSourceFont);
            painter->setBrush(QColor(242, 190, 135));
            painter->setPen(Qt::NoPen);
            const int radius = symbolSourceRect.height() / 2;
            painter->drawRoundedRect(symbolSourceRect, radius, radius);
            painter->setPen(Qt::black);
            painter->drawText(symbolSourceRect, Qt::AlignVCenter | Qt::AlignHCenter, sourceNameString);
            painter->restore();

            painter->save();
            painter->setFont(balanceFont);
            // painter->setBrush(QColor(0, 80, 0));
            // painter->drawRect(balanceRect);
            painter->setPen(greenPen);
            painter->drawText(balanceRect, Qt::AlignVCenter | Qt::AlignRight, balanceString);

            painter->setFont(linkFont);
            // painter->setBrush(QColor(80, 80, 0));
            // painter->drawRect(balanceLinkRect);
            painter->drawText(balanceLinkRect, Qt::AlignVCenter | Qt::AlignRight, linkString);
            painter->restore();

            painter->setFont(symbolShortNameFont);
            // painter->setBrush(QColor(80, 0, 80));
            // painter->drawRect(symbolShortNameRect);
            painter->drawText(symbolShortNameRect, shortNameString);

            painter->restore();
        }

        C_NODISCARD QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            QSize size = QStyledItemDelegate::sizeHint(option, index);
            size.setHeight(48);

            return size;
        }

    protected:
        QFont symbolNameFont;
        QFont symbolSourceFont;
        QFont symbolShortNameFont;
        QFont balanceFont;
        QFont linkFont;

        QFontMetrics symbolNameMetrics;
        QFontMetrics sourceMetrics;
        QFontMetrics shortNameMetrics;

        int symbolNameFontHeight;
        int symbolShortNameFontHeight;

        QPen redPen;
        QPen greenPen;
        QPen greenDiffPen;
        QPen redDiffPen;
        QPen grayPen;
        QPen yellowPen;
        QPen selectedPen;
    };
} // namespace

struct PortfolioWidget::Impl
{
    inline Impl()  = default;
    inline ~Impl() = default;

    PortfolioModel *sourceModel { nullptr };
    QItemSelectionModel *selectionModel { nullptr };

    QModelIndex mouseHoverIndex {};
    QSortFilterProxyModel *sortProxyModel { nullptr };
};

PortfolioWidget::PortfolioWidget(QWidget *parent) :
    QListView { parent },
    _impl { new Impl }
{
    setMouseTracking(true);
    setItemDelegate(new PortfolioItemDelegate(this));

    _impl->sourceModel    = new PortfolioModel(this);
    _impl->sortProxyModel = new QSortFilterProxyModel(this);
    _impl->sortProxyModel->setSourceModel(_impl->sourceModel);
    _impl->sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->setModel(_impl->sortProxyModel);
    _impl->selectionModel = new QItemSelectionModel(_impl->sortProxyModel, this);
    this->setSelectionModel(_impl->selectionModel);
}

PortfolioWidget::~PortfolioWidget() = default;

void PortfolioWidget::linkSearchEdit(QLineEdit *edit) noexcept
{
    auto proxy = qobject_cast<QSortFilterProxyModel *>(model());
    connect(edit, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);
}

void PortfolioWidget::insertItem(const QPixmap &icon, const QString &symbol, const QString &source, const QString &shortName, const QString &link, const QString &balance) noexcept
{
    _impl->sourceModel->insertPortfolioElement(icon, symbol, source, shortName, link, balance);
}

void PortfolioWidget::updateBalance(const QString &symbol, const QString &source, const QString &balance) noexcept
{
    _impl->sourceModel->updateBalance(symbol, source, balance);
}

std::pair<QString, QString> PortfolioWidget::sourceFromPoint(const QPoint &pt)
{
    auto proxyIndex = indexAt(pt);
    if (!proxyIndex.isValid())
        return {};
    auto index = _impl->sortProxyModel->mapToSource(proxyIndex);
    return _impl->sourceModel->sourceFromIndex(index);
}

void PortfolioWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::MouseButton::RightButton)
        emit mouseRightClick(event->pos());

    QListView::mouseReleaseEvent(event);
}
void PortfolioWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto proxyIndex = indexAt(event->pos());

    if (!proxyIndex.isValid())
    {
        _impl->mouseHoverIndex = QModelIndex();
        emit itemHover("", "");
        return;
    }

    auto index = _impl->sortProxyModel->mapToSource(proxyIndex);

    if (!index.isValid())
        return;

    if (index != _impl->mouseHoverIndex)
    {
        const auto &[symbol, source] = _impl->sourceModel->sourceFromIndex(index);
        _impl->mouseHoverIndex       = index;
        emit itemHover(symbol, source);
    }

    QListView::mouseMoveEvent(event);
}
void PortfolioWidget::leaveEvent(QEvent *event)
{
    _impl->mouseHoverIndex = QModelIndex();
    emit mouseLeave();
    QWidget::leaveEvent(event);
}

END_CENTAUR_NAMESPACE
