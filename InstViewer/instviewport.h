#ifndef INSTVIEWPORT_H_
#define INSTVIEWPORT_H_
#include <QGraphicsView>
#include "instscene.h"
#include "../DataLoader/dataloader.h"

class InstViewPort : public QGraphicsView {
    Q_OBJECT
public:
    InstViewPort(DataLoader* loader, QWidget* parent = nullptr);
    InstScene* getScene() const { return scene; }

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void adjustSceneToView();

private:
    InstScene* scene;
    DataLoader* loader;
};

#endif
