#include "ui/LoginDialog.h"

#include "core/ApiClient.h"
#include "core/Config.h"

#include <QComboBox>
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
    setFixedWidth(460);
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

    // Title
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

    // ── Business row (hidden until authenticated) ─────────────────
    m_businessRow = new QWidget(formCard);
    m_businessRow->setVisible(false);
    auto *bizRowLayout = new QVBoxLayout(m_businessRow);
    bizRowLayout->setContentsMargins(0, 14, 0, 0);
    bizRowLayout->setSpacing(5);

    auto *bizLabel = new QLabel(tr("Business"), m_businessRow);
    bizLabel->setObjectName(QStringLiteral("loginFieldLabel"));
    bizRowLayout->addWidget(bizLabel);

    m_business = new QComboBox(m_businessRow);
    m_business->setObjectName(QStringLiteral("loginCombo"));
    m_business->setFixedHeight(40);
    bizRowLayout->addWidget(m_business);

    formLayout->addWidget(m_businessRow);

    // ── Branch row (hidden until branch_pos_separate is enabled) ──
    m_branchRow = new QWidget(formCard);
    m_branchRow->setVisible(false);
    auto *branchRowLayout = new QVBoxLayout(m_branchRow);
    branchRowLayout->setContentsMargins(0, 14, 0, 0);
    branchRowLayout->setSpacing(5);

    auto *branchLabel = new QLabel(tr("Branch"), m_branchRow);
    branchLabel->setObjectName(QStringLiteral("loginFieldLabel"));
    branchRowLayout->addWidget(branchLabel);

    m_branchCombo = new QComboBox(m_branchRow);
    m_branchCombo->setObjectName(QStringLiteral("loginCombo"));
    m_branchCombo->setFixedHeight(40);
    branchRowLayout->addWidget(m_branchCombo);

    formLayout->addWidget(m_branchRow);

    formLayout->addSpacing(24);

    // Sign-in button
    m_loginBtn = new QPushButton(tr("Sign in"), formCard);
    m_loginBtn->setObjectName(QStringLiteral("loginBtn"));
    m_loginBtn->setDefault(true);
    m_loginBtn->setFixedHeight(44);
    m_loginBtn->setCursor(Qt::PointingHandCursor);
    formLayout->addWidget(m_loginBtn);

    formLayout->addSpacing(4);

    root->addWidget(formCard);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_password, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);
}

void LoginDialog::loadBusinesses()
{
    m_loginBtn->setEnabled(false);
    m_loginBtn->setText(tr("Loading…"));
    m_api->fetchBusinesses(
        [this](const QJsonObject &root) {
            m_loginBtn->setEnabled(true);
            const QJsonArray data = root.value(QStringLiteral("data")).toArray();
            if (data.isEmpty()) {
                m_loginBtn->setText(tr("Sign in"));
                m_token.clear();
                m_api->setAccessToken(QString());
                QMessageBox::warning(this, tr("No business found"),
                                     tr("No business is associated with your account. "
                                        "Please contact your administrator."));
                return;
            }
            m_business->clear();
            for (const QJsonValue &v : data) {
                const QJsonObject o = v.toObject();
                m_business->addItem(o.value(QStringLiteral("name")).toString(),
                                    o.value(QStringLiteral("id")).toInt());
            }
            const int bizH = m_businessRow->sizeHint().height();
            m_businessRow->setVisible(true);
            m_step = Step::SelectBusiness;
            m_loginBtn->setText(tr("Continue"));
            resize(width(), height() + bizH);
        },
        [this](const QString &msg, int) {
            m_loginBtn->setEnabled(true);
            m_loginBtn->setText(tr("Sign in"));
            QMessageBox::warning(this, tr("Sign in"), msg);
        });
}

void LoginDialog::loadBranches()
{
    m_loginBtn->setEnabled(false);
    m_loginBtn->setText(tr("Loading…"));
    m_api->fetchBranches(
        [this](const QJsonObject &root) {
            m_loginBtn->setEnabled(true);
            const QJsonObject data = root.value(QStringLiteral("data")).toObject();
            const bool separate = data.value(QStringLiteral("branch_pos_separate")).toBool();
            const QJsonArray arr = data.value(QStringLiteral("branches")).toArray();

            if (!separate || arr.isEmpty()) {
                accept();
                return;
            }

            m_branchCombo->clear();
            for (const QJsonValue &v : arr) {
                const QJsonObject o = v.toObject();
                m_branchCombo->addItem(o.value(QStringLiteral("name")).toString(),
                                       o.value(QStringLiteral("id")).toInt());
            }
            const int branchH = m_branchRow->sizeHint().height();
            m_branchRow->setVisible(true);
            m_step = Step::SelectBranch;
            m_loginBtn->setText(tr("Sign in"));
            resize(width(), height() + branchH);
        },
        [this](const QString &msg, int status) {
            m_loginBtn->setEnabled(true);
            m_loginBtn->setText(tr("Continue"));
            QMessageBox::warning(this, tr("Branches"),
                tr("Could not load branches (status %1): %2\nProceeding without branch selection.").arg(status).arg(msg));
            accept();
        });
}

void LoginDialog::onLoginClicked()
{
    switch (m_step) {
    case Step::Auth: {
        const QString email    = m_email->text().trimmed();
        const QString password = m_password->text();
        if (email.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, tr("Sign in"), tr("Please enter your email and password."));
            return;
        }
        m_loginBtn->setEnabled(false);
        m_loginBtn->setText(tr("Signing in…"));
        m_api->login(
            email, password,
            [this](const QJsonObject &root) {
                m_loginBtn->setEnabled(true);
                m_token = root.value(QStringLiteral("access_token")).toString();
                m_api->setAccessToken(m_token);
                m_email->setEnabled(false);
                m_password->setEnabled(false);
                // Disconnect Enter shortcut — business/branch steps need explicit clicks
                disconnect(m_password, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);
                loadBusinesses();
            },
            [this](const QString &msg, int) {
                m_loginBtn->setEnabled(true);
                m_loginBtn->setText(tr("Sign in"));
                QMessageBox::warning(this, tr("Sign in"), msg);
            });
        break;
    }
    case Step::SelectBusiness: {
        m_businessId   = m_business->currentData().toInt();
        m_businessName = m_business->currentText();
        if (m_businessId <= 0) {
            QMessageBox::warning(this, tr("Sign in"), tr("Please select a business."));
            return;
        }
        m_api->setBusinessId(m_businessId);
        m_business->setEnabled(false);
        loadBranches();
        break;
    }
    case Step::SelectBranch: {
        m_branchId = m_branchCombo->currentData().toInt();
        accept();
        break;
    }
    }
}

} // namespace pos
