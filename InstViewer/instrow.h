#ifndef INSTROW_H_
#define INSTROW_H_
#include <QWidget>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include "instdefines.h"
#include "instlabelitem.h"

class InstRow : public QGraphicsItemGroup {
public:
    InstRow(int labelWidth, int delayWidth, int height, QGraphicsItem* parent = nullptr);
    void setInst(quint16* shift, InstWrapper* inst);
    void setDelayStr(const QVector<QString> delay_str);

private:
    void initInst(InstWrapper* inst);

private:
    // QGraphicsDropShadowEffect* shadow_effect;
    bool init = false;
    InstLabelItem* inst_label;
    Inst* inst;
    QVector<InstLabelItem*> delay_labels;
    QVector<QString> delay_str;
    int delayWidth;
    int label_width;
    int height;
};
#endif
