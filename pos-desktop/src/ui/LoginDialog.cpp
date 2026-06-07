#include "ui/LoginDialog.h"

#include "core/ApiClient.h"
#include "core/Config.h"

#include <QComboBox>
#include <QGraphicsDropShadowEffect>
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
    setFixedSize(460, 530);
    setObjectName(QStringLiteral("loginDialog"));

    Config::instance().load();

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Brand header ──────────────────────────────────────────────
    auto *brandArea = new QWidget(this);
    brandArea->setObjectName(QStringLiteral("loginBrand"));
    brandArea->setFixedHeight(148);

    auto *brandLayout = new QVBoxLayout(brandArea);
    brandLayout->setContentsMargins(32, 28, 32, 20);
    brandLayout->setSpacing(10);
    brandLayout->setAlignment(Qt::AlignCenter);

    auto *logoLabel = new QLabel(brandArea);
    logoLabel->setObjectName(QStringLiteral("loginLogo"));
    QPixmap logo(QStringLiteral(":/logo.png"));
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(200, 68, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoLabel->setText(QStringLiteral("ZEEBROO POS"));
        logoLabel->setObjectName(QStringLiteral("loginLogoFallback"));
    }
    logoLabel->setAlignment(Qt::AlignCenter);
    brandLayout->addWidget(logoLabel);

    auto *tagline = new QLabel(tr("Point of Sale Desktop"), brandArea);
    tagline->setObjectName(QStringLiteral("loginTagline"));
    tagline->setAlignment(Qt::AlignCenter);
    brandLayout->addWidget(tagline);

    root->addWidget(brandArea);

    // ── Divider ───────────────────────────────────────────────────
    auto *divider = new QWidget(this);
    divider->setObjectName(QStringLiteral("loginDivider"));
    divider->setFixedHeight(1);
    root->addWidget(divider);

    // ── Form card ─────────────────────────────────────────────────
    auto *formCard = new QWidget(this);
    formCard->setObjectName(QStringLiteral("loginCard"));
    formCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *formLayout = new QVBoxLayout(formCard);
    formLayout->setContentsMargins(36, 28, 36, 32);
    formLayout->setSpacing(0);

    // Title + sub
    auto *titleLabel = new QLabel(tr("Sign in to your account"), formCard);
    titleLabel->setObjectName(QStringLiteral("loginTitle"));
    titleLabel->setAlignment(Qt::AlignLeft);
    formLayout->addWidget(titleLabel);

    formLayout->addSpacing(4);

    auto *subLabel = new QLabel(tr("Enter your credentials to connect to the POS server"), formCard);
    subLabel->setObjectName(QStringLiteral("loginSub"));
    subLabel->setWordWrap(true);
    formLayout->addWidget(subLabel);

    formLayout->addSpacing(24);

    // Email
    auto *emailLabel = new QLabel(tr("Email address"), formCard);
    emailLabel->setObjectName(QStringLiteral("loginFieldLabel"));
    formLayout->addWidget(emailLabel);
    formLayout->addSpacing(5);

    m_email = new QLineEdit(formCard);
    m_email->setObjectName(QStringLiteral("loginField"));
    m_email->setPlaceholderText(tr("you@example.com"));
    m_email->setFixedHeight(40);
    formLayout->addWidget(m_email);
    formLayout->addSpacing(14);

    // Password
    auto *passLabel = new QLabel(tr("Password"), formCard);
    passLabel->setObjectName(QStringLiteral("loginFieldLabel"));
    formLayout->addWidget(passLabel);
    formLayout->addSpacing(5);

    m_password = new QLineEdit(formCard);
    m_password->setObjectName(QStringLiteral("loginField"));
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setPlaceholderText(QStringLiteral("••••••••"));
    m_password->setFixedHeight(40);
    formLayout->addWidget(m_password);
    formLayout->addSpacing(14);

    // Business
    auto *bizLabel = new QLabel(tr("Business"), formCard);
    bizLabel->setObjectName(QStringLiteral("loginFieldLabel"));
    formLayout->addWidget(bizLabel);
    formLayout->addSpacing(5);

    m_business = new QComboBox(formCard);
    m_business->setObjectName(QStringLiteral("loginCombo"));
    m_business->setEnabled(false);
    m_business->setFixedHeight(40);
    m_business->addItem(tr("— sign in to load businesses —"));
    formLayout->addWidget(m_business);

    formLayout->addStretch();
    formLayout->addSpacing(8);

    // Sign-in button
    auto *loginBtn = new QPushButton(tr("Sign in"), formCard);
    loginBtn->setObjectName(QStringLiteral("loginBtn"));
    loginBtn->setDefault(true);
    loginBtn->setFixedHeight(44);
    loginBtn->setCursor(Qt::PointingHandCursor);
    formLayout->addWidget(loginBtn);

    root->addWidget(formCard);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_password, &QLineEdit::returnPressed, loginBtn, &QPushButton::click);
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
            if (m_business->count() == 0) {
                QMessageBox::warning(this, tr("No business found"),
                                     tr("No business is associated with your account. "
                                        "Please contact your administrator."));
                m_token.clear();
                m_api->setAccessToken(QString());
            } else if (m_business->count() == 1) {
                m_businessId = m_business->currentData().toInt();
                accept();
            } else {
                QMessageBox::information(this, tr("Select business"),
                                         tr("You have multiple businesses — select one and click Sign in again."));
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
            QMessageBox::warning(this, tr("Sign in"), tr("Please select a business."));
            return;
        }
        accept();
        return;
    }

    const QString email    = m_email->text().trimmed();
    const QString password = m_password->text();
    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("Sign in"), tr("Please enter your email and password."));
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
