#include "cdsdataloader.h"
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <zstd.h>
#include <QFile>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CDSDataLoader::CDSDataLoader(QObject* parent) : DataLoader(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE", "cds_db");
    // db.setDatabaseName("cds.db");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return;
    }
}

void CDSDataLoader::load(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "data file not exist" << path;
        return;
    }
    quint32 meta_size;
    file.read((char*)&meta_size, 4);
    QByteArray json_data;
    json_data.resize(meta_size);
    file.read(json_data.data(), meta_size);
    QJsonDocument doc = QJsonDocument::fromJson(json_data);
    QJsonObject root = doc.object();
    if (root.contains("type_info")) {
        QJsonArray types = root["type_info"].toArray();
        for (auto it = types.begin(); it != types.end(); it++) {
            QJsonObject cur_type = it->toObject();
            MetaInfo info;
            info.name = cur_type.value("name").toString();
            info.size = cur_type.value("size").toInt();
            info.description = cur_type.value("description").toString();
            metas.append(info);
        }
    }
    if (root.contains("type_name")) {
        QJsonArray types = root["type_name"].toArray();
        for (auto it = types.begin(); it != types.end(); it++) {
            type_names.append(it->toString());
        }
    }
    primary_key = root.value("primary_key").toString();

    QByteArray compress_insts = file.readAll();
    QByteArray insts;
    quint64 decompress_size = ZSTD_getFrameContentSize(compress_insts.data(), compress_insts.size());
    if (decompress_size == ZSTD_CONTENTSIZE_ERROR) {
        qWarning() << "decompress content size error";
    } else if (decompress_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        qWarning() << "decompress content size unknown";
    }
    insts.resize(decompress_size);
    quint64 actual_decompress_size = ZSTD_decompress(insts.data(), insts.size(), compress_insts.data(), compress_insts.size());
    qDebug() << "insts decompressed" << actual_decompress_size;

    QSqlQuery query(db);
    QString queryStr = "CREATE TABLE IF NOT EXISTS insts (";
    QString meta_names;
    bool primary_key_valid = false;
    for (int i = 0; i < metas.size(); i++) {
        queryStr += metas[i].name + " ";
        meta_names += metas[i].name;
        switch (metas[i].size) {
        case 1: queryStr = queryStr + "TINYINT "; break;
        case 2: queryStr = queryStr + "SMALLINT "; break;
        case 8: queryStr = queryStr + "BIGINT "; break;
        default: queryStr = queryStr + "INTEGER "; break;
        }
        if (metas[i].name == primary_key) {
            queryStr = queryStr + "PRIMARY KEY";
            primary_key_valid = true;
            primary_key_idx = i;
        }
        if (i != metas.size() - 1) {
            queryStr += ",";
            meta_names += ",";
        }
    }
    if (!primary_key_valid) {
        queryStr += ",primary_key INTEGER PRIMARY KEY";
        primary_key = "primary_key";
        primary_key_idx = metas.size();
        meta_names += ",primary_key";
    }
    queryStr += ")";
    if (!query.exec(queryStr)) {
        qCritical() << "create table failed" << query.lastError().text();
    }

    constexpr int BATCH_SIZE = 10000;
    quint64 current_size = 0;
    quint64 inst_size = insts.size();

    // 计算每条记录的大小
    int record_size = 0;
    for (auto& meta : metas) {
        record_size += meta.size;
    }

    // 准备插入语句
    QString insertQuery = "INSERT INTO insts(" + meta_names + ") VALUES (";
    for (int i = 0; i < metas.size(); i++) {
        insertQuery += "?";
        if (i < metas.size() - 1) {
            insertQuery += ",";
        }
    }

    if (!primary_key_valid) {
        insertQuery += ",?";
    }
    insertQuery += ")";
    while (current_size < inst_size) {
        quint64 remain_size = inst_size - current_size;
        int batch_count = remain_size / record_size;
        if (batch_count > BATCH_SIZE) {
            batch_count = BATCH_SIZE;
        }

        db.transaction();
        QSqlQuery batchQuery(db);
        if (!batchQuery.prepare(insertQuery)) {
            qDebug() << "batch prepare fail";
        }

        // 绑定批量数据
        quint64 offset = current_size;
        for (int j = 0; j < batch_count; j++) {

            for (auto& meta : metas) {
                switch (meta.size) {
                case 1:
                    batchQuery.addBindValue(*(quint8*)(insts.data() + offset));
                    break;
                case 2:
                    batchQuery.addBindValue(*(quint16*)(insts.data() + offset));
                    break;
                case 4:
                    batchQuery.addBindValue(*(quint32*)(insts.data() + offset));
                    break;
                case 8:
                    batchQuery.addBindValue(*(quint64*)(insts.data() + offset));
                    break;
                default:
                    break;
                }
                offset += meta.size;
            }
            if (!primary_key_valid) {
                batchQuery.addBindValue(id);
                id++;
            }
            if (!batchQuery.exec()) {
                qCritical() << "insert failed:" << batchQuery.lastError().text();
            }
        }

        db.commit();
        current_size += batch_count * record_size;
        inst_num += batch_count;
    }

    qDebug() << "inst count" << inst_num;
}

bool CDSDataLoader::getInst(quint64 id, Inst* inst) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM insts WHERE " + primary_key + " = ?");
    query.addBindValue(id);
    if (!query.exec()) {
        return false;
    }
    if (!query.next()) {
        return false;
    }
    QSqlRecord record = query.record();
    inst->id = record.value(primary_key).toULongLong();
    inst->tick = record.value(0).toULongLong();
    inst->pc = record.value(1).toULongLong();
    inst->paddr = record.value(2).toULongLong();
    inst->type = record.value(3).toUInt();
    
    int delayCount = metas.size() - 4;
    for (int i = 4; i < metas.size(); i++) {
        inst->delay[i - 4] = record.value(i).toUInt();
    }
    return true;
}

int CDSDataLoader::getMultiInst(quint64 id, int num, Inst* inst) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM insts WHERE " + primary_key + " >= ? LIMIT ?");
    query.addBindValue(id);
    query.addBindValue(num);
    if (!query.exec()) {
        return 0;
    }
    int current = 0;
    int delayCount = metas.size() - 4;
    while (current < num) {
        if (!query.next()) {
            return current;
        }
        QSqlRecord record = query.record();
        Inst& currentInst = inst[current];
        currentInst.id = record.value(primary_key).toULongLong();
        currentInst.tick = record.value(0).toULongLong();
        currentInst.pc = record.value(1).toULongLong();
        currentInst.paddr = record.value(2).toULongLong();
        currentInst.type = record.value(3).toUInt();
        
        for (int i = 4; i < metas.size(); i++) {
            currentInst.delay[i - 4] = record.value(i).toUInt();
        }
        current++;
    }
    return current;
}

quint64 CDSDataLoader::getInstCount() {
    return inst_num;
}

QVector<QString> CDSDataLoader::getDelayNames() {
    QVector<QString> result;
    result.append("ST");
    for (int i = 4; i< metas.size(); i++) {
        result.append(metas[i].name);
    }
    return result;
}

void CDSDataLoader::getInstTicks(Inst* inst, quint64* ticks) {
    ticks[0] = inst->tick;
    for (int i = 0; i < inst->delay_num - 1; i++) {
        ticks[i + 1] = inst->delay[i] + ticks[0];
    }
}

QString CDSDataLoader::getTypeName(quint8 type) {
    if (type < type_names.size()) {
        return type_names[type];
    }
    return QString();
}

int CDSDataLoader::getTypeNum() {
    return type_names.size();
}