#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QMap>
#include "InstViewer/instdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void openFile();

private:
    QMenuBar* _menuBar;
    InstDialog* instDialog;
    QTabWidget* tabs;
    QVector<QWidget*> tabWidgets;
    int current_idx = 1;
};
#endif // MAINWINDOW_H
