/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 11/11/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "SquarifyWidget.hpp"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>

BEGIN_CENTAUR_NAMESPACE

enum class Stack
{
    Vertical,
    Horizontal
};

struct SquarifyWidget::Impl
{
    explicit inline Impl(QGraphicsView *parent) :
        scene { new QGraphicsScene(parent) }
    {
    }
    inline ~Impl() = default;

    void recalculate(const QRectF &rect) noexcept;
    void squarify(QRectF &area, QList<QRectF> &rects, qreal lastAR, Stack stack, QList<double> &tree, qsizetype index);

public:
    QGraphicsScene *scene;
    QList<QGraphicsRectItem *> squares;
    QList<qreal> data;
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

void SquarifyWidget::setData(const QList<std::pair<qreal, QString>> &data, const QBrush &negative, const QBrush &positive)
{

    for (const auto &[value, string] : data)
        _impl->data.push_back(std::abs(value));

    std::sort(_impl->data.begin(), _impl->data.end(),
        [](qreal q1, qreal q2) {
            // sort descending and by the data value
            return q1 > q2;
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
            _impl->scene->addItem(new QGraphicsRectItem);
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

    for (auto &item : _impl->scene->items())
    {
        dynamic_cast<QGraphicsRectItem *>(item)->setBrush(negative);
    }
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

    auto sum = std::accumulate(data.begin(), data.end(), 0.0);
    QList<double> map;
    std::for_each(data.begin(), data.end(), [&totalArea, &map, &sum](qreal element) {
        const auto area = (element * totalArea) / (sum);
        map.push_back(area);
    });

    // Calculate the first stacking
    QList<QRectF> currentStack;
    Stack stack = rect.width() > rect.height() ? Stack::Vertical : Stack::Horizontal;

    if (map.size() == 1)
    {
        dynamic_cast<QGraphicsRectItem *>(scene->items()[0])->setRect(rect);
    }
    else
    {
        QRectF rc = rect;
        squarify(rc, currentStack, 0.0, stack, map, 0);
    }
}

void SquarifyWidget::Impl::squarify(QRectF &area, QList<QRectF> &rects, qreal lastAR, Stack stack, QList<double> &tree, qsizetype index)
{
    const auto aspectRatio = [](qreal w, qreal h) {
        return std::max(w / h, h / w);
    };

    const auto height = area.height();
    const auto width  = area.width();

    if (tree.empty() && rects.empty())
        return;

    /*
        if (tree.size() == 1)
        {
            dynamic_cast<QGraphicsRectItem *>(scene->items()[index])->setRect(area);
            tree.clear();
        }
        else
        {*/
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
                rc.setWidth(tree.front() / height);
                break;
            case Stack::Horizontal:
                // Width is fixed
                rc.setWidth(width);
                rc.setHeight(tree.front() / width);
                break;
        }

        rects.push_back(rc);
        lastAR = aspectRatio(rc.width(), rc.height());
        tree.pop_front();
    }
    else
    {
        auto const totalArea = std::accumulate(rects.begin(), rects.end(), 0.0, [](double ac, const QRectF &rc) {
            return ac + (rc.width() * rc.height());
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
                    rcHeight = tree.front() / rcWidth;
                    break;
                case Stack::Horizontal:
                    rcHeight = (totalArea / width);
                    rcWidth  = tree.front() / rcHeight;
                    break;
            }
            newAspectRatio = aspectRatio(rcHeight, rcWidth);
        }
        else
            newAspectRatio = lastAR + 1; // force to layout the rectangle

        if (newAspectRatio < lastAR || qFuzzyCompare(newAspectRatio, lastAR)) // Let's try to avoid funny weird things with fp
        {
            // qDebug() << rects;
            // No worst
            rects.push_back(QRectF {
                QPointF {    0.0,      0.0},
                QSizeF {rcWidth, rcHeight}
            });
            lastAR = newAspectRatio;
            tree.pop_front();
        }
        else
        {
            // qDebug() << "RECTS: " << rects;

            // Worst
            // Now we calculate the positions

            QPointF origin = area.topLeft();
            for (auto &rect : rects)
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
                dynamic_cast<QGraphicsRectItem *>(scene->items()[index])->setRect(rect);
                index++;
                qDebug() << "RECT: " << rect;
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
    //  }

    squarify(area, rects, lastAR, stack, tree, index);
}

END_CENTAUR_NAMESPACE
