#ifndef INSTPANEL_H_
#define INSTPANEL_H_
#include <QWidget>
#include "instviewport.h"
#include "insttoolbar.h"

class InstPanel : public QWidget {
    Q_OBJECT
public:
    InstPanel(const QString& model, const QString& path, QWidget* parent = nullptr);
    DataLoader* getLoader();

private:
    InstViewPort* viewport;
    InstToolBar* toolBar;
    DataLoader* loader;
};

#endif