#include "graphtoolbar.h"
#include <QPushButton>

GraphToolbar::GraphToolbar(GraphScene* scene, QWidget* parent) : QToolBar(parent) {
    this->scene = scene;
    QPushButton* settingButton = new QPushButton(this);
    settingButton->setIcon(QIcon(":/img/setting.svg"));
    settingButton->setIconSize(QSize(24, 24));
    settingButton->setToolTip("Settings");
    addWidget(settingButton);
    connect(settingButton, &QPushButton::clicked, this, [=, this](){
        emit settingClicked();
    });
}
