/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 17/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "WatchlistWidget.hpp"
#include "WatchListModel.hpp"
#include <QApplication>
#include <QFont>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QStyledItemDelegate>

BEGIN_CENTAUR_NAMESPACE
namespace
{
    class WatchListItemDelegate : public QStyledItemDelegate
    {
    public:
        explicit WatchListItemDelegate(QObject *parent = nullptr) :
            QStyledItemDelegate(parent),
            symbolNameFont { QApplication::font() },
            priceFont { QApplication::font() },
            latencyFont { QApplication::font() },
            diffFont { QApplication::font() },
            diffMetrics { diffFont },
            latencyMetrics { latencyFont },
            symbolNameFontHeight { 13 },
            priceFontHeight { 13 },
            diffFontHeight { 13 },
            redPen { QColor(215, 98, 52) },
            greenPen { QColor(0, 255, 0) },
            greenDiffPen { QColor(0, 255, 0, 200) },
            redDiffPen { QColor(255, 129, 112, 200) },
            grayPen { QColor(238, 238, 238) },
            yellowPen { QColor(255, 215, 0) },
            selectedPen { QColor(185, 214, 238) }
        {
            symbolNameFont.setWeight(QFont::Weight::Black);
            symbolNameFont.setPointSize(13);

            priceFont.setWeight(QFont::Weight::Medium);
            priceFont.setPointSize(12);

            diffFont.setWeight(QFont::Weight::Medium);
            diffFont.setPointSize(11);

            latencyFont.setWeight(QFont::Weight::Light);
            latencyFont.setPointSize(10);

            QFontMetrics nameMetrics(symbolNameFont);
            symbolNameFontHeight = nameMetrics.height();

            QFontMetrics priceMetrics(priceFont);
            priceFontHeight = priceMetrics.height();

            diffMetrics    = QFontMetrics(diffFont);
            diffFontHeight = diffMetrics.height();

            latencyMetrics    = QFontMetrics(latencyFont);
            latencyFontHeight = latencyMetrics.height();
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
            QRect mainSymbolNameRect = option.rect,
                  priceRect          = option.rect,
                  latencyRect        = option.rect,
                  diffRect           = option.rect;

            const double diff  = index.data(WatchlistModel::DiffRole).toReal();
            QString diffString = QString("%1 %").arg(QLocale(QLocale::English).toString(diff, 'f', 2));

            int horizontalDiffDisplacement = diffMetrics.horizontalAdvance(diffString);
            diffRect.setRight(diffRect.right() - 5);
            diffRect.setLeft(diffRect.right() - horizontalDiffDisplacement);

            // Main name rect
            mainSymbolNameRect.setRight(diffRect.left() - 10);
            mainSymbolNameRect.setHeight(symbolNameFontHeight);

            // Diff rect top and height
            diffRect.setBottom(mainSymbolNameRect.bottom());
            diffRect.setTop(diffRect.bottom() - diffFontHeight);

            // Price rect
            priceRect.setTop(priceRect.top() + symbolNameFontHeight + 2);
            priceRect.setLeft(priceRect.left() + 15);
            priceRect.setHeight(priceFontHeight);
            priceRect.setRight(diffRect.left() - 10);

            // Latency rect
            const qint64 latency              = index.data(WatchlistModel::LatencyRole).toLongLong();
            QString latencyText               = QString("%1 ms").arg(latency);
            int horizontalLatencyDisplacement = latencyMetrics.horizontalAdvance(latencyText);
            latencyRect.setRight(diffRect.right());
            latencyRect.setLeft(diffRect.right() - horizontalLatencyDisplacement);
            latencyRect.setTop(latencyRect.bottom() - latencyFontHeight);
            latencyRect.setHeight(latencyFontHeight);

            auto icon = index.data(WatchlistModel::IconRole).value<QPixmap>();
            if (!icon.isNull())
                painter->drawPixmap(iconRect, icon);

            painter->setFont(symbolNameFont);
            // painter->setBrush(QColor(0, 80, 80));
            // painter->drawRect(mainSymbolNameRect);
            if (useSelectedPen)
                painter->setPen(selectedPen);
            painter->drawText(mainSymbolNameRect, index.data(Qt::DisplayRole).toString());

            painter->setFont(priceFont);
            // painter->setBrush(QColor(0, 80, 0));
            // painter->drawRect(priceRect);

            const double price     = index.data(WatchlistModel::PriceRole).toReal();
            const double lastPrice = index.data(WatchlistModel::LastPriceRole).toReal();

            if (price > lastPrice)
                painter->setPen(greenPen);
            else if (price < lastPrice)
                painter->setPen(redPen);
            else
                painter->setPen(grayPen);

            painter->drawText(priceRect, "$ " + QLocale(QLocale::English).toString(price, 'f', 2));

            painter->setFont(diffFont);
            // painter->setBrush(QColor(80, 0, 80));
            // painter->drawRect(diffRect);
            if (diff >= 0.0)
                painter->setPen(greenDiffPen);
            else
                painter->setPen(redDiffPen);

            painter->drawText(diffRect, diffString);

            painter->setFont(latencyFont);
            // painter->setBrush(QColor(80, 80, 0));
            // painter->drawRect(latencyRect);
            if (latency <= 85)
            {
                painter->setPen(greenPen);
            }
            else if (latency >= 86 && latency <= 185)
            {
                painter->setPen(yellowPen);
            }
            else if (latency >= 186 && latency <= 10000000)
            {
                painter->setPen(redPen);
            }

            painter->drawText(latencyRect, latencyText);

            painter->restore();
        }

