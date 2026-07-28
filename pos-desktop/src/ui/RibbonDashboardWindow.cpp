#include "ui/RibbonDashboardWindow.h"
#include "core/ApiClient.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>
#include <ribbonui.h>

namespace pos {

RibbonDashboardWindow::RibbonDashboardWindow(ApiClient *api, QObject *parent)
    : QObject(parent)
    , m_api(api)
{}

RibbonDashboardWindow::~RibbonDashboardWindow() = default;

void RibbonDashboardWindow::show()
{
    if (!m_engine) {
        m_engine = new QQmlApplicationEngine(this);

        m_engine->addImportPath(QStringLiteral("qrc:/qt/qml"));
        m_engine->addImportPath(QLibraryInfo::path(QLibraryInfo::QmlImportsPath));

        QObject::connect(m_engine, &QQmlApplicationEngine::objectCreationFailed,
                         this, [this](const QUrl &) {
            m_engine->deleteLater();
            m_engine = nullptr;
        }, Qt::QueuedConnection);

        QObject::connect(m_engine, &QQmlApplicationEngine::objectCreated,
                         this, [](QObject *obj, const QUrl &) {
            if (!obj) return;
            if (auto *win = qobject_cast<QWindow *>(obj)) {
                win->show();
                win->raise();
                win->requestActivate();
            }
        }, Qt::QueuedConnection);

        RibbonUI::registerTypes("RibbonUI");

        m_engine->rootContext()->setContextProperty(QStringLiteral("posApi"), m_api);
        m_engine->rootContext()->setContextProperty(QStringLiteral("appController"), this);
        m_engine->load(QUrl(QStringLiteral("qrc:/qml/RibbonDashboard.qml")));
        return;
    }

    // Window was hidden — bring it back.
    const auto roots = m_engine->rootObjects();
    for (QObject *obj : roots) {
        if (auto *win = qobject_cast<QWindow *>(obj)) {
            win->show();
            win->raise();
            win->requestActivate();
            return;
        }
    }

    m_engine->load(QUrl(QStringLiteral("qrc:/qml/RibbonDashboard.qml")));
}

void RibbonDashboardWindow::openPos()
{
    if (!m_posWindow) {
        m_posWindow = new MainWindow(m_api);
        // When POS window closes normally, just destroy it so it can be reopened later.
        QObject::connect(m_posWindow, &QObject::destroyed, this, [this]() {
            m_posWindow = nullptr;
        });
    }
    m_posWindow->showMaximized();
    m_posWindow->raise();
    m_posWindow->activateWindow();

    // Hide the ribbon dashboard while POS is in use.
    if (m_engine) {
        const auto roots = m_engine->rootObjects();
        for (QObject *obj : roots) {
            if (auto *win = qobject_cast<QWindow *>(obj)) {
                win->hide();
                break;
            }
        }
    }
}

void RibbonDashboardWindow::submitBill(const QVariantMap &data)
{
    QJsonObject body;
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        const QVariant &v = it.value();
        if (!v.isValid() || v.isNull()) continue;
        switch (v.typeId()) {
        case QMetaType::Double:
        case QMetaType::Float:
            body.insert(it.key(), v.toDouble()); break;
        case QMetaType::Int:
        case QMetaType::LongLong:
            body.insert(it.key(), v.toLongLong()); break;
        case QMetaType::Bool:
            body.insert(it.key(), v.toBool()); break;
        default:
            body.insert(it.key(), v.toString()); break;
        }
    }

    m_api->createBill(body,
        [this](const QJsonObject &resp) {
            const QString name = resp.value(QStringLiteral("data"))
                                    .toObject()
                                    .value(QStringLiteral("name"))
                                    .toString();
            emit billCreated(name.isEmpty() ? QStringLiteral("Bill") : name);
        },
        [this](const QString &msg, int /*status*/) {
            emit billError(msg);
        }
    );
}

