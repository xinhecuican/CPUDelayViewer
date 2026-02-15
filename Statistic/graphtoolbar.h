#ifndef GRAPHTOOLBAR_H
#define GRAPHTOOLBAR_H
#include <QToolBar>
#include "graphscene.h"

class GraphToolbar : public QToolBar {
    Q_OBJECT
public:
    GraphToolbar(GraphScene* scene, QWidget* parent = nullptr);

signals:
    void settingClicked();

private:
    GraphScene* scene;
};

#endif