        C_NODISCARD QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            QSize size = QStyledItemDelegate::sizeHint(option, index);
            size.setHeight(42);

            return size;
        }

    protected:
        QFont symbolNameFont;
        QFont priceFont;
        QFont latencyFont;
        QFont diffFont;

        QFontMetrics diffMetrics;
        QFontMetrics latencyMetrics;

        int symbolNameFontHeight;
        int priceFontHeight;
        int diffFontHeight;
        int latencyFontHeight;

        QPen redPen;
        QPen greenPen;
        QPen greenDiffPen;
        QPen redDiffPen;
        QPen grayPen;
        QPen yellowPen;
        QPen selectedPen;
    };
} // namespace

struct WatchlistWidget::Impl
{
    inline Impl()  = default;
    inline ~Impl() = default;

    WatchlistModel *sourceModel { nullptr };
    QItemSelectionModel *selectionModel { nullptr };

    QModelIndex mouseHoverIndex {};
    QSortFilterProxyModel *sortProxyModel { nullptr };
};

WatchlistWidget::WatchlistWidget(QWidget *parent) :
    QListView { parent },
    _impl { new Impl }
{
    setMouseTracking(true);
    setItemDelegate(new WatchListItemDelegate(this));

    _impl->sourceModel    = new WatchlistModel(this);
    _impl->sortProxyModel = new QSortFilterProxyModel(this);
    _impl->sortProxyModel->setSourceModel(_impl->sourceModel);
    _impl->sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->setModel(_impl->sortProxyModel);
    _impl->selectionModel = new QItemSelectionModel(_impl->sortProxyModel, this);
    this->setSelectionModel(_impl->selectionModel);
}

WatchlistWidget::~WatchlistWidget() = default;

void WatchlistWidget::insertItem(const QPixmap &icon, const QString &symbol, const QString &source, qreal price, qreal diff, qint64 latency) noexcept
{
    _impl->sourceModel->insertWatchListElement(icon, symbol, source, price, diff, latency);
}

void WatchlistWidget::linkSearchEdit(QLineEdit *edit) noexcept
{
    auto proxy = qobject_cast<QSortFilterProxyModel *>(model());
    connect(edit, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);
}

void WatchlistWidget::updatePrice(const QString &symbol, const QString &source, qreal price) noexcept
{
    _impl->sourceModel->updatePrice(symbol, source, price);
}

void WatchlistWidget::updateDifference(const QString &symbol, const QString &source, qreal diff) noexcept
{
    _impl->sourceModel->updateDiff(symbol, source, diff);
}

void WatchlistWidget::updateLatency(const QString &symbol, const QString &source, qint64 latency) noexcept
{
    _impl->sourceModel->updateLatency(symbol, source, latency);
}

void WatchlistWidget::updatePriceAndLatency(const QString &symbol, const QString &source, qreal price, qint64 latency) noexcept
{
    _impl->sourceModel->updatePriceAndLatency(symbol, source, price, latency);
}

void WatchlistWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::MouseButton::RightButton)
        emit mouseRightClick(event->pos());

    QListView::mouseReleaseEvent(event);
}

std::pair<QString, QString> WatchlistWidget::sourceFromPoint(const QPoint &pt)
{
    auto proxyIndex = indexAt(pt);
    if (!proxyIndex.isValid())
        return {};
    auto index = _impl->sortProxyModel->mapToSource(proxyIndex);
    return _impl->sourceModel->sourceFromIndex(index);
}

void WatchlistWidget::removeItem(const QString &symbol, const QString &source) noexcept
{
    _impl->sourceModel->removeItem(symbol, source);
}

void WatchlistWidget::mouseMoveEvent(QMouseEvent *event)
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

void WatchlistWidget::leaveEvent(QEvent *event)
{
    _impl->mouseHoverIndex = QModelIndex();
    emit mouseLeave();
    QWidget::leaveEvent(event);
}

END_CENTAUR_NAMESPACE
