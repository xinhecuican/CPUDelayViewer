#ifndef INSTTOOLBAR_H_
#define INSTTOOLBAR_H_
#include <QToolBar>
#include <QLineEdit>

class InstToolBar : public QToolBar {
    Q_OBJECT
public:
    InstToolBar(QWidget* parent = nullptr);

public slots:
    void onTopRowChanged(quint64 inst_id);

signals:
    void jumpToInst(quint64 instId);

private:
    QLineEdit* jumpEdit;
};

#endif