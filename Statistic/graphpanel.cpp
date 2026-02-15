#include "graphpanel.h"
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include "graphdialog.h"

GraphPanel::GraphPanel(GraphDialog* dialog, DataLoader* loader, QWidget* parent) : QWidget(parent) {
    this->dialog = dialog;
    info = dialog->getDialogInfo();
    this->loader = loader;
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    toolbar = new GraphToolbar(scene, this);
    layout->addWidget(toolbar); 
    connect(toolbar, &GraphToolbar::settingClicked, this, [=, this](){
        dialog->showDialog(this, loader);
    });
    viewport = new GraphViewport(this);
    scene = viewport->getScene();
    layout->addWidget(viewport);
    connect(scene, &GraphScene::rangeChanged, this, [=, this](quint64 start, quint64 end){
        info.start = start;
        info.end = end;
        dialog->setDialogInfo(info);
        QString query = dialog->getQuery(info.start, info.end);
        setQuery(query, info.start, info.end);
    });
    connect(dialog, &GraphDialog::conditionValid, this, [=, this](QWidget* trigger, DataLoader* loader, const QString& query, quint64 start, quint64 end){
        if (trigger == this) {
            setQuery(query, start, end);
        }
    });
}

void GraphPanel::setQuery(const QString& query, quint64 start, quint64 end) {
    QSqlQuery* q = loader->getQuery();
    if (!q) {
        qDebug() << "getQuery failed";
        return;
    }
    QString prepareQuery = query + "AND " + loader->getPrimaryKey() + " BETWEEN ? AND ?";
    if (!q->prepare(prepareQuery)) {
        qDebug() << "prepare failed" << q->lastError().text() << prepareQuery;
        return;
    }
    quint64 cur_start = start;
    quint64 step = (end - start) / 100;
    QVector<qreal> points;
    for (int i = 0; i < 100; i++) {
        q->addBindValue(cur_start);
        cur_start += step;
        q->addBindValue(cur_start);
        if (!q->exec()) {
            qDebug() << "exec failed" << q->lastError().text();
            return;
        }
        while (q->next()) {
            points.append(q->value(0).toULongLong());
        }
    }
    scene->setGraphPoints(points, start, end);
}