#ifndef INSTSCENE_H_
#define INSTSCENE_H_
#include <QGraphicsScene>
#include "../DataLoader/dataloader.h"
#include "instrow.h"
#include "instdelayheader.h"

class InstScene : public QGraphicsScene {
    Q_OBJECT
public:
    InstScene(DataLoader* loader, QObject* parent = nullptr);
    ~InstScene();

protected:
    void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent);

private:
    void updateInst(int capacity);
    void checkIncrease(int capcity);
    void updateWrapper(int index);
    void updateRows();
    void updateDelayTime();

private:
    DataLoader* loader;
    int instCapacity = 0;
    int instCount = 0;
    int instHeightMargin = 60;
    int instDelayWidth = 40;
    int columnCount = 0;
    Inst* insts;
    InstWrapper* instWrappers;
    quint32 instMaxNum = 128;
    quint32 maxNumMask = 0x7F;
    QVector<InstRow*> rows;
    InstDelayHeader* delayHeader;
    quint64 current_id = 0;
    int currentBeginIdx = 0;
    int delayNum = 0;
    quint16* delayShiftVec = nullptr;
    quint64* instDelays = nullptr; 
    quint64 delayLabelValues[1024];
};

#endif
