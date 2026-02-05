#ifndef INSTDDELAYHEADER_H
#define INSTDDELAYHEADER_H

#include <QGraphicsItemGroup>
#include <QVector>
#include "instlabelitem.h"

class InstDelayHeader : public QGraphicsItemGroup {
public:
    InstDelayHeader(int labelWidth, int delayWidth, int height, QGraphicsItem* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);
    void setDelayLabels(quint64* delays);
    void onLabelNumChange(int labelNum);
private:
    int labelNum;
    int delayWidth;
    int labelWidth;
    int height;
    InstLabelItem* tickLabel;
    QVector<InstLabelItem*> labels;
};

#endif