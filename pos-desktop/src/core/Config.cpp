#include "core/Config.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace pos {

Config &Config::instance()
{
    static Config cfg;
    return cfg;
}

QString Config::configPath() const
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + QStringLiteral("/config.json");
}

bool Config::load()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        configPath(),
        appDir + QStringLiteral("/config.json"),
        appDir + QStringLiteral("/../config.json"),
    };

    for (const QString &path : candidates) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }
        const QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
        if (root.contains(QStringLiteral("api_base_url"))) {
            m_apiBaseUrl = root.value(QStringLiteral("api_base_url")).toString(m_apiBaseUrl).trimmed();
        }
        if (root.contains(QStringLiteral("device_name"))) {
            m_deviceName = root.value(QStringLiteral("device_name")).toString(m_deviceName).trimmed();
        }
        while (m_apiBaseUrl.endsWith(QLatin1Char('/'))) {
            m_apiBaseUrl.chop(1);
        }
        return true;
    }
    return false;
}

bool Config::save() const
{
    QJsonObject root;
    root.insert(QStringLiteral("api_base_url"), m_apiBaseUrl);
    root.insert(QStringLiteral("device_name"), m_deviceName);

    QFile file(configPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

void Config::setApiBaseUrl(const QString &url)
{
    m_apiBaseUrl = url.trimmed();
    while (m_apiBaseUrl.endsWith(QLatin1Char('/'))) {
        m_apiBaseUrl.chop(1);
    }
}

void Config::setDeviceName(const QString &name)
{
    m_deviceName = name.trimmed();
}

} // namespace pos
