#ifndef INSTVIEWPORT_H_
#define INSTVIEWPORT_H_
#include <QGraphicsView>
#include "instscene.h"
#include "../DataLoader/dataloader.h"

class InstViewPort : public QGraphicsView {
    Q_OBJECT
public:
    InstViewPort(const QString& model, const QString& path, QWidget* parent = nullptr);

signals:
    void topRowChanged(quint64 inst_id);



public slots:
    void onJumpToInst(quint64 instId);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void adjustSceneToView();

private:
    InstScene* scene;
    DataLoader* loader;
};

#endif
