#include "ui/LoginDialog.h"

#include "core/ApiClient.h"
#include "core/Config.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace pos {

LoginDialog::LoginDialog(ApiClient *api, QWidget *parent)
    : QDialog(parent)
    , m_api(api)
{
    setWindowTitle(tr("Zeebroo POS — Sign in"));
    setModal(true);
    resize(440, 320);

    Config::instance().load();

    auto *layout = new QVBoxLayout(this);
    auto *heading = new QLabel(tr("<h2>Zeebroo POS Desktop</h2><p>Connect to your Laravel POS API.</p>"), this);
    heading->setWordWrap(true);
    layout->addWidget(heading);

    auto *form = new QFormLayout();
    m_apiUrl = new QLineEdit(Config::instance().apiBaseUrl(), this);
    m_email = new QLineEdit(this);
    m_email->setPlaceholderText(tr("you@example.com"));
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    m_business = new QComboBox(this);
    m_business->setEnabled(false);

    form->addRow(tr("API base URL"), m_apiUrl);
    form->addRow(tr("Email"), m_email);
    form->addRow(tr("Password"), m_password);
    form->addRow(tr("Business"), m_business);
    layout->addLayout(form);

    auto *row = new QHBoxLayout();
    auto *loginBtn = new QPushButton(tr("Sign in"), this);
    loginBtn->setDefault(true);
    loginBtn->setObjectName(QStringLiteral("primaryButton"));
    row->addStretch();
    row->addWidget(loginBtn);
    layout->addLayout(row);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
}

void LoginDialog::loadBusinesses()
{
    m_business->clear();
    m_api->fetchBusinesses(
        [this](const QJsonObject &root) {
            const QJsonArray data = root.value(QStringLiteral("data")).toArray();
            for (const QJsonValue &v : data) {
                const QJsonObject o = v.toObject();
                const int id = o.value(QStringLiteral("id")).toInt();
                const QString name = o.value(QStringLiteral("name")).toString();
                m_business->addItem(name, id);
            }
            m_business->setEnabled(m_business->count() > 0);
            if (m_business->count() == 1) {
                m_businessId = m_business->currentData().toInt();
                accept();
            } else if (m_business->count() > 1) {
                QMessageBox::information(this, tr("Sign in"),
                                       tr("Select your business and click Sign in again."));
            }
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Businesses"), msg);
        });
}

void LoginDialog::onLoginClicked()
{
    if (!m_token.isEmpty()) {
        m_businessId = m_business->currentData().toInt();
        if (m_businessId <= 0) {
            QMessageBox::warning(this, tr("Sign in"), tr("Select a business."));
            return;
        }
        accept();
        return;
    }

    Config::instance().setApiBaseUrl(m_apiUrl->text());
    Config::instance().save();

    const QString email = m_email->text().trimmed();
    const QString password = m_password->text();
    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("Sign in"), tr("Enter email and password."));
        return;
    }

    m_api->login(
        email,
        password,
        [this](const QJsonObject &root) {
            m_token = root.value(QStringLiteral("access_token")).toString();
            m_api->setAccessToken(m_token);
            loadBusinesses();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Sign in"), msg);
        });
}

} // namespace pos
