#ifndef DATALOADER_H
#define DATALOADER_H
#include "../InstViewer/instdefines.h"
#include <QString>
#include <QVector>
#include <QObject>
#include <QMap>
#include <QMetaObject>
#include <QFileInfo>
#include <QDateTime>
#include <QSqlQuery>

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

    DataLoader(int index, QObject* parent = nullptr) : QObject(parent) {}
    virtual ~DataLoader() = default;
    virtual bool getInst(quint64 id, Inst* inst) = 0;
    virtual int getMultiInst(quint64 id, int num, Inst* inst) = 0;
    // QSqlQuery* manage by caller
    virtual QSqlQuery* query(const QString& sql) = 0;
    virtual void load(const QString& path){}
    virtual inline quint64 getInstCount() {return 0;}
    virtual QVector<QString> getDelayNames() { return QVector<QString>(); }
    virtual QVector<QString> getResultNames() { return QVector<QString>(); }
    virtual QVector<int> getResultLevels() { return QVector<int>(); }
    static void registerLoader(const QString& name, const QMetaObject& meta) {
        loader_meta[name] = meta;
    }
    virtual void getInstTicks(Inst* inst, quint64* ticks) {}
    virtual QString getTypeName(quint8 type) { return QString(); }
    virtual int getTypeNum() { return 0; }
    virtual QString getPrimaryKey() { return QString(); }
    virtual QVector<QString> getInstTypes() { return QVector<QString>(); }

    static QMap<QString, QMetaObject> loader_meta;
};

class DataLoaderFactory {
public:
    static DataLoader* createLoader(const QString& name, const QString& path, QObject* parent = nullptr) {
        QFileInfo fileInfo(path);
        QDateTime currentModifyTime;
        bool needCreateNew = false;
        
        // 检查路径是否存在
        if (!fileInfo.exists()) {
            return nullptr;
        } else {
            currentModifyTime = fileInfo.lastModified();
            
            // 检查修改时间是否在所有loaders之后
            bool isNewer = true;
            for (const LoaderMeta& meta : loaders) {
                if (meta.path == path && meta.lastModifytime >= currentModifyTime) {
                    isNewer = false;
                    break;
                }
            }
            needCreateNew = isNewer;
        }
        
        if (needCreateNew) {
            if (!DataLoader::loader_meta.contains(name)) {
                qCritical() << "DataLoader" << name << "unregistered";
                return nullptr;
            }
            // 构建新的loader
            DataLoader* newLoader = (DataLoader*)DataLoader::loader_meta[name].newInstance(loaderIdx, parent);
            if (newLoader) {
                newLoader->load(path);
                // 更新loaders列表
                bool found = false;
                for (int i = 0; i < loaders.size(); i++) {
                    if (loaders[i].path == path) {
                        loaders[i].loader = newLoader;
                        loaders[i].lastModifytime = currentModifyTime;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    LoaderMeta meta;
                    meta.path = path;
                    meta.lastModifytime = currentModifyTime;
                    meta.loader = newLoader;
                    loaders.append(meta);
                }
            }
            return newLoader;
        } else {
            // 返回已存在的loader
            for (const LoaderMeta& meta : loaders) {
                if (meta.path == path) {
                    return meta.loader;
                }
            }
            return nullptr;
        }
    }

private:
    struct LoaderMeta {
        QString path;
        QDateTime lastModifytime;
        DataLoader* loader;
    };
    static QVector<LoaderMeta> loaders;
    static int loaderIdx;
};

#endif // DATALOADER_H
