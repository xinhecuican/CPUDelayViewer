#ifndef CDSDATALOADER_H
#define CDSDATALOADER_H
#include "dataloader.h"
#include <QtSql/QSqlDatabase>

class CDSDataLoader : public DataLoader
{
    Q_OBJECT
public:
    Q_INVOKABLE CDSDataLoader(int index, QObject* parent = nullptr);
    void load(const QString& path) override;
    bool getInst(quint64 id, Inst* inst) override;
    int getMultiInst(quint64 id, int num, Inst* inst) override;
    quint64 getInstCount() override;
    QVector<QString> getDelayNames() override;
    void getInstTicks(Inst* inst, quint64* ticks) override;
    QString getTypeName(quint8 type) override;
    int getTypeNum() override;
    QVector<QString> getResultNames() override;
    QVector<int> getResultLevels() override;
    QSqlQuery* query(const QString& sql) override;
    QString getPrimaryKey() override;
    QVector<QString> getInstTypes() override;
    QVector<QString> getFieldsNames() override;
    QSqlQuery* getQuery() override;

private:
    struct MetaInfo {
        QString name;
        int size;
        QString description;
    };
    QSqlDatabase db;
    QVector<MetaInfo> metas;
    QString primary_key;
    QVector<QString> type_names;
    QVector<QString> result_names;
    QVector<int> result_levels;
    quint64 inst_num = 0;
    QSqlQuery multiQuery;
    QSqlQuery instQuery;
    int primary_key_idx;
    QVector<QString> fields_names;
};

REGISTER_LOADER(CDSDataLoader)

#endif // CDSDATALOADER_H
