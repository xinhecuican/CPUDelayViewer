#ifndef CDSDATALOADER_H
#define CDSDATALOADER_H
#include "dataloader.h"
#include <QtSql/QSqlDatabase>

class CDSDataLoader : public DataLoader
{
    Q_OBJECT
public:
    Q_INVOKABLE CDSDataLoader(QObject* parent = nullptr);
    void load(const QString& path) override;
    bool getInst(quint64 id, Inst* inst) override;
    int getMultiInst(quint64 id, int num, Inst* inst) override;
    quint64 getInstCount() override;
    QVector<QString> getDelayNames() override;
    void getInstTicks(Inst* inst, quint64* ticks) override;

private:
    struct MetaInfo {
        QString name;
        int size;
        QString description;
    };
    QSqlDatabase db;
    QVector<MetaInfo> metas;
    QString primary_key;
    quint64 inst_num = 0;
    int primary_key_idx;
    quint64 id = 0;
};

REGISTER_LOADER(CDSDataLoader)

#endif // CDSDATALOADER_H
