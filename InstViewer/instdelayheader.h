#ifndef INSTDDELAYHEADER_H
#define INSTDDELAYHEADER_H

#include <QGraphicsItemGroup>
#include <QVector>
#include "instlabelitem.h"

class InstDelayHeader : public QObject, public QGraphicsItemGroup {
    Q_OBJECT
public:
    InstDelayHeader(int labelWidth, int typeWidth, int delayWidth, int height, QGraphicsItem* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);
    void setDelayLabels(quint64* delays);
    void onLabelNumChange(int labelNum);

public slots:
    void delayLabelEnter(int shift);
    void delayLabelLeave(int shift);

private:
    int labelNum;
    int delayWidth;
    int labelWidth;
    int typeWidth;
    int height;
    InstLabelItem* tickLabel;
    QVector<InstLabelItem*> labels;
};

#endif