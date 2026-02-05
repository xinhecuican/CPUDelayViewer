#include "instviewport.h"
#include <QDebug>

InstViewPort::InstViewPort(const QString& model, const QString& path, QWidget* parent) : QGraphicsView(parent) {
    loader = qobject_cast<DataLoader*>(DataLoader::loader_meta[model].newInstance());
    if (loader == nullptr) {
        qDebug() << "Failed to create loader:" << model;
        return;
    }
    loader->setParent(this);
    loader->load(path);
    scene = new InstScene(loader, this);
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


