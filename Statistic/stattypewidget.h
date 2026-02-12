#ifndef STATWIDGET_H_
#define STATWIDGET_H_

#include <QTableWidget>
#include "../DataLoader/dataloader.h"



class StatTypeWidget : public QTableWidget {
    Q_OBJECT
public:
    StatTypeWidget(DataLoader* loader, const QString& query, QWidget* parent = nullptr);

private:
    DataLoader* loader;
};

#endif // STATWIDGET_H_