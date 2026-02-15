#include "graphscene.h"

GraphScene::GraphScene(QObject* parent) : QGraphicsScene(parent) {
    graphItem = new GraphItem();
    addItem(graphItem);
    graphItem->setPos(100, 100);
    connect(graphItem, &GraphItem::rangeChanged, this, [=, this](int start, int end) {
        emit rangeChanged(start, end);
    });
}

void GraphScene::setGraphPoints(const QVector<qreal>& points, int start, int end, bool isPercent) {
    graphItem->setGraphPoints(points, start, end, isPercent);
}
