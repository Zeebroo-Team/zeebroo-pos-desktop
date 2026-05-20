#include "core/ApiClient.h"
#include "core/Config.h"
#include "core/PosBeep.h"
#include "ui/LoginDialog.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("Zeebroo"));
    QApplication::setApplicationName(QStringLiteral("PosDesktop"));

    QFile styleFile(QStringLiteral(":/styles.qss"));
    if (styleFile.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    pos::Config::instance().load();

    pos::ApiClient api;
    pos::LoginDialog login(&api);
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    api.setAccessToken(login.accessToken());
    api.setBusinessId(login.businessId());

    pos::PosBeep::preload();

    pos::MainWindow window(&api);
    window.showMaximized();

    return app.exec();
}
