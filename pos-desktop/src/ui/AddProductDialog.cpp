#include "ui/AddProductDialog.h"

#include "core/ApiClient.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace pos {

AddProductDialog::AddProductDialog(ApiClient *api,
                                   const QString &currency,
                                   bool branchProductSeparate,
                                   const QVector<Branch> &branches,
                                   int selectedBranchId,
                                   QWidget *parent)
    : QDialog(parent)
    , m_api(api)
    , m_currency(currency)
{
    setWindowTitle(tr("Add Product"));
    setModal(true);
    setObjectName(QStringLiteral("addProductDlg"));

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Dialog header ─────────────────────────────────────────────
    auto *header = new QWidget(this);
    header->setObjectName(QStringLiteral("addProductHeader"));
    auto *headerLay = new QVBoxLayout(header);
    headerLay->setContentsMargins(24, 20, 24, 18);
    headerLay->setSpacing(4);

    auto *titleLbl = new QLabel(tr("Add New Product"), header);
    titleLbl->setObjectName(QStringLiteral("addProductTitle"));
    auto *subLbl = new QLabel(tr("Quick-add a product to your catalog"), header);
    subLbl->setObjectName(QStringLiteral("addProductSub"));
    headerLay->addWidget(titleLbl);
    headerLay->addWidget(subLbl);
    root->addWidget(header);

    auto *divTop = new QWidget(this);
    divTop->setObjectName(QStringLiteral("checkoutDivider"));
    divTop->setFixedHeight(1);
    root->addWidget(divTop);

    // ── Form ──────────────────────────────────────────────────────
    auto *content = new QWidget(this);
    auto *formLay = new QVBoxLayout(content);
    formLay->setContentsMargins(24, 20, 24, 16);
    formLay->setSpacing(6);

    // Product name
    auto *nameLbl = new QLabel(tr("PRODUCT NAME"), content);
    nameLbl->setObjectName(QStringLiteral("checkoutSection"));
    formLay->addWidget(nameLbl);

    m_nameEdit = new QLineEdit(content);
    m_nameEdit->setFixedHeight(40);
    m_nameEdit->setPlaceholderText(tr("e.g. Coca-Cola 500ml"));
    m_nameEdit->setMaxLength(255);
    formLay->addWidget(m_nameEdit);
    formLay->addSpacing(10);

    // SKU / barcode
    auto *skuLbl = new QLabel(tr("SKU / BARCODE"), content);
    skuLbl->setObjectName(QStringLiteral("checkoutSection"));
    formLay->addWidget(skuLbl);

    m_skuEdit = new QLineEdit(content);
    m_skuEdit->setFixedHeight(40);
    m_skuEdit->setPlaceholderText(tr("Optional — scan or type"));
    m_skuEdit->setMaxLength(120);
    formLay->addWidget(m_skuEdit);
    formLay->addSpacing(10);

    // Selling price + Opening stock (side-by-side)
    auto *twoCol = new QHBoxLayout();
    twoCol->setSpacing(14);

    auto *priceCol = new QVBoxLayout();
    priceCol->setSpacing(6);
    auto *priceLbl = new QLabel(tr("SELLING PRICE  *"), content);
    priceLbl->setObjectName(QStringLiteral("checkoutSection"));
    priceCol->addWidget(priceLbl);
    m_priceEdit = new QDoubleSpinBox(content);
    m_priceEdit->setRange(0.0, 9'999'999.0);
    m_priceEdit->setDecimals(2);
    m_priceEdit->setValue(0.0);
    m_priceEdit->setFixedHeight(40);
    m_priceEdit->setAlignment(Qt::AlignRight);
    if (!currency.isEmpty()) {
        m_priceEdit->setSuffix(QStringLiteral("  ") + currency);
    }
    priceCol->addWidget(m_priceEdit);
    twoCol->addLayout(priceCol);

    auto *stockCol = new QVBoxLayout();
    stockCol->setSpacing(6);
    auto *stockLbl = new QLabel(tr("OPENING STOCK"), content);
    stockLbl->setObjectName(QStringLiteral("checkoutSection"));
    stockCol->addWidget(stockLbl);
    m_stockEdit = new QDoubleSpinBox(content);
    m_stockEdit->setRange(0.0, 9'999'999.0);
    m_stockEdit->setDecimals(0);
    m_stockEdit->setValue(0.0);
    m_stockEdit->setFixedHeight(40);
    m_stockEdit->setAlignment(Qt::AlignRight);
    stockCol->addWidget(m_stockEdit);
    twoCol->addLayout(stockCol);

    formLay->addLayout(twoCol);

    // ── Branch dropdown (shown whenever the business has branches) ─
    if (!branches.isEmpty()) {
        formLay->addSpacing(10);

        auto *branchLbl = new QLabel(tr("BRANCH"), content);
        branchLbl->setObjectName(QStringLiteral("checkoutSection"));
        formLay->addWidget(branchLbl);

        m_branchCombo = new QComboBox(content);
        m_branchCombo->setFixedHeight(40);
        m_branchCombo->setObjectName(QStringLiteral("addProductBranchCombo"));

        int selectIdx = 0;
        for (const Branch &b : branches) {
            m_branchCombo->addItem(b.name, b.id);
            if (b.id == selectedBranchId) {
                selectIdx = m_branchCombo->count() - 1;
            }
        }
        m_branchCombo->setCurrentIndex(selectIdx);
        formLay->addWidget(m_branchCombo);
    }

    // Error message (hidden until needed)
    formLay->addSpacing(8);
    m_errorLbl = new QLabel(content);
    m_errorLbl->setObjectName(QStringLiteral("addProductError"));
    m_errorLbl->setWordWrap(true);
    m_errorLbl->hide();
    formLay->addWidget(m_errorLbl);

    root->addWidget(content, 1);

    auto *divBot = new QWidget(this);
    divBot->setObjectName(QStringLiteral("checkoutDivider"));
    divBot->setFixedHeight(1);
    root->addWidget(divBot);

    // ── Footer ────────────────────────────────────────────────────
    auto *footer = new QFrame(this);
    footer->setObjectName(QStringLiteral("checkoutFooter"));
    footer->setFixedHeight(64);
    auto *footerLay = new QHBoxLayout(footer);
    footerLay->setContentsMargins(24, 0, 24, 0);
    footerLay->setSpacing(10);

    auto *cancelBtn = new QPushButton(tr("Cancel"), footer);
    cancelBtn->setObjectName(QStringLiteral("checkoutCancelBtn"));
    cancelBtn->setFixedHeight(40);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    footerLay->addWidget(cancelBtn);
    footerLay->addStretch();

    m_submitBtn = new QPushButton(tr("Add Product  +"), footer);
    m_submitBtn->setObjectName(QStringLiteral("addProductBtn"));
    m_submitBtn->setFixedHeight(40);
    m_submitBtn->setCursor(Qt::PointingHandCursor);
    connect(m_submitBtn, &QPushButton::clicked, this, &AddProductDialog::onSubmit);
    footerLay->addWidget(m_submitBtn);
    root->addWidget(footer);

    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setMinimumWidth(440);

    m_nameEdit->setFocus();
    connect(m_nameEdit, &QLineEdit::returnPressed, m_submitBtn, &QPushButton::click);
}