void RibbonDashboardWindow::fetchAccounts()
{
    m_api->fetchAccounts(
        [this](const QJsonObject &resp) {
            const QJsonArray arr = resp.value(QStringLiteral("data")).toArray();
            QVariantList list;
            list.reserve(arr.size());
            for (const QJsonValue &v : arr) {
                const QJsonObject o = v.toObject();
                list.append(QVariantMap{
                    {QStringLiteral("id"),           o.value(QStringLiteral("id")).toInt()},
                    {QStringLiteral("account_name"), o.value(QStringLiteral("account_name")).toString()},
                    {QStringLiteral("bank_name"),    o.value(QStringLiteral("bank_name")).toString()},
                });
            }
            emit accountsLoaded(list);
        },
        [](const QString &, int) {}
    );
}

void RibbonDashboardWindow::fetchBillAssignmentTargets()
{
    m_api->fetchBillAssignmentTargets(
        [this](const QJsonObject &resp) {
            const QJsonObject data = resp.value(QStringLiteral("data")).toObject();
            auto toList = [](const QJsonArray &arr) {
                QVariantList list;
                list.reserve(arr.size());
                for (const QJsonValue &v : arr) {
                    const QJsonObject o = v.toObject();
                    list.append(QVariantMap{
                        {QStringLiteral("id"),   o.value(QStringLiteral("id")).toInt()},
                        {QStringLiteral("name"), o.value(QStringLiteral("name")).toString()},
                    });
                }
                return list;
            };
            QVariantMap targets;
            targets.insert(QStringLiteral("branches"),      toList(data.value(QStringLiteral("branches")).toArray()));
            targets.insert(QStringLiteral("departments"),   toList(data.value(QStringLiteral("departments")).toArray()));
            targets.insert(QStringLiteral("properties"),    toList(data.value(QStringLiteral("properties")).toArray()));
            targets.insert(QStringLiteral("employees"),     toList(data.value(QStringLiteral("employees")).toArray()));
            targets.insert(QStringLiteral("modifications"), toList(data.value(QStringLiteral("modifications")).toArray()));
            targets.insert(QStringLiteral("rentals"),       toList(data.value(QStringLiteral("rentals")).toArray()));
            emit billAssignmentTargetsLoaded(targets);
        },
        [this](const QString &msg, int) {
            emit billAssignmentTargetsError(msg);
        }
    );
}

void RibbonDashboardWindow::fetchLoans()
{
    m_api->fetchLoans(
        [this](const QJsonObject &resp) {
            const QJsonArray arr = resp.value(QStringLiteral("data")).toArray();
            QVariantList list;
            list.reserve(arr.size());
            for (const QJsonValue &v : arr) {
                const QJsonObject o = v.toObject();
                list.append(QVariantMap{
                    {QStringLiteral("id"),              o.value(QStringLiteral("id")).toInt()},
                    {QStringLiteral("name"),            o.value(QStringLiteral("name")).toString()},
                    {QStringLiteral("borrowed_amount"), o.value(QStringLiteral("borrowed_amount")).toDouble()},
                });
            }
            emit loansLoaded(list);
        },
        [](const QString &, int) {}
    );
}

