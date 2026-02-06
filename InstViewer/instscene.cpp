#include "instscene.h"
#include <QGraphicsSceneWheelEvent>
#include <QDebug>
#include <QFont>
#include "../config.h"
#include <QFontMetrics>
#include <QtGlobal>

InstScene::InstScene(DataLoader* loader, QObject* parent) : QGraphicsScene(parent) {
    this->loader = loader;
    delayNum = loader->getDelayNames().size();
    connect(this, &QGraphicsScene::sceneRectChanged, this, [=, this](const QRectF& rect) {
        int capacity = (rect.height() / instHeightMargin - 1);
        columnCount = std::ceil((double)(rect.width() - instLabelWidth - instTypeWidth) / (double)(instDelayWidth));
        delayHeader->onLabelNumChange(columnCount);
        updateInst(capacity);
    });
    setBackgroundBrush(Qt::white);
    insts = new Inst[instMaxNum];
    instWrappers = new InstWrapper[instMaxNum];
    instDelays = new quint64[instMaxNum * delayNum];
    for (int i = 0; i < instMaxNum; i++) {
        insts[i].setDelay(delayNum);
        instWrappers[i].inst = &insts[i];
    }
    QFont defaultFont(Config::getStr("InstScnene/Font", "Courier New"), Config::getInt("InstScnene/FontSize", 12));
    setFont(defaultFont);
    QFontMetrics metrics(defaultFont);
    instHeightMargin = metrics.capHeight() + 20;
    instDelayWidth = 0;
    for (int i = 0; i < delayNum; i++) {
        instDelayWidth = qMax(instDelayWidth, metrics.horizontalAdvance(loader->getDelayNames()[i]));
    }
    instDelayWidth += 10;
    instLabelWidth = metrics.horizontalAdvance("0000000000000000");
    int typeNum = loader->getTypeNum();
    instTypeWidth = metrics.horizontalAdvance(loader->getTypeName(0));
    for (int i = 1; i < typeNum; i++) {
        instTypeWidth = qMax(instTypeWidth, metrics.horizontalAdvance(loader->getTypeName(i)));
    }

    delayHeader = new InstDelayHeader(instLabelWidth, instTypeWidth, instDelayWidth, instHeightMargin);
    addItem(delayHeader);
    qDebug() << "instLabelWidth" << instLabelWidth << "instTypeWidth" << instTypeWidth << "instDelayWidth" << instDelayWidth;
}

InstScene::~InstScene() {
    delete[] insts;
    delete[] instWrappers;
    delete[] instDelays;
    if (delayShiftVec != nullptr) {
        delete [] delayShiftVec;
    }
}



void InstScene::wheelEvent(QGraphicsSceneWheelEvent* wheelEvent) {
    if (wheelEvent->delta() < 0) {
        if (current_id + instCount < loader->getInstCount() - 1) {
            currentBeginIdx = (currentBeginIdx + 1) & maxNumMask;
            current_id++;
            emit topRowChanged(current_id);
            int updateIdx = (currentBeginIdx + instCount) & maxNumMask;
            loader->getInst(current_id + instCount, &insts[updateIdx]);
            updateWrapper(updateIdx);
        }
    } else if (current_id != 0){
        if (currentBeginIdx != 0) {
            currentBeginIdx = currentBeginIdx - 1;
        } else {
            currentBeginIdx = maxNumMask;
        }
        current_id--;
        emit topRowChanged(current_id);
        loader->getInst(current_id, &insts[currentBeginIdx]);
        updateWrapper(currentBeginIdx);
    }
    updateRows();
}

