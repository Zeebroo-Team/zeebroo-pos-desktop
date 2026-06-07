#include "ui/CheckoutDialog.h"

#include "core/ApiClient.h"
#include "core/Cart.h"
#include "core/SaleReceipt.h"
#include "ui/ReceiptDialog.h"

#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

namespace pos {

CheckoutDialog::CheckoutDialog(ApiClient *api, Cart *cart, const BootstrapData &bootstrap,
                               double discountPercent, QWidget *parent)
    : QDialog(parent)
    , m_api(api)
    , m_cart(cart)
    , m_bootstrap(bootstrap)
    , m_discountPercent(discountPercent)
{
    setWindowTitle(tr("Checkout"));
    setModal(true);
    setObjectName(QStringLiteral("checkoutDlg"));

    const double total = m_cart->total(m_discountPercent);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Summary strip ─────────────────────────────────────────────
    auto *summary = new QFrame(this);
    summary->setObjectName(QStringLiteral("checkoutSummary"));
    summary->setFixedHeight(80);
    auto *summaryLay = new QHBoxLayout(summary);
    summaryLay->setContentsMargins(32, 0, 32, 0);

    auto *summaryTitleLbl = new QLabel(tr("Total due"), summary);
    summaryTitleLbl->setObjectName(QStringLiteral("checkoutSummaryLabel"));
    m_summaryTotalLbl = new QLabel(money(total), summary);
    m_summaryTotalLbl->setObjectName(QStringLiteral("checkoutSummaryTotal"));
    summaryLay->addWidget(summaryTitleLbl);
    summaryLay->addStretch();
    summaryLay->addWidget(m_summaryTotalLbl);
    root->addWidget(summary);

    auto *divTop = new QWidget(this);
    divTop->setObjectName(QStringLiteral("checkoutDivider"));
    divTop->setFixedHeight(1);
    root->addWidget(divTop);

    // ── Content ───────────────────────────────────────────────────
    auto *content = new QWidget(this);
    auto *contentLay = new QVBoxLayout(content);
    contentLay->setContentsMargins(28, 22, 28, 22);
    contentLay->setSpacing(14);

    // Payment method
    auto *payLabel = new QLabel(tr("PAYMENT METHOD"), content);
    payLabel->setObjectName(QStringLiteral("checkoutSection"));
    contentLay->addWidget(payLabel);

    m_payGroup = new QButtonGroup(this);
    auto *payRow = new QHBoxLayout();
    payRow->setSpacing(8);
    auto *cashBtn   = new QPushButton(tr("Cash"), content);
    auto *cardBtn   = new QPushButton(tr("Card"), content);
    auto *creditBtn = new QPushButton(tr("Credit"), content);
    cashBtn->setCheckable(true);
    cardBtn->setCheckable(true);
    creditBtn->setCheckable(true);
    cashBtn->setChecked(true);
    cashBtn->setObjectName(QStringLiteral("payMethod"));
    cardBtn->setObjectName(QStringLiteral("payMethod"));
    creditBtn->setObjectName(QStringLiteral("payMethod"));
    cashBtn->setToolTip(tr("Cash  [F1]"));
    cardBtn->setToolTip(tr("Card  [F2]"));
    creditBtn->setToolTip(tr("Credit  [F3]"));
    cashBtn->setFixedHeight(44);
    cardBtn->setFixedHeight(44);
    creditBtn->setFixedHeight(44);
    m_payGroup->addButton(cashBtn,   0);
    m_payGroup->addButton(cardBtn,   1);
    m_payGroup->addButton(creditBtn, 2);
    payRow->addWidget(cashBtn);
    payRow->addWidget(cardBtn);
    payRow->addWidget(creditBtn);
    contentLay->addLayout(payRow);
    connect(m_payGroup, &QButtonGroup::idClicked, this, &CheckoutDialog::onPaymentMethodChanged);

    // ── Two-column: cash panel (left) + numpad (right) ────────────
    auto *colsWidget = new QWidget(content);
    auto *colsLay = new QHBoxLayout(colsWidget);
    colsLay->setContentsMargins(0, 0, 0, 0);
    colsLay->setSpacing(24);

    // Left column: discount + cash inputs + notes
    m_cashPanel = new QWidget(colsWidget);
    m_cashPanel->setMinimumWidth(260);
    auto *cashLay = new QVBoxLayout(m_cashPanel);
    cashLay->setContentsMargins(0, 0, 0, 0);
    cashLay->setSpacing(8);

    // ── Discount ─────────────────────────────────
    auto *discountLbl = new QLabel(tr("DISCOUNT %  [D]"), m_cashPanel);
    discountLbl->setObjectName(QStringLiteral("checkoutSection"));
    cashLay->addWidget(discountLbl);

    auto *discountRow = new QHBoxLayout();
    discountRow->setSpacing(6);
    m_discountEdit = new QLineEdit(m_cashPanel);
    m_discountEdit->setObjectName(QStringLiteral("discountInput"));
    m_discountEdit->setAlignment(Qt::AlignRight);
    m_discountEdit->setFixedWidth(86);
    m_discountEdit->setFixedHeight(44);
    m_discountEdit->setPlaceholderText(QStringLiteral("0"));
    m_discountEdit->setToolTip(tr("Discount percentage  [D to focus]"));
    if (m_discountPercent > 0.001) {
        m_discountEdit->setText(QString::number(m_discountPercent, 'f', 2));
    }
    auto *pctSuffix = new QLabel(QStringLiteral("%"), m_cashPanel);
    pctSuffix->setObjectName(QStringLiteral("discountPctLabel"));
    m_discountAmountLbl = new QLabel(m_cashPanel);
    m_discountAmountLbl->setObjectName(QStringLiteral("discountSaving"));
    m_discountAmountLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    discountRow->addWidget(m_discountEdit);
    discountRow->addWidget(pctSuffix);
    discountRow->addSpacing(4);
    discountRow->addWidget(m_discountAmountLbl, 1);
    cashLay->addLayout(discountRow);
    connect(m_discountEdit, &QLineEdit::textChanged, this, &CheckoutDialog::onDiscountChanged);

    // Mini breakdown: subtotal / discount / total
    auto *subtotalRow = new QHBoxLayout();
    subtotalRow->addWidget(new QLabel(tr("Subtotal"), m_cashPanel));
    m_subtotalLbl = new QLabel(money(m_cart->subtotal()), m_cashPanel);
    m_subtotalLbl->setObjectName(QStringLiteral("checkoutMiniValue"));
    m_subtotalLbl->setAlignment(Qt::AlignRight);
    subtotalRow->addWidget(m_subtotalLbl);
    cashLay->addLayout(subtotalRow);

    auto *discAmtRow = new QHBoxLayout();
    auto *discAmtKey = new QLabel(tr("Discount"), m_cashPanel);
    discAmtKey->setObjectName(QStringLiteral("checkoutMiniLabel"));
    m_discountRowLbl = new QLabel(money(0.0), m_cashPanel);
    m_discountRowLbl->setObjectName(QStringLiteral("discountRowValue"));
    m_discountRowLbl->setAlignment(Qt::AlignRight);
    discAmtRow->addWidget(discAmtKey);
    discAmtRow->addWidget(m_discountRowLbl);
    cashLay->addLayout(discAmtRow);

    auto *divDiscount = new QWidget(m_cashPanel);
    divDiscount->setObjectName(QStringLiteral("checkoutDivider"));
    divDiscount->setFixedHeight(1);
    cashLay->addWidget(divDiscount);

    // ── Tendered ─────────────────────────────────
    auto *tenderedLbl = new QLabel(tr("AMOUNT TENDERED"), m_cashPanel);
    tenderedLbl->setObjectName(QStringLiteral("checkoutSection"));
    cashLay->addWidget(tenderedLbl);

    m_tenderedEdit = new QLineEdit(m_cashPanel);
    m_tenderedEdit->setReadOnly(true);
    m_tenderedEdit->setAlignment(Qt::AlignRight);
    m_tenderedEdit->setObjectName(QStringLiteral("tenderedInput"));
    m_tenderedEdit->setFixedHeight(54);
    cashLay->addWidget(m_tenderedEdit);

    auto *dueRow = new QHBoxLayout();
    dueRow->addWidget(new QLabel(tr("Amount due"), m_cashPanel));
    m_dueLabel = new QLabel(money(total), m_cashPanel);
    m_dueLabel->setAlignment(Qt::AlignRight);
    dueRow->addWidget(m_dueLabel);
    cashLay->addLayout(dueRow);

    auto *changeRow = new QHBoxLayout();
    changeRow->addWidget(new QLabel(tr("Change / balance"), m_cashPanel));
    m_changeLabel = new QLabel(money(0.0), m_cashPanel);
    m_changeLabel->setObjectName(QStringLiteral("changeAmount"));
    m_changeLabel->setAlignment(Qt::AlignRight);
    changeRow->addWidget(m_changeLabel);
    cashLay->addLayout(changeRow);

    m_cashHint = new QLabel(m_cashPanel);
    m_cashHint->setObjectName(QStringLiteral("errorHint"));
    m_cashHint->setWordWrap(true);
    m_cashHint->hide();
    cashLay->addWidget(m_cashHint);

    cashLay->addSpacing(10);

    auto *notesLbl = new QLabel(tr("NOTES"), m_cashPanel);
    notesLbl->setObjectName(QStringLiteral("checkoutSection"));
    cashLay->addWidget(notesLbl);

    m_notesEdit = new QTextEdit(m_cashPanel);
    m_notesEdit->setFixedHeight(72);
    m_notesEdit->setPlaceholderText(tr("Optional"));
    cashLay->addWidget(m_notesEdit);
    cashLay->addStretch();

    colsLay->addWidget(m_cashPanel, 1);

    // Right column: numpad
    auto *padWidget = new QWidget(colsWidget);
    auto *padLay = new QVBoxLayout(padWidget);
    padLay->setContentsMargins(0, 0, 0, 0);
    padLay->setSpacing(6);

    auto *pad = new QGridLayout();
    pad->setSpacing(6);
    const QStringList keys = {
        QStringLiteral("1"), QStringLiteral("2"), QStringLiteral("3"),
        QStringLiteral("4"), QStringLiteral("5"), QStringLiteral("6"),
        QStringLiteral("7"), QStringLiteral("8"), QStringLiteral("9"),
        QStringLiteral("."), QStringLiteral("0"), QStringLiteral("⌫"),
    };
    int kIdx = 0;
    for (const QString &k : keys) {
        auto *btn = new QPushButton(k, padWidget);
        btn->setObjectName(QStringLiteral("numpadKey"));
        btn->setProperty("numpadKey", k);
        btn->setFixedSize(72, 56);
        connect(btn, &QPushButton::clicked, this, &CheckoutDialog::onNumpadKey);
        pad->addWidget(btn, kIdx / 3, kIdx % 3);
        ++kIdx;
    }
    padLay->addLayout(pad);

    auto *padActions = new QHBoxLayout();
    padActions->setSpacing(6);
    auto *exactBtn       = new QPushButton(tr("Exact"), padWidget);
    auto *clearTenderBtn = new QPushButton(tr("Clear"), padWidget);
    exactBtn->setObjectName(QStringLiteral("numpadKey"));
    clearTenderBtn->setObjectName(QStringLiteral("numpadKey"));
    exactBtn->setToolTip(tr("Set exact amount  [E]"));
    clearTenderBtn->setToolTip(tr("Clear amount  [Del]"));
    exactBtn->setFixedHeight(48);
    clearTenderBtn->setFixedHeight(48);
    connect(exactBtn,       &QPushButton::clicked, this, &CheckoutDialog::onExactDue);
    connect(clearTenderBtn, &QPushButton::clicked, this, &CheckoutDialog::onClearTendered);
    padActions->addWidget(exactBtn);
    padActions->addWidget(clearTenderBtn);
    padLay->addLayout(padActions);

    colsLay->addWidget(padWidget, 0);
    contentLay->addWidget(colsWidget);
    root->addWidget(content, 1);

    auto *divBot = new QWidget(this);
    divBot->setObjectName(QStringLiteral("checkoutDivider"));
    divBot->setFixedHeight(1);
    root->addWidget(divBot);

    // ── Footer ────────────────────────────────────────────────────
    auto *footer = new QFrame(this);
    footer->setObjectName(QStringLiteral("checkoutFooter"));
    footer->setFixedHeight(76);
    auto *footerLay = new QHBoxLayout(footer);
    footerLay->setContentsMargins(28, 0, 28, 0);
    footerLay->setSpacing(12);

    auto *cancelBtn = new QPushButton(tr("Cancel"), footer);
    cancelBtn->setObjectName(QStringLiteral("checkoutCancelBtn"));
    cancelBtn->setFixedHeight(48);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setToolTip(tr("Cancel  [Esc]"));
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    footerLay->addWidget(cancelBtn);
    footerLay->addStretch();

    m_completeBtn = new QPushButton(tr("Complete Sale  ▶"), footer);
    m_completeBtn->setObjectName(QStringLiteral("completeSale"));
    m_completeBtn->setEnabled(false);
    m_completeBtn->setFixedHeight(48);
    m_completeBtn->setDefault(true);
    m_completeBtn->setCursor(Qt::PointingHandCursor);
    m_completeBtn->setToolTip(tr("Complete sale  [Enter]"));
    connect(m_completeBtn, &QPushButton::clicked, this, &CheckoutDialog::onCompleteSale);
    footerLay->addWidget(m_completeBtn);
    root->addWidget(footer);

    layout()->setSizeConstraint(QLayout::SetFixedSize);
    updatePaymentUi();
    setFocus();
}

// ── Helpers ────────────────────────────────────────────────────────────────

QString CheckoutDialog::money(double value) const
{
    return QString::number(value, 'f', 2)
        + (m_bootstrap.currency.isEmpty() ? QString() : QStringLiteral(" ") + m_bootstrap.currency);
}

int CheckoutDialog::defaultAccountId() const
{
    if (m_bootstrap.settings.defaultDepositAccountId > 0) {
        return m_bootstrap.settings.defaultDepositAccountId;
    }
    if (!m_bootstrap.accounts.isEmpty()) {
        return m_bootstrap.accounts.first().id;
    }
    return 0;
}

// ── Keyboard ───────────────────────────────────────────────────────────────

void CheckoutDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    // Payment method: F1 Cash  F2 Card  F3 Credit
    case Qt::Key_F1:
        if (auto *b = m_payGroup->button(0)) b->click();
        event->accept(); return;
    case Qt::Key_F2:
        if (auto *b = m_payGroup->button(1)) b->click();
        event->accept(); return;
    case Qt::Key_F3:
        if (auto *b = m_payGroup->button(2)) b->click();
        event->accept(); return;

