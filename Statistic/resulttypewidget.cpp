#include "resulttypewidget.h"
#include <QSqlRecord>
#include <QTableWidgetItem>

ResultTypeWidget::ResultTypeWidget(DataLoader* loader, const QString& query, QWidget* parent) : QTableWidget(parent) {
    this->loader = loader;
    QSqlQuery* q = loader->query(query);
    if (q == nullptr) {
        return;
    }
    QSqlRecord record = q->record();
    setColumnCount(record.count());
    for (int i = 0; i < record.count(); i++) {
        setHorizontalHeaderItem(i, new QTableWidgetItem(record.fieldName(i)));
    }
    int row = 0;
    QVector<QVector<QString>> data;
    while (q->next()) {
        QVector<QString> rowData;
        for (int i = 0; i < q->record().count(); i++) {
            rowData.append(q->value(i).toString());
        }
        data.append(rowData);
        row++;
    }
    setRowCount(row);
    QVector<QString> resultTypes = loader->getResultNames();
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < record.count(); j++) {
            if (j == 0) {
                setVerticalHeaderItem(i, new QTableWidgetItem(resultTypes[data[i][j].toInt()]));
            }
            setItem(i, j, new QTableWidgetItem(data[i][j]));
        }
    }
    delete q;
}

