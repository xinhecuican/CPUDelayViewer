#include "instpanel.h"
#include <QVBoxLayout>
#include "instscene.h"

InstPanel::InstPanel(const QString& model, const QString& path, QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    loader = DataLoaderFactory::createLoader(model, path, this);
    viewport = new InstViewPort(loader, this);
    viewport->setFrameShape(QFrame::NoFrame);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolBar = new InstToolBar(loader, viewport->getScene(), this);
    toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    layout->addWidget(toolBar);
    layout->addWidget(viewport);
}
