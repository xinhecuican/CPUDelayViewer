#include "graphitem.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneWheelEvent>

GraphItem::GraphItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
    coordinateItem = new CoordinateItem(this);
    coordinateItem->setTicker(100, 50);
    coordinateItem->setAnnotation(10, 5);
    coordinateItem->setLabels("x轴", "y轴");
}

QRectF GraphItem::boundingRect() const {
    return coordinateItem->boundingRect();
}

void GraphItem::setGraphPoints(QVector<qreal> points, int start, int end, bool isPercent) {
    graphPath.clear();
    QRectF sceneRect = scene()->sceneRect();
    qreal maxHeight = sceneRect.height() * 0.8 < 600 ? 600 : sceneRect.height() * 0.8;
    int pointMinWidth = points.size() * 20;
    qreal maxWidth = sceneRect.width() * 0.9 < pointMinWidth ? pointMinWidth : sceneRect.width() * 0.9;
    int xstep = maxWidth / points.size();
    qreal barWidth = xstep * 0.8;
    qreal max_percent = 0;
    if (!isPercent) {
        qreal max_point = 0;
        qreal all = 0;
        for (int i = 0; i < points.size(); i++) {
            max_point = points[i] > max_point ? points[i] : max_point;
            all += points[i];
        }
        for (int i = 0; i < points.size(); i++) {
            points[i] = points[i] / max_point;
        }
        max_percent = max_point / all;
        
    } else {
        for (int i = 0; i < points.size(); i++) {
            max_percent = points[i] > max_percent ? points[i] : max_percent;
        }
    }
    for (int i = 0; i < points.size(); i++) {
        qreal barHeight = points[i] * maxHeight;
        qreal x = i * xstep + (xstep - barWidth) / 2;
        graphPath.addRect(x, 0, barWidth, barHeight);
    }
    coordinateItem->setCoordinate(maxWidth, maxHeight, start, end, 0, max_percent);
    update();
}

void GraphItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(0x69c0ff), 1));
    painter->setBrush(QColor(0x69c0ff));
    painter->drawPath(graphPath);
}

void GraphItem::wheelEvent(QGraphicsSceneWheelEvent* event) {
    QPointF pos = event->pos();
    if (pos.x() < 0 || pos.x() > coordinateItem->width) return;

    qreal range = (coordinateItem->xend - coordinateItem->xstart) * 0.1;
    if (event->delta() > 0) {
        qreal start = coordinateItem->xstart + pos.x() / coordinateItem->width * range;
        qreal end = coordinateItem->xend - pos.x() / coordinateItem->width * range;
        emit rangeChanged(start, end);
    } else {
        qreal start = coordinateItem->xstart - pos.x() / coordinateItem->width * range;
        qreal end = coordinateItem->xend + pos.x() / coordinateItem->width * range;
        emit rangeChanged(start, end);
    }
    update();
}