#include "core/ApiClient.h"
#include "core/Config.h"
#include "core/PosBeep.h"
#include "ui/LoginDialog.h"
#include "ui/RibbonDashboardWindow.h"

#include <QApplication>
#include <QFile>
#include <QtPlugin>
#include <ribbonui.h>

// Force the macOS linker to include the RibbonUI static QML plugin object from
// the archive (dead-code stripping would otherwise omit it, silently breaking
// "import RibbonUI" at runtime).
Q_IMPORT_PLUGIN(RibbonUIPlugin)

int main(int argc, char *argv[])
{
    // Must be called before QApplication: sets DPI policy and alpha buffer flag.
    RibbonUI::init();

    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("Zeebroo"));
    QApplication::setApplicationName(QStringLiteral("PosDesktop"));

    // Prevent Qt from quitting when the login dialog closes and the QML
    // window hasn't been created yet (zero open windows at that instant).
    // The ribbon window calls Qt.quit() explicitly via its close dialog.
    QApplication::setQuitOnLastWindowClosed(false);

    QFile styleFile(QStringLiteral(":/styles.qss"));
    if (styleFile.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    pos::Config::instance().load();
    pos::PosBeep::preload();

    pos::ApiClient api;

    pos::LoginDialog login(&api);
    if (login.exec() == QDialog::Accepted) {
        api.setAccessToken(login.accessToken());
        api.setBusinessId(login.businessId());
        api.setBranchId(login.branchId());

        pos::RibbonDashboardWindow dashboard(&api);
        dashboard.setBusinessName(login.businessName());
        dashboard.show();
        app.exec();
    }

    return 0;
}