    // D → jump to discount field
    case Qt::Key_D:
        m_discountEdit->setFocus();
        m_discountEdit->selectAll();
        event->accept(); return;

    // Digit / decimal input → tendered amount (cash mode only)
    // Note: when m_discountEdit has focus, digits go directly to it (Qt handles it before
    // reaching this keyPressEvent), so this branch only fires for tendered context.
    case Qt::Key_0: case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4:
    case Qt::Key_5: case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9:
    case Qt::Key_Period: case Qt::Key_Comma: {
        if (m_paymentMethod != QStringLiteral("cash")) break;
        const QString d = (event->key() == Qt::Key_Period || event->key() == Qt::Key_Comma)
                          ? QStringLiteral(".")
                          : QString(QChar(event->key()));
        QString val = m_tenderedEdit->text();
        if (d == QStringLiteral(".") && val.contains(QLatin1Char('.'))) { event->accept(); return; }
        m_tenderedEdit->setText(val + d);
        updatePaymentUi();
        event->accept(); return;
    }
    // Backspace → remove last digit
    case Qt::Key_Backspace:
        if (m_paymentMethod == QStringLiteral("cash")) {
            QString val = m_tenderedEdit->text();
            if (!val.isEmpty()) val.chop(1);
            m_tenderedEdit->setText(val);
            updatePaymentUi();
            event->accept(); return;
        }
        break;
    // Delete → clear tendered
    case Qt::Key_Delete:
        if (m_paymentMethod == QStringLiteral("cash")) {
            m_tenderedEdit->clear();
            updatePaymentUi();
            event->accept(); return;
        }
        break;
    // E → exact amount
    case Qt::Key_E:
        if (m_paymentMethod == QStringLiteral("cash")) {
            onExactDue();
            event->accept(); return;
        }
        break;
    // Enter / Return → complete sale
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_completeBtn->isEnabled()) {
            onCompleteSale();
            event->accept(); return;
        }
        break;
    default:
        break;
    }
    QDialog::keyPressEvent(event);
}

