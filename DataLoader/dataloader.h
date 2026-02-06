#ifndef DATALOADER_H
#define DATALOADER_H
#include "../InstViewer/instdefines.h"
#include <QString>
#include <QVector>
#include <QObject>
#include <QMap>
#include <QMetaObject>

// 注册宏
#define REGISTER_LOADER(ClassName) \
    class ClassName##Register { \
    public: \
        ClassName##Register() { \
            DataLoader::registerLoader(#ClassName, ClassName::staticMetaObject); \
        } \
    }; \
    static ClassName##Register ClassName##RegisterInstance;

class DataLoader : public QObject {
    Q_OBJECT
public:
    DataLoader(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~DataLoader() = default;
    virtual bool getInst(quint64 id, Inst* inst) = 0;
    virtual int getMultiInst(quint64 id, int num, Inst* inst) = 0;
    virtual void load(const QString& path){}
    virtual inline quint64 getInstCount() {return 0;}
    virtual QVector<QString> getDelayNames() { return QVector<QString>(); }
    static void registerLoader(const QString& name, const QMetaObject& meta) {
        loader_meta[name] = meta;
    }
    virtual void getInstTicks(Inst* inst, quint64* ticks) {}
    virtual QString getTypeName(quint8 type) { return QString(); }
    virtual int getTypeNum() { return 0; }
    static QMap<QString, QMetaObject> loader_meta;
};

#endif // DATALOADER_H
