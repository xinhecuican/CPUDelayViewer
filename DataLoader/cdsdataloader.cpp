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
#include <QElapsedTimer>

CDSDataLoader::CDSDataLoader(int index, QObject* parent) : DataLoader(index, parent) {
    db = QSqlDatabase::addDatabase("QSQLITE", "cds_db" + QString::number(index));
    // db.setDatabaseName("cds.db");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return;
    }
    instQuery = QSqlQuery(db);
    multiQuery = QSqlQuery(db);
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
    for (int i = meta_size - 1; i >= 0; i--) {
        if (json_data[i] != '\0') {
            json_data.truncate(i+1);
            break;
        }
    }
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
    if (root.contains("result_level")) {
        QJsonArray levels = root["result_level"].toArray();
        for (auto it = levels.begin(); it != levels.end(); it++) {
            QJsonObject result_info = it->toObject();
            result_names.append(result_info.value("n").toString());
            result_levels.append(result_info.value("l").toInt());
        }
    }
    primary_key = root.value("primary_key").toString();

    QByteArray compress_insts = file.readAll();
    QByteArray insts;
    // if (root.contains("origin_size")) {
    //     quint64 origin_size = root.value("origin_size").toInteger();
    //     insts.resize(origin_size);
    //     qDebug() << "origin_size" << origin_size;
    //     quint64 actual_decompress_size = ZSTD_decompress(insts.data(), origin_size, compress_insts.data(), compress_insts.size());
    //     if (ZSTD_isError(actual_decompress_size)) {
    //         qWarning() << "decompress error" << actual_decompress_size << ZSTD_getErrorName(actual_decompress_size);
    //     }
    // } else {
        char* compress_data = compress_insts.data();
        int decompressed_size = 0;
        ZSTD_DCtx* zstd_dctx = ZSTD_createDCtx();
        while (decompressed_size < compress_insts.size()) {
            quint64 decompress_size = ZSTD_getFrameContentSize(compress_data, compress_insts.size());
            if (decompress_size == ZSTD_CONTENTSIZE_ERROR) {
                qWarning() << "decompress content size error";
            } else if (decompress_size == ZSTD_CONTENTSIZE_UNKNOWN) {
                qWarning() << "decompress content size unknown";
            }
            QByteArray decompress_data;
            decompress_data.resize(decompress_size);
            size_t compressed_size = ZSTD_findFrameCompressedSize(compress_data, compress_insts.size());
            quint64 actual_decompress_size = ZSTD_decompressDCtx(zstd_dctx, decompress_data.data(), decompress_data.size(), compress_data, compressed_size);
            if (ZSTD_isError(actual_decompress_size)) {
                qWarning() << "decompress error" << actual_decompress_size << ZSTD_getErrorName(actual_decompress_size);
                break;
            }
            compress_data += compressed_size;
            decompressed_size += compressed_size;
            decompress_data.truncate(actual_decompress_size);
            insts.append(decompress_data);
        }
        ZSTD_freeDCtx(zstd_dctx);
    // }
    qDebug() << "insts decompressed" << insts.size();
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

    constexpr int BATCH_SIZE = 50000; // 增加批处理大小
    quint64 current_size = 0;
    quint64 inst_size = insts.size();
    quint64 id = 0; // 初始化id变量，避免使用随机值

    // 计算每条记录的大小
    int record_size = 0;
    for (auto& meta : metas) {
        record_size += meta.size;
    }

    // 禁用SQLite同步机制以提高写入速度
    QSqlQuery pragmaQuery(db);
    pragmaQuery.exec("PRAGMA synchronous = OFF");
    pragmaQuery.exec("PRAGMA journal_mode = OFF");

    // 准备插入语句
    QElapsedTimer timer;
    timer.start();
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
    
    QVector<QVariantList> batchData;
    batchData.resize(metas.size()+1);
    for (int i = 0; i < metas.size()+1; i++) {
        batchData[i].resize(BATCH_SIZE);
    }
    while (current_size < inst_size) {
        quint64 remain_size = inst_size - current_size;
        int batch_count = remain_size / record_size;
        int origin_batch_count = batch_count;
        if (batch_count > BATCH_SIZE) {
            batch_count = BATCH_SIZE;
        }

        db.transaction();
        QSqlQuery batchQuery(db);
        if (!batchQuery.prepare(insertQuery)) {
            qDebug() << "batch prepare fail";
            db.rollback();
            continue;
        }

        auto prepareStartTime = std::chrono::high_resolution_clock::now();
        quint64 offset = current_size;
        int metas_size = metas.size();
        for (int j = 0; j < batch_count; j++) {
            for (int i = 0; i < metas_size; i++) {
                switch (metas[i].size) {
                case 1:
                    batchData[i][j] = *(quint8*)(insts.data() + offset);
                    break;
                case 2:
                    batchData[i][j] = *(quint16*)(insts.data() + offset);
                    break;
                case 4:
                    batchData[i][j] = *(quint32*)(insts.data() + offset); 
                    break;
                case 8:
                    batchData[i][j] = *(quint64*)(insts.data() + offset);
                    break;
                default:
                    break;
                }
                offset += metas[i].size;
            }
            if (!primary_key_valid) {
                batchData[metas_size][j] = id;
                id++;
            }
        }
        if (batch_count < BATCH_SIZE) {
            for (int i = 0; i < metas_size + 1; i++) {
                batchData[i].resize(batch_count);
            }
        }
        for (int i = 0; i < metas_size; i++) {
            batchQuery.addBindValue(batchData[i]);
        }
        if (!primary_key_valid) {
            batchQuery.addBindValue(batchData[metas_size]);
        }

        if (!batchQuery.execBatch()) {
            qCritical() << "batch insert failed:" << batchQuery.lastError().text() << id << current_size << inst_size;
            db.rollback();
            break;
        } else {
            db.commit();
            current_size += batch_count * record_size;
            inst_num += batch_count;
        }
    }

    instQuery.prepare("SELECT * FROM insts WHERE " + primary_key + " = ?");
    multiQuery.prepare("SELECT * FROM insts WHERE " + primary_key + " BETWEEN ? AND ?");

    qDebug() << "inst count" << inst_num << "insert time" << timer.elapsed() << "ms";
}

