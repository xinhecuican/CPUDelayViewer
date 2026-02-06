#include "instpanel.h"
#include <QVBoxLayout>

InstPanel::InstPanel(const QString& model, const QString& path, QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    toolBar = new InstToolBar(this);
    toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    viewport = new InstViewPort(model, path, this);
    viewport->setFrameShape(QFrame::NoFrame);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(toolBar, &InstToolBar::jumpToInst, viewport, &InstViewPort::onJumpToInst);
    connect(viewport, &InstViewPort::topRowChanged, toolBar, &InstToolBar::onTopRowChanged);

    layout->addWidget(toolBar);
    layout->addWidget(viewport);
}
