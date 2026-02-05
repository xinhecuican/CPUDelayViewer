#ifndef INSTDEFINES_H
#define INSTDEFINES_H
#include <QVector>

struct Inst {
    uint64_t id;
    uint64_t tick;
    uint64_t pc;
    uint64_t paddr;
    uint8_t type;
    uint8_t delay_num;
    uint16_t* delay;

    Inst() {
        delay_num = 0;
        delay = nullptr;
    }

    Inst(int delay_num) {
        this->delay_num = delay_num;
        delay = new uint16_t[delay_num];
        memset(delay, 0, delay_num * sizeof(uint16_t));
    }
    
    void setDelay(int delay_num) {
        this->delay_num = delay_num;
        delay = new uint16_t[delay_num];
        memset(delay, 0, delay_num * sizeof(uint16_t));
    }

    ~Inst() {
        if (delay != nullptr) {
            delete[] delay;
        }
    }
};

struct InstWrapper {
    Inst* inst;
    QString paddr_str;
    QString tick_str;

    void copyStr(const InstWrapper& other) {
        paddr_str = other.paddr_str;
        tick_str = other.tick_str;
    }
};

#endif // INSTDEFINES_H
