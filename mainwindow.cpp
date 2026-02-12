#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "InstViewer/instpanel.h"
#include "Statistic/stattypewidget.h"
#include "Statistic/resulttypewidget.h"

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

    statRangeDialog = new StatRangeDialog(this);
    statRangeDialog->hide();
    connect(statRangeDialog, &StatRangeDialog::dataValid, this, [=, this](DataLoader* loader, quint64 start, quint64 end){
        StatTypeWidget* widget = new StatTypeWidget(loader, "SELECT type, COUNT(*) FROM insts WHERE " + loader->getPrimaryKey() + 
                " BETWEEN " + QString::number(start) + " AND " + QString::number(end) + " GROUP BY type", this);
        int index = tabs->addTab(widget, "指令类型统计");
        widget->show();
        tabs->setCurrentIndex(index);
        if (tabWidgets.size() > index) {
            tabWidgets[index] = widget;
        } else {
            tabWidgets.append(widget);
        }
    });
    resultRangeDialog = new StatRangeDialog(this);
    resultRangeDialog->hide();
    connect(resultRangeDialog, &StatRangeDialog::dataValid, this, [=, this](DataLoader* loader, quint64 start, quint64 end){
        ResultTypeWidget* widget = new ResultTypeWidget(loader, "SELECT result, COUNT(*) FROM insts WHERE " + loader->getPrimaryKey() + 
                " BETWEEN " + QString::number(start) + " AND " + QString::number(end) + " GROUP BY result", this);
        int index = tabs->addTab(widget, "结果类型统计");
        widget->show();
        tabs->setCurrentIndex(index);
        if (tabWidgets.size() > index) {
            tabWidgets[index] = widget;
        } else {
            tabWidgets.append(widget);
        }
    });



    QMenu* statisticMenu = new QMenu(tr("统计"), _menuBar);
    _menuBar->addMenu(statisticMenu);
    QAction* instTypeAction = statisticMenu->addAction(tr("指令类型统计"));
    instTypeAction->connect(instTypeAction, &QAction::triggered, this, [=, this](bool checked){
        openRangeDialog(statRangeDialog, checked);
    });
    QAction* resultTypeAction = statisticMenu->addAction(tr("结果类型统计"));
    resultTypeAction->connect(resultTypeAction, &QAction::triggered, this, [=, this](bool checked){
        openRangeDialog(resultRangeDialog, checked);
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile()
{
    instDialog->open();
}

void MainWindow::openRangeDialog(StatRangeDialog* dialog, bool checked)
{
    QWidget* currentWidget = tabs->currentWidget();
    if (currentWidget == nullptr) {
        return;
    }
    InstPanel* panel = dynamic_cast<InstPanel*>(currentWidget);
    if (panel == nullptr) {
        return;
    }
    dialog->showDialog(panel->getLoader());
}