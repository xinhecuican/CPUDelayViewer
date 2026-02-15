#include "graphviewport.h"

GraphViewport::GraphViewport(QWidget* parent) : QGraphicsView(parent) {
    scene = new GraphScene(this);
    setScene(scene);
    setMouseTracking(true);
}

void GraphViewport::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    adjustSceneToView();
}

void GraphViewport::adjustSceneToView() {
    QSize viewSize = viewport()->size();
    scene->setSceneRect(0, 0, viewSize.width(), viewSize.height());
}