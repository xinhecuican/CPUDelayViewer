#include "instdelayheader.h"
#include <QtGlobal>
#include <QGraphicsLineItem>

InstDelayHeader::InstDelayHeader(int labelWidth, int typeWidth, int delayWidth, int height, QGraphicsItem* parent) : QGraphicsItemGroup(parent) {
    labelNum = 0;
    this->delayWidth = delayWidth;
    this->labelWidth = labelWidth;
    this->typeWidth = typeWidth;
    this->height = height;
    tickLabel = new InstLabelItem(this);
    tickLabel->setBorderWidth(0);
    tickLabel->setPos(0, 0);
    tickLabel->setWidthHeight(labelWidth, height);
}

void InstDelayHeader::onLabelNumChange(int labelNum) {
    if (labelNum <= 0) return;
    int lastNum = this->labelNum;
    this->labelNum = labelNum;
    if (lastNum < labelNum) {
        for (int i = lastNum; i < labelNum; i++) {
            InstLabelItem* label = new InstLabelItem(this);
            int pos = labelWidth + typeWidth + i * delayWidth;
            label->setPos(pos, 0);
            label->setBorderWidth(0);
            label->setWidthHeight(delayWidth, height);
            labels.append(label);
        }
    }
}

void InstDelayHeader::setDelayLabels(quint64* delays) {
    quint64 startDelay = delays[0];
    tickLabel->setPlainText(QString::number(startDelay, 16));
    for (int i = 0; i < labelNum; i++) {
        if (delays[i] == UINT64_MAX) {
            labels[i]->setPlainText("");
            labels[i]->setFillColor(QColor(0xd9d9d9));
        } else {
            labels[i]->setPlainText(QString::number(delays[i] - startDelay));
            labels[i]->setFillColor(Qt::white);
        }
    }
    update();
}

void InstDelayHeader::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QGraphicsItemGroup::paint(painter, option, widget);
}

void InstDelayHeader::delayLabelEnter(int shift) {
    if (shift < labelNum) {
        labels[shift]->setFillColor(QColor(0x40a9ff));
        update();
    }
}

void InstDelayHeader::delayLabelLeave(int shift) {
    if (shift < labelNum) {
        labels[shift]->setFillColor(Qt::white);
        update();
    }
}


