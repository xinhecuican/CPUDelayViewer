#include "instrow.h"
#include <QGraphicsSceneHoverEvent>

InstRow::InstRow(int labelWidth, int typeWidth, int delayWidth, int height, DataLoader* loader, QGraphicsItem* parent) : QGraphicsItemGroup(parent) {
    this->loader = loader;
    this->delayWidth = delayWidth;
    this->label_width = labelWidth;
    this->typeWidth = typeWidth;
    this->height = height;
    setAcceptHoverEvents(true);
    // shadow_effect = new QGraphicsDropShadowEffect();
    // shadow_effect->setBlurRadius(5);
    // shadow_effect->setOffset(0, 0);
    // shadow_effect->setColor(Qt::black);
    // setGraphicsEffect(shadow_effect);
}

quint32 defaultColors[12] = {
    0xff7a45, 0xffa940, 0xbae637, 0x4096ff, 0xffec3d, 0xf759ab,
    0x73d13d, 0x36cfc9, 0x40a9ff, 0x9254de, 0x597ef7, 0xffc53d
};

void InstRow::initInst(InstWrapper* inst) {
    prepareGeometryChange();
    init = true;
    inst_label = new InstLabelItem(this);
    inst_label->setPos(0, 0);
    inst_label->setWidthHeight(label_width, height);
    addToGroup(inst_label);
    type_label = new InstLabelItem(this);
    type_label->setPos(label_width, 0);
    type_label->setWidthHeight(typeWidth, height);
    addToGroup(type_label);

    for (int i = 0; i < inst->inst->delay_num; i++) {
        InstLabelItem* delay_item = new InstLabelItem(this);
        delay_item->setData(0, i);
        delay_item->setAcceptHoverEvents(true);
        addToGroup(delay_item);
        delay_item->installSceneEventFilter(this);
        delay_item->setWidthHeight(delayWidth, height);
        delay_item->setFillColor(QColor(defaultColors[i % 12]));
        delay_labels.append(delay_item);
    }
    shifts.resize(inst->inst->delay_num);
}

void InstRow::setInst(quint16* shift, InstWrapper* inst) {
    if (!init) {
        initInst(inst);
    }
    for (int i = 0; i < inst->inst->delay_num; i++) {
        delay_labels[i]->setPos(label_width + typeWidth + delayWidth * (shift[i]), 0);
        shifts[i] = shift[i];
    }
    inst_label->setPlainText(inst->paddr_str);
    type_label->setPlainText(loader->getTypeName(inst->inst->type));
    for (int i = 0; i < inst->inst->delay_num; i++) {
        delay_labels[i]->setPlainText(delay_str[i]);
    }
    bound = inst_label->boundingRect();
    for (auto label : delay_labels) {
        QRectF label_bound = label->boundingRect();
        label_bound.moveTo(label->pos());
        bound = bound.united(label_bound);
    }
}

void InstRow::setDelayStr(QVector<QString> delay_str) {
    this->delay_str = delay_str;
}

void InstRow::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    if (init) {
        inst_label->setFillColor(QColor(0x40a9ff));
    }
    QGraphicsItemGroup::hoverEnterEvent(event);
}

void InstRow::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    if (init) {
        inst_label->setFillColor(Qt::white);
    }
    QGraphicsItemGroup::hoverLeaveEvent(event);
}

bool InstRow::sceneEventFilter(QGraphicsItem* watched, QEvent* event) {
    int id = watched->data(0).toInt();
    if (event->type() == QEvent::GraphicsSceneHoverEnter) {
        emit hoverDelayLabelEnter(shifts[id]);
    } else if (event->type() == QEvent::GraphicsSceneHoverLeave) {
        emit hoverDelayLabelLeave(shifts[id]);
    }
    return false;
}