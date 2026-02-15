#ifndef GRAPHSCENE_H_
#define GRAPHSCENE_H_
#include <QGraphicsScene>
#include "graphitem.h"

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    GraphScene(QObject* parent = nullptr);
    void setGraphPoints(const QVector<qreal>& points, int start, int end, bool isPercent = false);

signals:
    void rangeChanged(int start, int end);

private:
    GraphItem* graphItem;
};

#endif