// ── Private methods ────────────────────────────────────────────────────────

void CheckoutDialog::updatePaymentUi()
{
    const double total    = m_cart->total(m_discountPercent);
    const double tendered = m_tenderedEdit->text().toDouble();
    const double change   = tendered > 0 ? tendered - total : 0.0;

    m_summaryTotalLbl->setText(money(total));
    m_dueLabel->setText(money(total));
    m_changeLabel->setText(money(change));

    bool canComplete = !m_cart->isEmpty();

    if (m_paymentMethod == QStringLiteral("cash")) {
        m_cashPanel->show();
        if (tendered > 0 && tendered + 0.0001 < total) {
            m_cashHint->setText(tr("Amount received is less than the total due."));
            m_cashHint->show();
            canComplete = false;
        } else {
            m_cashHint->hide();
        }
        if (tendered < 0.0001) {
            canComplete = false;
        }
    } else {
        m_cashPanel->hide();
        m_cashHint->hide();
        if (defaultAccountId() <= 0) {
            canComplete = false;
        }
    }

    m_completeBtn->setEnabled(canComplete);
}

// ── Slots ──────────────────────────────────────────────────────────────────

void CheckoutDialog::onDiscountChanged()
{
    const double pct = qBound(0.0, m_discountEdit->text().toDouble(), 100.0);
    m_discountPercent = pct;

    const double discAmt = m_cart->discountAmount(pct);
    m_subtotalLbl->setText(money(m_cart->subtotal()));
    m_discountRowLbl->setText(pct > 0.001 ? QStringLiteral("- ") + money(discAmt) : money(0.0));

    if (pct > 0.001) {
        m_discountAmountLbl->setText(tr("Save %1").arg(money(discAmt)));
    } else {
        m_discountAmountLbl->clear();
    }

    updatePaymentUi();
}

