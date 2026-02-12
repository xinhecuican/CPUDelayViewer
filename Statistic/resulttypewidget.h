#ifndef RESULTTYPEWIDGET_H_
#define RESULTTYPEWIDGET_H_

#include <QTableWidget>
#include "../DataLoader/dataloader.h"



class ResultTypeWidget : public QTableWidget {
    Q_OBJECT
public:
    ResultTypeWidget(DataLoader* loader, const QString& query, QWidget* parent = nullptr);

private:
    DataLoader* loader;
};

#endif // RESULTTYPEWIDGET_H_
