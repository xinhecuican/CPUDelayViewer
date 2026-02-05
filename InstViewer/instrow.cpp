#include "instrow.h"

InstRow::InstRow(int labelWidth, int delayWidth, int height, QGraphicsItem* parent) : QGraphicsItemGroup(parent) {
    this->delayWidth = delayWidth;
    this->label_width = labelWidth;
    this->height = height;
    // shadow_effect = new QGraphicsDropShadowEffect();
    // shadow_effect->setBlurRadius(5);
    // shadow_effect->setOffset(0, 0);
    // shadow_effect->setColor(Qt::black);
    // setGraphicsEffect(shadow_effect);
}

quint32 defaultColors[12] = {
    0xff7a45, 0xffa940, 0xbae637, 0xffc53d, 0xffec3d, 0xf759ab,
    0x73d13d, 0x36cfc9, 0x40a9ff, 0x4096ff, 0x597ef7, 0x9254de
};

void InstRow::initInst(InstWrapper* inst) {
    init = true;
    inst_label = new InstLabelItem(this);
    inst_label->setPos(0, 0);
    inst_label->setWidthHeight(label_width, height);
    inst_label->setGroup(this);

    for (int i = 0; i < inst->inst->delay_num; i++) {
        InstLabelItem* delay_item = new InstLabelItem(this);
        delay_item->setWidthHeight(delayWidth, height);
        delay_item->setFillColor(QColor(defaultColors[i % 12]));
        delay_item->setGroup(this);
        delay_labels.append(delay_item);
    }
}

void InstRow::setInst(quint16* shift, InstWrapper* inst) {
    if (!init) {
        initInst(inst);
    }
    for (int i = 0; i < inst->inst->delay_num; i++) {
        delay_labels[i]->setPos(label_width + delayWidth * (shift[i]), 0);
    }
    inst_label->setPlainText(inst->paddr_str);
    for (int i = 0; i < inst->inst->delay_num; i++) {
        delay_labels[i]->setPlainText(delay_str[i]);
    }
}

void InstRow::setDelayStr(QVector<QString> delay_str) {
    this->delay_str = delay_str;
}