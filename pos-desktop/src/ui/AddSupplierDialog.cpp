#include "ui/AddSupplierDialog.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace pos {

AddSupplierDialog::AddSupplierDialog(ApiClient *api, QWidget *parent)
    : QDialog(parent)
    , m_api(api)
{
    setWindowTitle(tr("Add Supplier"));
    setMinimumWidth(420);
    setObjectName(QStringLiteral("addSupplierDialog"));
    buildUi();
}

void AddSupplierDialog::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Header ────────────────────────────────────────────────────────────────
    auto *header = new QWidget(this);
    header->setObjectName(QStringLiteral("poGradientHeader"));
    auto *headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(20, 16, 20, 16);
    headerLayout->setSpacing(3);
    auto *titleLbl = new QLabel(tr("Add Supplier"), header);
    titleLbl->setObjectName(QStringLiteral("poHeaderTitle"));
    auto *subLbl = new QLabel(tr("New supplier will be available immediately in the dropdown"), header);
    subLbl->setObjectName(QStringLiteral("poHeaderSubtitle"));
    headerLayout->addWidget(titleLbl);
    headerLayout->addWidget(subLbl);
    root->addWidget(header);

    // ── Form card ─────────────────────────────────────────────────────────────
    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("poDetailCard"));
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(20, 20, 20, 20);
    cardLayout->setSpacing(14);

    auto makeRow = [&](const QString &label, QLineEdit *&field, const QString &placeholder, bool required = false) {
        auto *col = new QVBoxLayout;
        col->setSpacing(5);
        auto *lbl = new QLabel(label + (required ? QStringLiteral(" *") : QString()), card);
        lbl->setObjectName(QStringLiteral("poFormLabel"));
        field = new QLineEdit(card);
        field->setObjectName(QStringLiteral("supplierFormField"));
        field->setPlaceholderText(placeholder);
        field->setFixedHeight(32);
        col->addWidget(lbl);
        col->addWidget(field);
        cardLayout->addLayout(col);
    };

    makeRow(tr("Supplier Name"), m_nameEdit, tr("e.g. Acme Wholesale Ltd."), true);
    makeRow(tr("Contact Name"),  m_contactEdit, tr("e.g. John Smith"));
    makeRow(tr("Email"),         m_emailEdit, tr("e.g. orders@acme.com"));
    makeRow(tr("Phone"),         m_phoneEdit, tr("e.g. +1 555 000 1234"));

    auto *outerPad = new QWidget(this);
    auto *outerLayout = new QVBoxLayout(outerPad);
    outerLayout->setContentsMargins(16, 16, 16, 0);
    outerLayout->addWidget(card);
    root->addWidget(outerPad);

    root->addStretch(1);

    // ── Bottom bar ────────────────────────────────────────────────────────────
    auto *divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName(QStringLiteral("poDivider"));
    root->addWidget(divider);

    auto *bar = new QWidget(this);
    bar->setObjectName(QStringLiteral("poBottomBar"));
    auto *barLayout = new QHBoxLayout(bar);
    barLayout->setContentsMargins(20, 12, 20, 12);
    barLayout->setSpacing(10);

    auto *cancelBtn = new QPushButton(tr("Cancel"), bar);
    cancelBtn->setObjectName(QStringLiteral("poCloseBtn"));
    cancelBtn->setFixedHeight(32);
    cancelBtn->setMinimumWidth(80);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_saveBtn = new QPushButton(tr("Add Supplier"), bar);
    m_saveBtn->setObjectName(QStringLiteral("poPlaceBtn"));
    m_saveBtn->setFixedHeight(32);
    m_saveBtn->setMinimumWidth(110);
    m_saveBtn->setDefault(true);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    connect(m_saveBtn, &QPushButton::clicked, this, &AddSupplierDialog::onSave);

    barLayout->addWidget(cancelBtn);
    barLayout->addStretch();
    barLayout->addWidget(m_saveBtn);
    root->addWidget(bar);
}

void AddSupplierDialog::onSave()
{
    const QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::information(this, tr("Required"), tr("Supplier name is required."));
        m_nameEdit->setFocus();
        return;
    }

    m_saveBtn->setEnabled(false);
    m_saveBtn->setText(tr("Saving…"));

    QJsonObject body;
    body.insert(QStringLiteral("name"), name);
    const QString contact = m_contactEdit->text().trimmed();
    const QString email   = m_emailEdit->text().trimmed();
    const QString phone   = m_phoneEdit->text().trimmed();
    if (!contact.isEmpty()) body.insert(QStringLiteral("contact_name"), contact);
    if (!email.isEmpty())   body.insert(QStringLiteral("email"), email);
    if (!phone.isEmpty())   body.insert(QStringLiteral("phone"), phone);

    m_api->createSupplier(body,
        [this](const QJsonObject &root) {
            m_created = Supplier::fromJson(root.value(QStringLiteral("data")).toObject());
            accept();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            m_saveBtn->setEnabled(true);
            m_saveBtn->setText(tr("Add Supplier"));
        });
}

} // namespace pos