void InstScene::updateInst(int capacity) {
    checkIncrease(capacity);
    bool needUpdate = instCount != capacity;
    if (instCount < capacity) {
        quint32 beginIdx = currentBeginIdx + instCount;
        quint32 endIdx = (beginIdx + capacity);
        if (endIdx > instMaxNum) {
            quint32 copyNum = instMaxNum - beginIdx;
            quint32 remainNum = instCount - capacity - copyNum;
            quint32 beginIdxMask = beginIdx & maxNumMask;
            quint32 remainIdx = beginIdxMask;
            quint32 endIdxMask = endIdx & maxNumMask;
            if (beginIdx < instMaxNum) {
                loader->getMultiInst(current_id + instCount, copyNum, &insts[beginIdxMask]);
                remainIdx = 0;
                for (int i = beginIdxMask; i < instMaxNum; i++) {
                    updateWrapper(i);
                }
            }
            loader->getMultiInst(current_id + instCount + copyNum, remainNum, &insts[remainIdx]);
            for (int i = remainIdx; i < endIdxMask; i++) {
                updateWrapper(i);
            }
        } else {
            loader->getMultiInst(current_id + instCount, capacity - instCount, &insts[beginIdx]);
            for (int i = beginIdx; i < endIdx; i++) {
                updateWrapper(i);
            }
        }
        for (int i = instCount; i < capacity; i++) {
            InstRow* row = new InstRow(instLabelWidth, instTypeWidth, instDelayWidth, instHeightMargin, loader);
            row->connect(row, &InstRow::hoverDelayLabelEnter, delayHeader, &InstDelayHeader::delayLabelEnter);
            row->connect(row, &InstRow::hoverDelayLabelLeave, delayHeader, &InstDelayHeader::delayLabelLeave);
            addItem(row);
            row->setPos(0, (i + 1) * instHeightMargin);
            row->setDelayStr(loader->getDelayNames());
            rows.append(row);
        }
        instCount = capacity;
    } else if (instCount > capacity) {
        auto it = rows.begin() + capacity;
        while (it != rows.end()) {
            removeItem(*it);
            delete *it;
            it = rows.erase(it);
        }
        instCount = capacity;
    }
    if (needUpdate) {
        if (delayShiftVec != nullptr) {
            delete [] delayShiftVec;
        }
        delayShiftVec = new quint16[instCount * delayNum];
        updateRows();
    }
}

void InstScene::checkIncrease(int capacity) {
    if (capacity > instMaxNum) {
        quint32 oldNum = instMaxNum;
        instMaxNum = 1 << (int)std::ceil(std::log2(capacity));
        maxNumMask = instMaxNum - 1;
        Inst* old = insts;
        insts = new Inst[instMaxNum];
        for (int i = 0; i < instMaxNum; i++) {
            insts[i].setDelay(delayNum);
        }
        InstWrapper* oldWrapper = instWrappers;
        instWrappers = new InstWrapper[instMaxNum];
        quint64* oldDelays = instDelays;
        instDelays = new quint64[instMaxNum * delayNum];
        quint32 endIdx = (currentBeginIdx + instCount);
        if (endIdx > oldNum) {
            quint32 copyNum = oldNum - currentBeginIdx;
            quint32 remainNum = instCount - copyNum;
            memcpy((void*)(&insts[instMaxNum-copyNum]), &old[currentBeginIdx], copyNum);
            memcpy((void*)insts, old, remainNum);
            for (int i = instMaxNum - copyNum; i < instMaxNum; i++) {
                instWrappers[i].inst = &insts[i];
                instWrappers[i].copyStr(oldWrapper[i-(instMaxNum - oldNum)]);
            }
            for (int i = 0; i < remainNum; i++) {
                instWrappers[i].inst = &insts[i];
                instWrappers[i].copyStr(oldWrapper[i]);
            }
            currentBeginIdx = instMaxNum - copyNum;
        } else {
            memcpy((void*)&insts[currentBeginIdx], &old[currentBeginIdx], instCount);
            for (int i = currentBeginIdx; i < currentBeginIdx + instCount; i++) {
                instWrappers[i].inst = &insts[i];
                instWrappers[i].copyStr(oldWrapper[i]);
            }
        }
        for (int i = 0; i < instCount; i++) {
            int idx = (i + currentBeginIdx) & maxNumMask;
            loader->getInstTicks(&insts[idx], &instDelays[idx * delayNum]);
        }
        delete [] old;
        delete [] oldWrapper;
        delete [] oldDelays;
    }
}

void InstScene::updateWrapper(int index) {
    instWrappers[index].tick_str = QString::number(instWrappers[index].inst->tick, 16);
    instWrappers[index].paddr_str = QString::number(instWrappers[index].inst->paddr, 16);
    loader->getInstTicks(instWrappers[index].inst, &instDelays[index * delayNum]);
}

