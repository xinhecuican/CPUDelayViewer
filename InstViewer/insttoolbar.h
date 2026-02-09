#ifndef INSTTOOLBAR_H_
#define INSTTOOLBAR_H_
#include <QToolBar>
#include <QLineEdit>
#include "../DataLoader/dataloader.h"

class InstScene;

class InstToolBar : public QToolBar {
    Q_OBJECT
public:
    InstToolBar(DataLoader* loader, InstScene* scene, QWidget* parent = nullptr);

public slots:
    void onTopRowChanged(quint64 inst_id);

signals:
    void jumpToInst(quint64 instId);

private:
    QLineEdit* jumpEdit;
    InstScene* scene;
    DataLoader* loader;
};

#endif