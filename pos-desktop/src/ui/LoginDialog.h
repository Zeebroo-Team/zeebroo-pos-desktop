#pragma once

#include <QDialog>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

namespace pos {

class ApiClient;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(ApiClient *api, QWidget *parent = nullptr);

    QString accessToken()  const { return m_token; }
    QString businessName() const { return m_businessName; }
    int     businessId()   const { return m_businessId; }
    int     branchId()     const { return m_branchId; }

private slots:
    void onLoginClicked();

private:
    enum class Step { Auth, SelectBusiness, SelectBranch };

    void loadBusinesses();
    void loadBranches();

    ApiClient  *m_api          = nullptr;
    QLineEdit  *m_email        = nullptr;
    QLineEdit  *m_password     = nullptr;
    QWidget    *m_businessRow  = nullptr;
    QComboBox  *m_business     = nullptr;
    QWidget    *m_branchRow    = nullptr;
    QComboBox  *m_branchCombo  = nullptr;
    QPushButton *m_loginBtn    = nullptr;
    QString     m_token;
    QString     m_businessName;
    int         m_businessId   = 0;
    int         m_branchId     = 0;
    Step        m_step         = Step::Auth;
};

} // namespace pos