void InstScene::updateRows() {
    updateDelayTime();
    delayHeader->setDelayLabels(delayLabelValues);
    int showCount = current_id + instCount > loader->getInstCount() ? loader->getInstCount() - current_id : instCount;
    int shift = 0;
    for (int i = 0; i < instCount; i++) {
        int currentIdx = (currentBeginIdx + i) & maxNumMask;
        rows[i]->setInst(&delayShiftVec[i * delayNum], &instWrappers[currentIdx]);
    }
    for (int i = 0; i < showCount; i++) {
        rows[i]->show();
    }
    for (int i = showCount; i < instCount; i++) {
        rows[i]->hide();
    }
    update();
}

void InstScene::updateDelayTime() {
    // 0 1 10
    // 0 2 3
    // 1 3 5
    int currentShift = 0;
    quint64 lastTick = UINT64_MAX;
    int startIdx = 0;
    bool instFinish[1024];
    uint8_t instCurrentShift[1024];
    memset(instFinish, false, sizeof(instFinish));
    memset(instCurrentShift, 0, sizeof(instCurrentShift));
    memset(delayShiftVec, 0, sizeof(uint8_t) * instCount * delayNum);

    while (1) {
        quint64 minTick = UINT64_MAX;
        quint16 validInstIdxs[1024];
        int validInstSize = 0;
        for (int i = startIdx; i < instCount; i++) {
            if (instFinish[i]) {
                continue;
            }
            int idx = (i + currentBeginIdx) & maxNumMask;
            if (instDelays[idx * delayNum + instCurrentShift[i]] < minTick) {
                minTick = instDelays[idx * delayNum + instCurrentShift[i]];
                validInstIdxs[0] = i;
                validInstSize = 1;
            } else if (instDelays[idx * delayNum + instCurrentShift[i]] == minTick) {
                validInstIdxs[validInstSize++] = i;
            }
        }
        quint64 tick_delta = minTick - lastTick;
        if (minTick > lastTick && tick_delta > 5) {
            delayLabelValues[currentShift] = UINT64_MAX;
            currentShift++;
        }
        delayLabelValues[currentShift] = minTick;
        lastTick = minTick;
        for (int i = 0; i < validInstSize; i++) {
            if (instFinish[validInstIdxs[i]]) {
                continue;
            }
            delayShiftVec[validInstIdxs[i] * delayNum + instCurrentShift[validInstIdxs[i]]] = currentShift;
            instCurrentShift[validInstIdxs[i]]++;
            if (instCurrentShift[validInstIdxs[i]] == delayNum) {
                instFinish[validInstIdxs[i]] = true;
            }
        }
        currentShift++;

        while (instFinish[startIdx]) {
            startIdx++;
            if (startIdx >= instCount) {
                return;
            }
        }

        if (currentShift >= columnCount) {
            // 将超出视图范围的标签移出视图
            for (int i = 0; i < instCount; i++) {
                if (!instFinish[i]) {
                    while(instCurrentShift[i] < delayNum) {
                        delayShiftVec[i * delayNum + instCurrentShift[i]] = UINT16_MAX;
                        instCurrentShift[i]++;
                    }
                }
            }
            return;
        }
    }
}

void InstScene::jumpToInst(quint64 id) {
    if (id >= loader->getInstCount()) {
        return;
    }
    current_id = id;
    emit topRowChanged(current_id);
    quint32 beginIdx = currentBeginIdx;
    quint32 endIdx = currentBeginIdx + instCount;
    if (endIdx > instMaxNum) {
        quint32 copynum = instMaxNum - currentBeginIdx;
        quint32 remainNum = instCount - copynum;
        loader->getMultiInst(current_id, copynum, &insts[currentBeginIdx]);
        loader->getMultiInst(current_id + copynum, remainNum, &insts[0]);
        for (int i = beginIdx; i < instMaxNum; i++) {
            updateWrapper(i);
        }
        for (int i = 0; i < remainNum; i++) {
            updateWrapper(i);
        }
    } else {
        loader->getMultiInst(current_id, instCount, &insts[currentBeginIdx]);
        for (int i = beginIdx; i < endIdx; i++) {
            updateWrapper(i);
        }
    }
    updateRows();
}
