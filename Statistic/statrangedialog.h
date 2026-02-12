#ifndef STATRANGEDIALOG_H
#define STATRANGEDIALOG_H

#include <QDialog>
#include "../DataLoader/dataloader.h"
#include <QLineEdit>



class StatRangeDialog : public QDialog
{
    Q_OBJECT
public:
    StatRangeDialog(QWidget* parent = nullptr);
    void showDialog(DataLoader* loader);
    
signals:
    void dataValid(DataLoader* loader, quint64 start, quint64 end);

private:
    DataLoader* loader;
    QLineEdit* startLineEdit;
    QLineEdit* endLineEdit;
};

#endif // STATRANGEDIALOG_H