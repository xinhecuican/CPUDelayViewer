#ifndef INSDIALOG_H_
#define INSDIALOG_H_
#include <QDialog>
#include <QVBoxLayout>

class InstDialog : public QDialog {
    Q_OBJECT
public:
    InstDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

public:
signals:
    void dataValid(const QString& model, const QString& fileName);

private:
    QString model;
    QString fileName;
    QVBoxLayout* layout;
};

#endif