void CheckoutDialog::onPaymentMethodChanged(int id)
{
    switch (id) {
    case 1:  m_paymentMethod = QStringLiteral("card");   break;
    case 2:  m_paymentMethod = QStringLiteral("credit"); break;
    default: m_paymentMethod = QStringLiteral("cash");   break;
    }
    updatePaymentUi();
}

void CheckoutDialog::onNumpadKey()
{
    const auto *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) {
        return;
    }
    const QString key = btn->property("numpadKey").toString();
    QString val = m_tenderedEdit->text();
    if (key == QStringLiteral("⌫")) {
        if (!val.isEmpty()) {
            val.chop(1);
        }
    } else if (key == QStringLiteral(".") && val.contains(QLatin1Char('.'))) {
        // prevent duplicate decimal point
    } else {
        val += key;
    }
    m_tenderedEdit->setText(val);
    updatePaymentUi();
}

void CheckoutDialog::onExactDue()
{
    m_tenderedEdit->setText(QString::number(m_cart->total(m_discountPercent), 'f', 2));
    updatePaymentUi();
}

void CheckoutDialog::onClearTendered()
{
    m_tenderedEdit->clear();
    updatePaymentUi();
}

void CheckoutDialog::onCompleteSale()
{
    QJsonArray items;
    for (const CartLine &line : m_cart->lines()) {
        QJsonObject item;
        item.insert(QStringLiteral("product_id"), line.productId);
        item.insert(QStringLiteral("quantity"), line.quantity);
        if (line.layerId > 0) {
            item.insert(QStringLiteral("product_stock_layer_id"), line.layerId);
        }
        items.append(item);
    }

    QJsonObject body;
    body.insert(QStringLiteral("items"), items);
    body.insert(QStringLiteral("payment_method"), m_paymentMethod);
    body.insert(QStringLiteral("channel"), QStringLiteral("online"));
    if (m_discountPercent > 0.0) {
        body.insert(QStringLiteral("discount_percent"), m_discountPercent);
    }
    const QString notes = m_notesEdit->toPlainText().trimmed();
    if (!notes.isEmpty()) {
        body.insert(QStringLiteral("notes"), notes);
    }
    if (m_paymentMethod == QStringLiteral("cash")) {
        body.insert(QStringLiteral("amount_tendered"), m_tenderedEdit->text().toDouble());
        const int accountId = defaultAccountId();
        if (accountId > 0) {
            body.insert(QStringLiteral("credit_account_id"), accountId);
        }
    } else if (m_paymentMethod == QStringLiteral("card")) {
        const int accountId = defaultAccountId();
        if (accountId > 0) {
            body.insert(QStringLiteral("credit_account_id"), accountId);
        }
    }

    m_completeBtn->setEnabled(false);
    m_api->checkout(
        body,
        [this](const QJsonObject &root) {
            const QJsonObject sale = root.value(QStringLiteral("data")).toObject();
            const SaleReceipt receipt = SaleReceipt::fromApiSale(
                sale,
                m_bootstrap.business.name,
                m_bootstrap.currency);

            m_cart->clear();
            m_notesEdit->clear();

            ReceiptDialog receiptDlg(receipt, this);
            receiptDlg.exec();

            accept();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Checkout"), msg);
            updatePaymentUi();
        });
}

} // namespace pos