void AddProductDialog::onSubmit()
{
    const QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        m_errorLbl->setText(tr("Product name is required."));
        m_errorLbl->show();
        m_nameEdit->setFocus();
        return;
    }

    m_errorLbl->hide();
    m_submitBtn->setEnabled(false);
    m_submitBtn->setText(tr("Adding…"));

    QJsonObject body;
    body.insert(QStringLiteral("name"), name);
    const QString sku = m_skuEdit->text().trimmed();
    if (!sku.isEmpty()) {
        body.insert(QStringLiteral("sku"), sku);
    }
    body.insert(QStringLiteral("unit_price"), m_priceEdit->value());
    if (m_stockEdit->value() > 0.0) {
        body.insert(QStringLiteral("stock_quantity"), m_stockEdit->value());
    }

    // Use explicit branch combo selection if shown, otherwise fall back to login branch
    if (m_branchCombo) {
        const int branchId = m_branchCombo->currentData().toInt();
        if (branchId > 0) {
            body.insert(QStringLiteral("branch_id"), branchId);
        }
    } else if (m_api->branchId() > 0) {
        body.insert(QStringLiteral("branch_id"), m_api->branchId());
    }

    m_api->createProduct(
        body,
        [this](const QJsonObject &root) {
            m_created = ProductCard::fromJson(root.value(QStringLiteral("data")).toObject());
            accept();
        },
        [this](const QString &msg, int) {
            m_errorLbl->setText(msg);
            m_errorLbl->show();
            m_submitBtn->setEnabled(true);
            m_submitBtn->setText(tr("Add Product  +"));
        });
}

} // namespace pos
