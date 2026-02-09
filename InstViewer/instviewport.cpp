#include "instviewport.h"
#include <QDebug>

InstViewPort::InstViewPort(DataLoader* loader, QWidget* parent) : QGraphicsView(parent) {
    if (loader == nullptr) {
        return;
    }
    scene = new InstScene(loader, this);

    setMouseTracking(true);
    setScene(scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void InstViewPort::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    adjustSceneToView();
}

void InstViewPort::adjustSceneToView() {
    QSize viewSize = viewport()->size();
    scene->setSceneRect(0, 0, viewSize.width(), viewSize.height());
}


