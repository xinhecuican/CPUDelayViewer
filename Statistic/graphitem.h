#ifndef GRAPHITEM_H_
#define GRAPHITEM_H_
#include <QGraphicsItem>
#include "coordinateitem.h"
#include <QPainterPath>

class GraphItem : public QObject, public QGraphicsItem {
    Q_OBJECT
public:
    GraphItem(QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    void setGraphPoints(QVector<qreal> points, int start, int end, bool isPercent = false);

signals:
    void rangeChanged(int start, int end);

private:
    CoordinateItem* coordinateItem;
    QPainterPath graphPath;
};

#endif