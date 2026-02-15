#ifndef GRAPHVIEWPORT_H_
#define GRAPHVIEWPORT_H_
#include <QGraphicsView>
#include "graphscene.h"

class GraphViewport : public QGraphicsView {
    Q_OBJECT
public:
    GraphViewport(QWidget* parent = nullptr);
    GraphScene* getScene() const { return scene; }
    
protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void adjustSceneToView();
private:
    GraphScene* scene;
};

#endif