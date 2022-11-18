/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 11/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "SquarifyWidget.hpp"
#include <QAbstractTextDocumentLayout>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOptionGraphicsItem>
#include <QTextDocument>

BEGIN_CENTAUR_NAMESPACE

enum class Stack
{
    Vertical,
    Horizontal
};

struct ItemData
{
    ItemData() = default;
    ItemData(QString d, double a, double o) :
        data { std::move(d) },
        area { a },
        orientation { o } { }
    QString data;
    double area;
    double orientation;
};
namespace
{

    struct GraphicsItemSquare : public QGraphicsRectItem
    {
        explicit GraphicsItemSquare(QGraphicsItem *parent = nullptr) :
            QGraphicsRectItem(parent)
        {
        }
        ~GraphicsItemSquare() override = default;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
        {
            painter->setRenderHint(QPainter::RenderHint::Antialiasing);

            QGraphicsRectItem::paint(painter, option, widget);

            painter->save();
            QTextDocument doc;
            doc.setHtml(itemData.data);
            QRectF rc = doc.documentLayout()->frameBoundingRect(doc.rootFrame());
            QPoint translatePoint {
                (static_cast<int>(option->rect.width() - rc.width())) / 2,
                (static_cast<int>(option->rect.height() - rc.height())) / 2
            };
            rc.moveTo(option->rect.topLeft());
            if (option->rect.contains(rc.toRect()))
            {
                painter->translate(option->rect.left() + translatePoint.x(), option->rect.top() + translatePoint.y());
                doc.drawContents(painter);
            }
            painter->restore();
        }

    public:
        void setItemData(const ItemData &data) noexcept
        {
            itemData = data;
        }

    private:
        ItemData itemData;
    };
} // namespace

struct SquarifyWidget::Impl
{
    explicit inline Impl(QGraphicsView *parent) :
        scene { new QGraphicsScene(parent) }
    {
    }
    inline ~Impl() = default;

    void recalculate(const QRectF &rect) noexcept;
    void squarify(QRectF &area, QList<std::pair<QRectF, ItemData>> &rects, qreal lastAR, Stack stack, QList<ItemData> &tree, qsizetype index);

public:
    QGraphicsScene *scene;
    QList<GraphicsItemSquare *> squares;
    QList<ItemData> data;
    QBrush positive;
    QBrush negative;
};

SquarifyWidget::SquarifyWidget(QWidget *parent) :
    QGraphicsView { parent },
    _impl { new Impl(this) }
{
    setScene(_impl->scene);
    connect(_impl->scene, &QGraphicsScene::sceneRectChanged, this, &SquarifyWidget::recalculate);
}

SquarifyWidget::~SquarifyWidget() = default;

void SquarifyWidget::setData(const QList<std::tuple<qreal, qreal, QString>> &data, const QBrush &negative, const QBrush &positive)
{
    _impl->data.clear();

    _impl->positive = positive;
    _impl->negative = negative;

    for (const auto &[price, percent, string] : data)
        _impl->data.emplace_back(string, std::abs(percent), percent > 0 ? 1 : -1);

    std::sort(_impl->data.begin(), _impl->data.end(),
        [](const ItemData &q1, const ItemData &q2) {
            // sort descending and by the data value
            return q1.area > q2.area;
        });

    // Create the items if they don't exist
    // Remove items if the data shrinks
    // Add items if the data grows

    auto currentItems = _impl->scene->items();

    if (currentItems.size() < data.size())
    {
        // Calculate difference
        auto diff = data.size() - currentItems.size();
        for (qsizetype i = 0ll; i < diff; ++i)
            _impl->scene->addItem(new GraphicsItemSquare);
    }
    else if (currentItems.size() > data.size())
    {
        QList<QGraphicsItem *> itemsToRemove;
        auto diff = currentItems.size() - data.size();
        for (qsizetype i = 0ll; i < diff; ++i)
            itemsToRemove.push_back(currentItems[i]);
        for (auto &item : itemsToRemove)
            _impl->scene->removeItem(item);
    } /*
       * else is just equal sizes
       *
       */
}

void SquarifyWidget::resizeEvent(QResizeEvent *event)
{
    QSizeF sizeF { event->size() };
    sizeF.setHeight(sizeF.height() - 10);
    sizeF.setWidth(sizeF.width() - 10);
    _impl->scene->setSceneRect(QRectF({ 0.0, 0.0 }, sizeF));
    QGraphicsView::resizeEvent(event);
}

void SquarifyWidget::recalculate(const QRectF &rect) noexcept
{
    _impl->recalculate(rect);
}

