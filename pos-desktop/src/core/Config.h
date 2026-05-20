#pragma once

#include <QString>

namespace pos {

class Config {
public:
    static Config &instance();

    QString apiBaseUrl() const { return m_apiBaseUrl; }
    QString deviceName() const { return m_deviceName; }

    void setApiBaseUrl(const QString &url);
    void setDeviceName(const QString &name);

    bool load();
    bool save() const;

    QString configPath() const;

private:
    Config() = default;

    QString m_apiBaseUrl = QStringLiteral("http://localhost:8000/api/v1/pos");
    QString m_deviceName = QStringLiteral("pos-desktop-1");
};

} // namespace pos
