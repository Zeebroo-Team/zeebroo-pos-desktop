#pragma once
#include <QObject>
#include <QString>

class QQmlApplicationEngine;

namespace pos {

class ApiClient;
class MainWindow;

class RibbonDashboardWindow : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RibbonDashboardWindow)
    Q_PROPERTY(QString businessName READ businessName WRITE setBusinessName NOTIFY businessNameChanged)
public:
    explicit RibbonDashboardWindow(ApiClient *api, QObject *parent = nullptr);
    ~RibbonDashboardWindow() override;

    void show();

    QString businessName() const { return m_businessName; }
    void    setBusinessName(const QString &n) { if (m_businessName != n) { m_businessName = n; emit businessNameChanged(); } }

    Q_INVOKABLE void openPos();
    Q_INVOKABLE void submitBill(const QVariantMap &data);
    Q_INVOKABLE void fetchLoans();
    Q_INVOKABLE void fetchBills();
    Q_INVOKABLE void fetchRentals();
    Q_INVOKABLE void fetchModifications();
    Q_INVOKABLE void fetchEmployees();
    Q_INVOKABLE void fetchAccounts();
    Q_INVOKABLE void fetchBillAssignmentTargets();

signals:
    void businessNameChanged();
    void billCreated(const QString &billName);
    void billError(const QString &message);
    void loansLoaded(const QVariantList &loans);
    void billsLoaded(const QVariantList &bills);
    void rentalsLoaded(const QVariantList &rentals);
    void modificationsLoaded(const QVariantList &modifications);
    void employeesLoaded(const QVariantList &employees);
    void accountsLoaded(const QVariantList &accounts);
    void billAssignmentTargetsLoaded(const QVariantMap &targets);
    void billAssignmentTargetsError(const QString &message);

private:
    ApiClient             *m_api          = nullptr;
    QQmlApplicationEngine *m_engine       = nullptr;
    MainWindow            *m_posWindow    = nullptr;
    QString                m_businessName;
};

} // namespace pos