bool CDSDataLoader::getInst(quint64 id, Inst* inst) {
    instQuery.addBindValue(id);
    if (!instQuery.exec()) {
        return false;
    }
    if (!instQuery.next()) {
        return false;
    }
    QSqlRecord record = instQuery.record();
    inst->id = record.value(primary_key).toULongLong();
    inst->tick = record.value(0).toULongLong();
    inst->paddr = record.value(1).toULongLong();
    inst->type = record.value(2).toUInt();
    inst->result = record.value(3).toUInt();
    
    int delayCount = metas.size() - 4;
    for (int i = 4; i < metas.size(); i++) {
        inst->delay[i - 4] = record.value(i).toUInt();
    }
    return true; 
}

int CDSDataLoader::getMultiInst(quint64 id, int num, Inst* inst) {
    multiQuery.addBindValue(id);
    multiQuery.addBindValue(id + num - 1);
    if (!multiQuery.exec()) {
        qDebug() << "multi query failed" << multiQuery.lastError().text();
        return 0;
    }
    int current = 0;
    int delayCount = metas.size() - 4;
    while (current < num) {
        if (!multiQuery.next()) {
            return current;
        }
        QSqlRecord record = multiQuery.record();
        Inst& currentInst = inst[current];
        currentInst.id = record.value(primary_key).toULongLong();
        currentInst.tick = record.value(0).toULongLong();
        currentInst.paddr = record.value(1).toULongLong();
        currentInst.type = record.value(2).toUInt();
        currentInst.result = record.value(3).toUInt();
        
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

QVector<QString> CDSDataLoader::getInstTypes() {
    return type_names;
}

QVector<QString> CDSDataLoader::getResultNames() {
    return result_names;
}

QVector<int> CDSDataLoader::getResultLevels() {
    return result_levels;
}

QSqlQuery* CDSDataLoader::query(const QString& sql) {
    QSqlQuery* query = new QSqlQuery(db);
    if (!query->exec(sql)) {
        qDebug() << "query failed" << query->lastError().text();
        delete query;
        return nullptr;
    }
    return query;
}

QString CDSDataLoader::getPrimaryKey() {
    return primary_key;
}
