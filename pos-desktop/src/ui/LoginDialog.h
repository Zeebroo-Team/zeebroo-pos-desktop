#pragma once

#include <QDialog>

class QComboBox;
class QLineEdit;

namespace pos {

class ApiClient;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(ApiClient *api, QWidget *parent = nullptr);

    QString accessToken() const { return m_token; }
    int businessId() const { return m_businessId; }

private slots:
    void onLoginClicked();

private:
    void loadBusinesses();

    ApiClient *m_api = nullptr;
    QLineEdit *m_apiUrl = nullptr;
    QLineEdit *m_email = nullptr;
    QLineEdit *m_password = nullptr;
    QComboBox *m_business = nullptr;
    QString m_token;
    int m_businessId = 0;
};

} // namespace pos
