#ifndef INSTROW_H_
#define INSTROW_H_
#include <QWidget>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include "instdefines.h"
#include "instlabelitem.h"
#include "../DataLoader/dataloader.h"

class InstRow : public QObject, public QGraphicsItemGroup {
    Q_OBJECT
public:
    InstRow(int labelWidth, int typeWidth, int delayWidth, int height, DataLoader* loader, QGraphicsItem* parent = nullptr);
    void setInst(quint16* shift, InstWrapper* inst);
    void setDelayStr(const QVector<QString> delay_str);
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    bool sceneEventFilter(QGraphicsItem* watched, QEvent* event) override;
    // QRectF boundingRect() const override { return bound; }

signals:
    void hoverDelayLabelEnter(int shift);
    void hoverDelayLabelLeave(int shift);

private:
    void initInst(InstWrapper* inst);

private:
    // QGraphicsDropShadowEffect* shadow_effect;
    DataLoader* loader;
    bool init = false;
    InstLabelItem* inst_label;
    InstLabelItem* type_label;
    Inst* inst;
    QVector<InstLabelItem*> delay_labels;
    QVector<QString> delay_str;
    int delayWidth;
    int typeWidth;
    int label_width;
    int height;
    QRectF bound;
    QVector<quint16> shifts;
};
#endif
