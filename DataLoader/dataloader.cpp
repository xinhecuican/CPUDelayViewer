#include "dataloader.h"

// 初始化静态成员变量
QMap<QString, QMetaObject> DataLoader::loader_meta;
QVector<DataLoaderFactory::LoaderMeta> DataLoaderFactory::loaders;
int DataLoaderFactory::loaderIdx = 0;