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
    pos::PosBeep::preload();

    pos::ApiClient api;

    // Sign-out loop: if the user signs out, show the login screen again.
    for (;;) {
        pos::LoginDialog login(&api);
        if (login.exec() != QDialog::Accepted)
            break;

        api.setAccessToken(login.accessToken());
        api.setBusinessId(login.businessId());

        bool signedOut = false;
        {
            pos::MainWindow window(&api);
            window.showMaximized();
            app.exec();                     // runs until window closes
            signedOut = window.wasSignedOut();
        }

        if (!signedOut)
            break;

        // Clear credentials and loop back to login
        api.setAccessToken(QString());
        api.setBusinessId(0);
    }

    return 0;
}
