#ifndef CONFIG_H
#define CONFIG_H
#include <QSettings>
#include <QRect>
#include <QString>

class Config {
public:


    static bool getBool(const QString& name, bool default_value=false) {
        return instance().setting.value(name, default_value).toBool();
    }
    static int getInt(const QString& name, int default_value=0) {
        return instance().setting.value(name, default_value).toInt();
    }
    static qreal getReal(const QString& name, qreal default_value=0) {
        return instance().setting.value(name, default_value).toReal();
    }
    static QRect getRect(const QString& name, const QRect& rect=QRect()) {
        return instance().setting.value(name, rect).toRect();
    }
    static QString getStr(const QString& name, const QString& default_value=QString()) {
        return instance().setting.value(name, default_value).toString();
    }
    static QVariant get(const QString& name, const QVariant& variant=QVariant()) {
        return instance().setting.value(name, variant);
    }
    static void set(const QString& name, const QVariant& value) {
        instance().setting.setValue(name, value);
    }

private:
    Config() : setting("CPUDelayViewer"){

    }
    static Config& instance() {
        static Config config;
        return config;
    }

    QSettings setting;
};

#endif // CONFIG_H