void RibbonDashboardWindow::fetchBills()
{
    m_api->fetchBills(
        [this](const QJsonObject &resp) {
            const QJsonArray arr = resp.value(QStringLiteral("data")).toArray();
            QVariantList list;
            list.reserve(arr.size());
            for (const QJsonValue &v : arr) {
                const QJsonObject o = v.toObject();
                list.append(QVariantMap{
                    {QStringLiteral("id"),                o.value(QStringLiteral("id")).toInt()},
                    {QStringLiteral("name"),              o.value(QStringLiteral("name")).toString()},
                    {QStringLiteral("amount"),            o.value(QStringLiteral("amount")).toDouble()},
                    {QStringLiteral("overdue"),           o.value(QStringLiteral("overdue")).toBool()},
                    {QStringLiteral("due_date"),             o.value(QStringLiteral("due_date")).toString()},
                    {QStringLiteral("due_date_fmt"),         o.value(QStringLiteral("due_date_fmt")).toString()},
                    {QStringLiteral("actual_due_date_fmt"),  o.value(QStringLiteral("actual_due_date_fmt")).toString()},
                    {QStringLiteral("first_install_date_fmt"), o.value(QStringLiteral("first_install_date_fmt")).toString()},
                    {QStringLiteral("payment_mode"),      o.value(QStringLiteral("payment_mode")).toString()},
                    {QStringLiteral("category"),          o.value(QStringLiteral("category")).toString()},
                    {QStringLiteral("category_label"),    o.value(QStringLiteral("category_label")).toString()},
                    {QStringLiteral("description"),       o.value(QStringLiteral("description")).toString()},
                    {QStringLiteral("recurring_type"),    o.value(QStringLiteral("recurring_type")).toString()},
                    {QStringLiteral("agreement_until"),   o.value(QStringLiteral("agreement_until")).toString()},
                    {QStringLiteral("remind_days"),       o.value(QStringLiteral("remind_days")).toInt()},
                    {QStringLiteral("notes"),             o.value(QStringLiteral("notes")).toString()},
                    {QStringLiteral("property_name"),     o.value(QStringLiteral("property_name")).toString()},
                    {QStringLiteral("employee_name"),     o.value(QStringLiteral("employee_name")).toString()},
                    {QStringLiteral("modification_name"), o.value(QStringLiteral("modification_name")).toString()},
                    {QStringLiteral("department_name"),   o.value(QStringLiteral("department_name")).toString()},
                    {QStringLiteral("rental_type"),       o.value(QStringLiteral("rental_type")).toString()},
                    {QStringLiteral("branch_name"),       o.value(QStringLiteral("branch_name")).toString()},
                });
            }
            emit billsLoaded(list);
        },
        [](const QString &, int) {}
    );
}

void RibbonDashboardWindow::fetchRentals()
{
    m_api->fetchRentals(
        [this](const QJsonObject &resp) {
            const QJsonArray arr = resp.value(QStringLiteral("data")).toArray();
            QVariantList list;
            list.reserve(arr.size());
            for (const QJsonValue &v : arr) {
                const QJsonObject o = v.toObject();
                list.append(QVariantMap{
                    {QStringLiteral("id"),     o.value(QStringLiteral("id")).toInt()},
                    {QStringLiteral("name"),   o.value(QStringLiteral("name")).toString()},
                    {QStringLiteral("amount"), o.value(QStringLiteral("amount")).toDouble()},
                });
            }
            emit rentalsLoaded(list);
        },
        [](const QString &, int) {}
    );
}

void RibbonDashboardWindow::fetchModifications()
{
    m_api->fetchModifications(
        [this](const QJsonObject &resp) {
            const QJsonArray arr = resp.value(QStringLiteral("data")).toArray();
            QVariantList list;
            list.reserve(arr.size());
            for (const QJsonValue &v : arr) {
                const QJsonObject o = v.toObject();
                list.append(QVariantMap{
                    {QStringLiteral("id"),     o.value(QStringLiteral("id")).toInt()},
                    {QStringLiteral("name"),   o.value(QStringLiteral("name")).toString()},
                    {QStringLiteral("amount"), o.value(QStringLiteral("amount")).toDouble()},
                });
            }
            emit modificationsLoaded(list);
        },
        [](const QString &, int) {}
    );
}

void RibbonDashboardWindow::fetchEmployees()
{
    m_api->fetchEmployees(
        [this](const QJsonObject &resp) {
            const QJsonArray arr = resp.value(QStringLiteral("data")).toArray();
            QVariantList list;
            list.reserve(arr.size());
            for (const QJsonValue &v : arr) {
                const QJsonObject o = v.toObject();
                list.append(QVariantMap{
                    {QStringLiteral("id"),   o.value(QStringLiteral("id")).toInt()},
                    {QStringLiteral("name"), o.value(QStringLiteral("name")).toString()},
                    {QStringLiteral("type"), o.value(QStringLiteral("type")).toString()},
                });
            }
            emit employeesLoaded(list);
        },
        [](const QString &, int) {}
    );
}

} // namespace pos
