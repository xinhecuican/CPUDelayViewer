#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "InstViewer/instpanel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1920, 1080);
    tabs = new QTabWidget(this);
    tabs->setTabsClosable(true);
    setCentralWidget(tabs);
    _menuBar = menuBar();
    QMenu* fileMenu = new QMenu(tr("文件"), _menuBar);
    _menuBar->addMenu(fileMenu);
    QAction* openFileAction = fileMenu->addAction(tr("打开指令"));
    openFileAction->connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);

    instDialog = new InstDialog(this);
    instDialog->hide();

    connect(tabs, &QTabWidget::tabCloseRequested, this, [=, this](int index){
        if (tabWidgets[index] == nullptr) {
            return;
        }
        tabWidgets[index]->deleteLater();
        tabWidgets[index] = nullptr;
    });
    connect(instDialog, &InstDialog::dataValid, this, [=, this](const QString& model, const QString& fileName){
        InstPanel* panel = new InstPanel(model, fileName, this);
        int index = tabs->addTab(panel, model + QString::number(current_idx++));
        panel->show();
        tabs->setCurrentIndex(index);
        if (tabWidgets.size() > index) {
            tabWidgets[index] = panel;
        } else {
            tabWidgets.append(panel);
        }
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile()
{
    instDialog->open();
}