void SquarifyWidget::Impl::recalculate(const QRectF &rect) noexcept
{
    auto totalArea = rect.width() * rect.height();

    // Normalize
    auto sum = std::accumulate(data.begin(), data.end(), 0.0, [](double ac, const ItemData &q1) {
        return ac + q1.area;
    });
    QList<ItemData> map;
    std::for_each(data.begin(), data.end(), [&totalArea, &map, &sum](const ItemData &element) {
        const auto area = (element.area * totalArea) / (sum);
        map.emplace_back(element.data, area, element.orientation);
    });

    // Calculate the first stacking
    QList<std::pair<QRectF, ItemData>> currentStack;
    Stack stack = rect.width() > rect.height() ? Stack::Vertical : Stack::Horizontal;

    if (map.size() == 1)
    {
        dynamic_cast<GraphicsItemSquare *>(scene->items()[0])->setRect(rect);
    }
    else
    {
        QRectF rc = rect;
        squarify(rc, currentStack, 0.0, stack, map, 0);
    }
}

void SquarifyWidget::Impl::squarify(QRectF &area, QList<std::pair<QRectF, ItemData>> &rects, qreal lastAR, Stack stack, QList<ItemData> &tree, qsizetype index)
{
    const auto aspectRatio = [](qreal w, qreal h) {
        return std::max(w / h, h / w);
    };

    const auto height = area.height();
    const auto width  = area.width();

    if (tree.empty() && rects.empty())
        return;

    if (rects.empty())
    {
        QRectF rc {
            QPointF {0, 0},
            QSizeF {0, 0}
        };
        switch (stack)
        {
            case Stack::Vertical:
                // Height is fixed
                rc.setHeight(height);
                rc.setWidth(tree.front().area / height);
                break;
            case Stack::Horizontal:
                // Width is fixed
                rc.setWidth(width);
                rc.setHeight(tree.front().area / width);
                break;
        }

        rects.emplace_back(rc, tree.front());
        lastAR = aspectRatio(rc.width(), rc.height());
        tree.pop_front();
    }
    else
    {
        auto const totalArea = std::accumulate(rects.begin(), rects.end(), 0.0, [](double ac, const std::pair<QRectF, ItemData> &rc) {
            return ac + (rc.first.width() * rc.first.height());
        });

        double newAspectRatio;
        double rcWidth = 0.0, rcHeight = 0.0;
        if (!tree.empty())
        {

            switch (stack)
            {
                case Stack::Vertical:
                    // New width
                    rcWidth  = (totalArea / height);
                    rcHeight = tree.front().area / rcWidth;
                    break;
                case Stack::Horizontal:
                    rcHeight = (totalArea / width);
                    rcWidth  = tree.front().area / rcHeight;
                    break;
            }
            newAspectRatio = aspectRatio(rcHeight, rcWidth);
        }
        else
            newAspectRatio = lastAR + 1; // force to establish the rectangle

        if (newAspectRatio < lastAR || qFuzzyCompare(newAspectRatio, lastAR)) // Let's try to avoid funny weird things with fp
        {
            // qDebug() << rects;
            // No worst
            rects.emplace_back(QRectF {
                                   QPointF {    0.0,      0.0},
                                   QSizeF {rcWidth, rcHeight}
            },
                tree.front());
            lastAR = newAspectRatio;
            tree.pop_front();
        }
        else
        {
            // qDebug() << "RECTS: " << rects;

            // Worst
            // Now we calculate the positions

            QPointF origin = area.topLeft();
            for (auto &[rect, rectData] : rects)
            {
                switch (stack)
                {
                    case Stack::Vertical:
                        // New width
                        rect.moveTo(origin);
                        rcWidth  = (totalArea / height);
                        rcHeight = rect.width() * rect.height() / rcWidth;
                        origin.setY(origin.y() + rcHeight);
                        break;
                    case Stack::Horizontal:
                        rect.moveTo(origin);
                        rcHeight = (totalArea / width);
                        rcWidth  = rect.width() * rect.height() / rcHeight;
                        origin.setX(origin.x() + rcWidth);
                        break;
                }
                rect.setSize({ rcWidth, rcHeight });
                dynamic_cast<GraphicsItemSquare *>(scene->items()[index])->setRect(rect);
                dynamic_cast<GraphicsItemSquare *>(scene->items()[index])->setItemData(rectData);
                if (rectData.orientation > 0)
                    dynamic_cast<GraphicsItemSquare *>(scene->items()[index])->setBrush(positive);
                else
                    dynamic_cast<GraphicsItemSquare *>(scene->items()[index])->setBrush(negative);

                index++;
            }

            switch (stack)
            {
                case Stack::Vertical:
                    area.setX(area.x() + rcWidth);
                    break;
                case Stack::Horizontal:
                    area.setY(area.y() + rcHeight);
                    break;
            }

            lastAR = newAspectRatio;
            stack  = area.width() > area.height() ? Stack::Vertical : Stack::Horizontal;
            // Clear all rects, since we are done with this, row or column
            rects.clear();
        }
    }

    squarify(area, rects, lastAR, stack, tree, index);
}

END_CENTAUR_NAMESPACE
