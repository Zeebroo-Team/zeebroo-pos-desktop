#include "ui/MainWindow.h"

#include "core/PosBeep.h"
#include "ui/LayerPickerDialog.h"
#include "ui/ReceiptDialog.h"
#include "core/SaleReceipt.h"
#include "ui/CartLineWidget.h"
#include "ui/ProductCardWidget.h"

#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTextEdit>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

namespace {

QFrame *makeTopBarSeparator(QWidget *parent)
{
    auto *sep = new QFrame(parent);
    sep->setObjectName(QStringLiteral("topBarSep"));
    sep->setFrameShape(QFrame::VLine);
    sep->setFixedWidth(1);
    return sep;
}

QWidget *makeInputGroup(QWidget *parent, const QString &placeholder, QLineEdit **outEdit,
                        const QString &buttonGlyph, const QString &buttonTip, const QObject *receiver,
                        const char *slot)
{
    auto *group = new QFrame(parent);
    group->setObjectName(QStringLiteral("inputGroup"));
    auto *lay = new QHBoxLayout(group);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    auto *edit = new QLineEdit(group);
    edit->setPlaceholderText(placeholder);
    edit->setClearButtonEnabled(true);
    edit->setObjectName(QStringLiteral("topBarInput"));
    *outEdit = edit;
    lay->addWidget(edit, 1);

    auto *btn = new QPushButton(buttonGlyph, group);
    btn->setObjectName(QStringLiteral("inputGroupBtn"));
    btn->setToolTip(buttonTip);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedSize(38, 38);
    QObject::connect(btn, SIGNAL(clicked()), receiver, slot);
    lay->addWidget(btn);

    return group;
}

} // namespace

namespace pos {

MainWindow::MainWindow(ApiClient *api, QWidget *parent)
    : QMainWindow(parent)
    , m_api(api)
{
    setWindowTitle(tr("Zeebroo POS"));
    resize(1280, 800);
    buildUi();
    connect(&m_cart, &Cart::changed, this, &MainWindow::onCartChanged);
    connect(m_api, &ApiClient::unauthorized, this, [this]() {
        QMessageBox::warning(this, tr("Session"), tr("Your session expired. Restart the app to sign in."));
    });
    reloadBootstrap();
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(10);

    // Top bar
    auto *top = new QFrame(central);
    top->setObjectName(QStringLiteral("topBar"));
    top->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    top->setMinimumHeight(56);
    top->setMaximumHeight(56);

    auto *topLay = new QHBoxLayout(top);
    topLay->setContentsMargins(12, 0, 12, 0);
    topLay->setSpacing(12);

    auto *brandBlock = new QFrame(top);
    brandBlock->setObjectName(QStringLiteral("brandBlock"));
    auto *brandLay = new QHBoxLayout(brandBlock);
    brandLay->setContentsMargins(0, 0, 0, 0);
    brandLay->setSpacing(10);
    auto *brandIcon = new QLabel(QStringLiteral("◆"), top);
    brandIcon->setObjectName(QStringLiteral("brandIcon"));
    brandIcon->setAlignment(Qt::AlignCenter);
    m_brandLabel = new QLabel(tr("POS"), top);
    m_brandLabel->setObjectName(QStringLiteral("brandTitle"));
    brandLay->addWidget(brandIcon);
    brandLay->addWidget(m_brandLabel, 1);
    topLay->addWidget(brandBlock);

    auto *statsBlock = new QFrame(top);
    statsBlock->setObjectName(QStringLiteral("statsBlock"));
    auto *statsLay = new QHBoxLayout(statsBlock);
    statsLay->setContentsMargins(0, 0, 0, 0);
    statsLay->setSpacing(6);
    m_statSalesLabel = new QLabel(statsBlock);
    m_statSalesLabel->setObjectName(QStringLiteral("statPill"));
    m_statSalesLabel->setTextFormat(Qt::RichText);
    m_statTotalLabel = new QLabel(statsBlock);
    m_statTotalLabel->setObjectName(QStringLiteral("statPill"));
    m_statTotalLabel->setTextFormat(Qt::RichText);
    statsLay->addWidget(m_statSalesLabel);
    statsLay->addWidget(m_statTotalLabel);
    topLay->addWidget(statsBlock);

    topLay->addWidget(makeTopBarSeparator(top));

    auto *fieldsBlock = new QFrame(top);
    fieldsBlock->setObjectName(QStringLiteral("topFieldsBlock"));
    auto *fieldsLay = new QHBoxLayout(fieldsBlock);
    fieldsLay->setContentsMargins(0, 0, 0, 0);
    fieldsLay->setSpacing(10);

    auto *searchGroup = makeInputGroup(fieldsBlock, tr("Search name…"), &m_searchEdit,
                                      QStringLiteral("⌕"), tr("Search products"), this,
                                      SLOT(onSearchTriggered()));
    searchGroup->setMinimumWidth(220);
    fieldsLay->addWidget(searchGroup, 2);

    auto *skuGroup = makeInputGroup(fieldsBlock, tr("SKU / scan…"), &m_skuEdit,
                                    QStringLiteral("▥"), tr("Add product by SKU"), this,
                                    SLOT(onSkuAdd()));
    skuGroup->setMinimumWidth(180);
    fieldsLay->addWidget(skuGroup, 1);

    topLay->addWidget(fieldsBlock, 1);

    topLay->addWidget(makeTopBarSeparator(top));

    auto *actionsBlock = new QFrame(top);
    actionsBlock->setObjectName(QStringLiteral("topActionsBlock"));
    auto *actionsLay = new QHBoxLayout(actionsBlock);
    actionsLay->setContentsMargins(0, 0, 0, 0);
    actionsLay->setSpacing(6);

    auto *refreshBtn = new QPushButton(QStringLiteral("↻"), top);
    refreshBtn->setObjectName(QStringLiteral("topActionBtn"));
    refreshBtn->setToolTip(tr("Refresh catalog"));
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setFixedSize(36, 36);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::reloadBootstrap);

    auto *clearSearchBtn = new QPushButton(QStringLiteral("✕"), top);
    clearSearchBtn->setObjectName(QStringLiteral("topActionBtn"));
    clearSearchBtn->setToolTip(tr("Clear search & filters"));
    clearSearchBtn->setCursor(Qt::PointingHandCursor);
    clearSearchBtn->setFixedSize(36, 36);
    connect(clearSearchBtn, &QPushButton::clicked, this, [this]() {
        m_searchEdit->clear();
        m_skuEdit->clear();
        m_activeCategoryId = 0;
        reloadBootstrap();
    });

    actionsLay->addWidget(refreshBtn);
    actionsLay->addWidget(clearSearchBtn);
    topLay->addWidget(actionsBlock);

    root->addWidget(top);

    connect(m_searchEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearchTriggered);
    connect(m_skuEdit, &QLineEdit::returnPressed, this, &MainWindow::onSkuAdd);

    // Three panels
    auto *panels = new QHBoxLayout();
    panels->setSpacing(10);

    // Left — current sale
    m_salePanel = new QFrame(central);
    m_salePanel->setObjectName(QStringLiteral("salePanel"));
    m_salePanel->setMinimumWidth(360);
    m_salePanel->setMaximumWidth(440);
    auto *leftLay = new QVBoxLayout(m_salePanel);
    leftLay->setContentsMargins(0, 0, 0, 0);
    leftLay->setSpacing(0);

    auto *saleHead = new QFrame(m_salePanel);
    saleHead->setObjectName(QStringLiteral("salePanelHeader"));
    auto *headLay = new QHBoxLayout(saleHead);
    headLay->setContentsMargins(14, 14, 14, 12);
    headLay->setSpacing(12);

    auto *headTextCol = new QVBoxLayout();
    headTextCol->setSpacing(4);
    auto *saleTitle = new QLabel(tr("Current sale"), saleHead);
    saleTitle->setObjectName(QStringLiteral("salePanelTitle"));
    m_saleItemBadge = new QLabel(tr("0 items"), saleHead);
    m_saleItemBadge->setObjectName(QStringLiteral("saleItemBadge"));
    headTextCol->addWidget(saleTitle);
    headTextCol->addWidget(m_saleItemBadge);
    headLay->addLayout(headTextCol, 1);

    m_saleHeaderSubtotal = new QLabel(money(0), saleHead);
    m_saleHeaderSubtotal->setObjectName(QStringLiteral("saleHeaderSubtotal"));
    m_saleHeaderSubtotal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    headLay->addWidget(m_saleHeaderSubtotal, 0, Qt::AlignVCenter);
    leftLay->addWidget(saleHead);

    m_cartScroll = new QScrollArea(m_salePanel);
    m_cartScroll->setObjectName(QStringLiteral("cartScroll"));
    m_cartScroll->setWidgetResizable(true);
    m_cartScroll->setFrameShape(QFrame::NoFrame);
    m_cartScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_cartContainer = new QWidget(m_cartScroll);
    m_cartContainer->setObjectName(QStringLiteral("cartContainer"));
    m_cartLayout = new QVBoxLayout(m_cartContainer);
    m_cartLayout->setContentsMargins(12, 8, 12, 12);
    m_cartLayout->setSpacing(10);
    m_cartLayout->setAlignment(Qt::AlignTop);
    m_cartEmpty = new QWidget(m_cartContainer);
    m_cartEmpty->setObjectName(QStringLiteral("cartEmpty"));
    auto *emptyLay = new QVBoxLayout(m_cartEmpty);
    emptyLay->setContentsMargins(24, 40, 24, 40);
    emptyLay->setSpacing(8);
    emptyLay->setAlignment(Qt::AlignCenter);
    auto *emptyIcon = new QLabel(QStringLiteral("🛒"), m_cartEmpty);
    emptyIcon->setObjectName(QStringLiteral("cartEmptyIcon"));
    emptyIcon->setAlignment(Qt::AlignCenter);
    auto *emptyText = new QLabel(tr("Your cart is empty"), m_cartEmpty);
    emptyText->setObjectName(QStringLiteral("cartEmptyTitle"));
    emptyText->setAlignment(Qt::AlignCenter);
    auto *emptyHint = new QLabel(tr("Add products from the catalog or scan a SKU."), m_cartEmpty);
    emptyHint->setObjectName(QStringLiteral("cartEmptyHint"));
    emptyHint->setWordWrap(true);
    emptyHint->setAlignment(Qt::AlignCenter);
    emptyLay->addWidget(emptyIcon);
    emptyLay->addWidget(emptyText);
    emptyLay->addWidget(emptyHint);
    m_cartLayout->addWidget(m_cartEmpty);
    m_cartScroll->setWidget(m_cartContainer);
    leftLay->addWidget(m_cartScroll, 1);

    auto *saleFoot = new QFrame(m_salePanel);
    saleFoot->setObjectName(QStringLiteral("salePanelFooter"));
    auto *footLay = new QVBoxLayout(saleFoot);
    footLay->setContentsMargins(14, 12, 14, 14);
    footLay->setSpacing(10);

    auto *summaryRow = new QHBoxLayout();
    m_saleFooterItems = new QLabel(tr("0 items"), saleFoot);
    m_saleFooterItems->setObjectName(QStringLiteral("saleFooterItems"));
    m_saleFooterSubtotal = new QLabel(money(0), saleFoot);
    m_saleFooterSubtotal->setObjectName(QStringLiteral("saleFooterSubtotal"));
    m_saleFooterSubtotal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    summaryRow->addWidget(m_saleFooterItems);
    summaryRow->addStretch();
    summaryRow->addWidget(m_saleFooterSubtotal);
    footLay->addLayout(summaryRow);

    m_clearCartBtn = new QPushButton(tr("Clear sale"), saleFoot);
    m_clearCartBtn->setObjectName(QStringLiteral("saleClearBtn"));
    m_clearCartBtn->setEnabled(false);
    m_clearCartBtn->setCursor(Qt::PointingHandCursor);
    connect(m_clearCartBtn, &QPushButton::clicked, this, &MainWindow::onClearCart);
    footLay->addWidget(m_clearCartBtn);
    leftLay->addWidget(saleFoot);

    panels->addWidget(m_salePanel, 0);

    // Center — catalog
    auto *center = new QFrame(central);
    center->setObjectName(QStringLiteral("panel"));
    auto *centerLay = new QVBoxLayout(center);
    centerLay->setContentsMargins(0, 0, 0, 0);

    m_categoryBar = new QFrame(center);
    m_categoryBar->setObjectName(QStringLiteral("categoryBar"));
    auto *categoryOuter = new QVBoxLayout(m_categoryBar);
    categoryOuter->setContentsMargins(0, 0, 0, 0);
    categoryOuter->setSpacing(0);

    m_categoryScroll = new QScrollArea(m_categoryBar);
    m_categoryScroll->setObjectName(QStringLiteral("categoryScroll"));
    m_categoryScroll->setWidgetResizable(true);
    m_categoryScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_categoryScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_categoryScroll->setFrameShape(QFrame::NoFrame);
    m_categoryScroll->setFixedHeight(52);

    m_categoryStrip = new QWidget(m_categoryScroll);
    m_categoryStrip->setObjectName(QStringLiteral("categoryStrip"));
    m_categoryStrip->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_categoryLayout = new QHBoxLayout(m_categoryStrip);
    m_categoryLayout->setContentsMargins(12, 8, 12, 8);
    m_categoryLayout->setSpacing(8);
    m_categoryLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_categoryScroll->setWidget(m_categoryStrip);
    categoryOuter->addWidget(m_categoryScroll);

    m_categoryGroup = new QButtonGroup(this);
    m_categoryGroup->setExclusive(true);
    connect(m_categoryGroup, &QButtonGroup::idClicked, this, &MainWindow::onCategoryClicked);
    centerLay->addWidget(m_categoryBar);

    m_productScroll = new QScrollArea(center);
    m_productScroll->setWidgetResizable(true);
    m_productScroll->setFrameShape(QFrame::NoFrame);
    m_productContainer = new QWidget(m_productScroll);
    m_productGrid = new QGridLayout(m_productContainer);
    m_productGrid->setSpacing(10);
    m_productGrid->setContentsMargins(12, 12, 12, 12);
    m_productGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    for (int c = 0; c < 6; ++c) {
        m_productGrid->setColumnStretch(c, 1);
    }
    m_productScroll->setWidget(m_productContainer);
    m_productScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    centerLay->addWidget(m_productScroll, 1);

    auto *totals = new QFrame(center);
    totals->setObjectName(QStringLiteral("totalsBar"));
    auto *totalsLay = new QHBoxLayout(totals);
    totalsLay->addWidget(new QLabel(tr("Subtotal"), totals));
    m_subtotalVal = new QLabel(money(0), totals);
    m_subtotalVal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    totalsLay->addWidget(m_subtotalVal, 1);

    m_discountRow = new QWidget(totals);
    auto *discLay = new QHBoxLayout(m_discountRow);
    discLay->setContentsMargins(0, 0, 0, 0);
    discLay->addWidget(new QLabel(tr("Discount (%)"), m_discountRow));
    m_discountSpin = new QDoubleSpinBox(m_discountRow);
    m_discountSpin->setRange(0, 100);
    m_discountSpin->setDecimals(2);
    connect(m_discountSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        m_discountPercent = v;
        updateTotals();
    });
    discLay->addWidget(m_discountSpin);
    discLay->addWidget(new QLabel(tr("Discount amount"), m_discountRow));
    m_discountVal = new QLabel(money(0), m_discountRow);
    discLay->addWidget(m_discountVal);
    totalsLay->addWidget(m_discountRow);

    totalsLay->addWidget(new QLabel(tr("Total"), totals));
    m_totalVal = new QLabel(money(0), totals);
    m_totalVal->setObjectName(QStringLiteral("grandTotal"));
    totalsLay->addWidget(m_totalVal);
    centerLay->addWidget(totals);
    panels->addWidget(center, 1);

    // Right — checkout
    auto *right = new QFrame(central);
    right->setObjectName(QStringLiteral("panel"));
    right->setMinimumWidth(300);
    right->setMaximumWidth(360);
    auto *rightLay = new QVBoxLayout(right);
    rightLay->setContentsMargins(0, 0, 0, 0);
    auto *checkoutHead = new QLabel(tr("Checkout"), right);
    checkoutHead->setObjectName(QStringLiteral("panelHeader"));
    rightLay->addWidget(checkoutHead);

    auto *checkoutBody = new QWidget(right);
    auto *checkoutLay = new QVBoxLayout(checkoutBody);

    checkoutLay->addWidget(new QLabel(tr("PAYMENT"), checkoutBody));
    m_payGroup = new QButtonGroup(this);
    auto *payRow = new QHBoxLayout();
    auto *cashBtn = new QPushButton(tr("Cash"), checkoutBody);
    auto *cardBtn = new QPushButton(tr("Card payment"), checkoutBody);
    auto *creditBtn = new QPushButton(tr("Credit payment"), checkoutBody);
    cashBtn->setCheckable(true);
    cardBtn->setCheckable(true);
    creditBtn->setCheckable(true);
    cashBtn->setChecked(true);
    cashBtn->setObjectName(QStringLiteral("payMethod"));
    cardBtn->setObjectName(QStringLiteral("payMethod"));
    creditBtn->setObjectName(QStringLiteral("payMethod"));
    m_payGroup->addButton(cashBtn, 0);
    m_payGroup->addButton(cardBtn, 1);
    m_payGroup->addButton(creditBtn, 2);
    payRow->addWidget(cashBtn);
    payRow->addWidget(cardBtn);
    payRow->addWidget(creditBtn);
    checkoutLay->addLayout(payRow);
    connect(m_payGroup, &QButtonGroup::idClicked, this, &MainWindow::onPaymentMethodChanged);

    m_cashPanel = new QWidget(checkoutBody);
    auto *cashLay = new QVBoxLayout(m_cashPanel);
    cashLay->addWidget(new QLabel(tr("AMOUNT CUSTOMER GAVE"), m_cashPanel));
    m_tenderedEdit = new QLineEdit(m_cashPanel);
    m_tenderedEdit->setReadOnly(true);
    m_tenderedEdit->setAlignment(Qt::AlignRight);
    m_tenderedEdit->setObjectName(QStringLiteral("tenderedInput"));
    cashLay->addWidget(m_tenderedEdit);

    auto *dueRow = new QHBoxLayout();
    dueRow->addWidget(new QLabel(tr("Amount due"), m_cashPanel));
    m_dueLabel = new QLabel(money(0), m_cashPanel);
    m_dueLabel->setAlignment(Qt::AlignRight);
    dueRow->addWidget(m_dueLabel);
    cashLay->addLayout(dueRow);

    auto *changeRow = new QHBoxLayout();
    changeRow->addWidget(new QLabel(tr("Change / balance"), m_cashPanel));
    m_changeLabel = new QLabel(money(0), m_cashPanel);
    m_changeLabel->setObjectName(QStringLiteral("changeAmount"));
    m_changeLabel->setAlignment(Qt::AlignRight);
    changeRow->addWidget(m_changeLabel);
    cashLay->addLayout(changeRow);

    m_cashHint = new QLabel(m_cashPanel);
    m_cashHint->setObjectName(QStringLiteral("errorHint"));
    m_cashHint->setWordWrap(true);
    m_cashHint->hide();
    cashLay->addWidget(m_cashHint);
    checkoutLay->addWidget(m_cashPanel);

    checkoutLay->addWidget(new QLabel(tr("Notes"), checkoutBody));
    m_notesEdit = new QTextEdit(checkoutBody);
    m_notesEdit->setMaximumHeight(64);
    m_notesEdit->setPlaceholderText(tr("Optional"));
    checkoutLay->addWidget(m_notesEdit);

    // Numpad
    auto *pad = new QGridLayout();
    const QStringList keys = {
        QStringLiteral("1"), QStringLiteral("2"), QStringLiteral("3"),
        QStringLiteral("4"), QStringLiteral("5"), QStringLiteral("6"),
        QStringLiteral("7"), QStringLiteral("8"), QStringLiteral("9"),
        QStringLiteral("."), QStringLiteral("0"), QStringLiteral("⌫"),
    };
    int i = 0;
    for (const QString &k : keys) {
        auto *btn = new QPushButton(k, checkoutBody);
        btn->setObjectName(QStringLiteral("numpadKey"));
        btn->setProperty("numpadKey", k);
        connect(btn, &QPushButton::clicked, this, &MainWindow::onNumpadKey);
        pad->addWidget(btn, i / 3, i % 3);
        ++i;
    }
    checkoutLay->addLayout(pad);

    auto *padActions = new QHBoxLayout();
    auto *exactBtn = new QPushButton(tr("Exact due"), checkoutBody);
    auto *clearTenderBtn = new QPushButton(tr("Clear"), checkoutBody);
    connect(exactBtn, &QPushButton::clicked, this, &MainWindow::onExactDue);
    connect(clearTenderBtn, &QPushButton::clicked, this, &MainWindow::onClearTendered);
    padActions->addWidget(exactBtn);
    padActions->addWidget(clearTenderBtn);
    checkoutLay->addLayout(padActions);

    checkoutLay->addStretch();
    rightLay->addWidget(checkoutBody, 1);

    m_completeBtn = new QPushButton(tr("Complete sale"), right);
    m_completeBtn->setObjectName(QStringLiteral("completeSale"));
    m_completeBtn->setEnabled(false);
    connect(m_completeBtn, &QPushButton::clicked, this, &MainWindow::onCompleteSale);
    rightLay->addWidget(m_completeBtn);
    panels->addWidget(right, 0);

    root->addLayout(panels, 1);
    setCentralWidget(central);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (!m_productScroll || m_bootstrap.products.isEmpty()) {
        return;
    }
    const int viewW = m_productScroll->viewport()->width();
    const int cols = viewW > 120 ? qMax(2, viewW / 158) : 4;
    if (cols != m_lastProductGridCols) {
        rebuildProductGrid();
    }
}

QString MainWindow::money(double value) const
{
    return QString::number(value, 'f', 2) + (m_bootstrap.currency.isEmpty() ? QString() : QStringLiteral(" ") + m_bootstrap.currency);
}

int MainWindow::defaultAccountId() const
{
    if (m_bootstrap.settings.defaultDepositAccountId > 0) {
        return m_bootstrap.settings.defaultDepositAccountId;
    }
    if (!m_bootstrap.accounts.isEmpty()) {
        return m_bootstrap.accounts.first().id;
    }
    return 0;
}

void MainWindow::reloadBootstrap()
{
    m_api->bootstrap(
        m_searchEdit ? m_searchEdit->text() : QString(),
        m_activeCategoryId,
        [this](const QJsonObject &root) {
            applyBootstrap(BootstrapData::fromJson(root.value(QStringLiteral("data")).toObject()));
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Catalog"), msg);
        });
}

void MainWindow::applyBootstrap(const BootstrapData &data)
{
    m_bootstrap = data;
    m_productsById.clear();
    m_productsBySku.clear();
    for (const ProductCard &p : data.products) {
        m_productsById.insert(p.id, p);
        if (!p.sku.isEmpty()) {
            m_productsBySku.insert(p.sku, p);
        }
    }

    m_brandLabel->setText(tr("POS · %1").arg(data.business.name));
    m_brandLabel->setToolTip(data.business.name);
    m_statSalesLabel->setText(
        tr("<span style='font-weight:800;font-size:12px;color:#111827'>%1</span> "
           "<span style='font-size:10px;color:#6b7280;font-weight:600'>sales</span>")
            .arg(data.today.onlineCount));
    m_statTotalLabel->setText(
        tr("<span style='font-weight:800;font-size:12px;color:#111827'>%1</span>")
            .arg(money(data.today.onlineTotal)));

    m_discountRow->setVisible(data.settings.discountFieldEnabled);
    if (!data.settings.discountFieldEnabled) {
        m_discountPercent = 0.0;
        m_discountSpin->setValue(0.0);
    }

    rebuildCategoryBar();

    rebuildProductGrid();
    updateTotals();
}

QPushButton *MainWindow::makeCategoryPill(const QString &text, int id, bool checked)
{
    QString label = text.trimmed();
    if (label.isEmpty()) {
        label = QStringLiteral("—");
    }

    auto *btn = new QPushButton(m_categoryStrip);
    btn->setCheckable(true);
    btn->setChecked(checked);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setObjectName(id == 0 ? QStringLiteral("categoryPillAll") : QStringLiteral("categoryPill"));
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn->setMinimumHeight(36);
    btn->setMaximumHeight(36);

    QFont pillFont = btn->font();
    pillFont.setPointSize(11);
    pillFont.setWeight(QFont::DemiBold);
    btn->setFont(pillFont);

    const int maxTextWidth = 168;
    const QFontMetrics fm(pillFont);
    if (fm.horizontalAdvance(label) > maxTextWidth) {
        btn->setText(fm.elidedText(label, Qt::ElideRight, maxTextWidth));
        btn->setToolTip(label);
    } else {
        btn->setText(label);
        btn->setToolTip(QString());
    }

    const int hPad = 16;
    btn->setMinimumWidth(qMin(fm.horizontalAdvance(btn->text()) + hPad * 2, maxTextWidth + hPad * 2));

    m_categoryGroup->addButton(btn, id);
    m_categoryLayout->addWidget(btn);
    return btn;
}

void MainWindow::rebuildCategoryBar()
{
    while (QLayoutItem *item = m_categoryLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    for (QAbstractButton *b : m_categoryGroup->buttons()) {
        m_categoryGroup->removeButton(b);
    }

    const bool hasCategories = !m_bootstrap.categories.isEmpty();
    m_categoryBar->setVisible(hasCategories);
    if (!hasCategories) {
        return;
    }

    makeCategoryPill(tr("All"), 0, m_activeCategoryId == 0);

    for (const Category &cat : m_bootstrap.categories) {
        makeCategoryPill(cat.name, cat.id, m_activeCategoryId == cat.id);
    }

    m_categoryStrip->adjustSize();
}

void MainWindow::rebuildProductGrid()
{
    while (QLayoutItem *item = m_productGrid->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    if (m_bootstrap.products.isEmpty()) {
        auto *empty = new QLabel(tr("No products found. Try another category or search."), m_productContainer);
        empty->setObjectName(QStringLiteral("catalogEmpty"));
        empty->setWordWrap(true);
        empty->setAlignment(Qt::AlignCenter);
        m_productGrid->addWidget(empty, 0, 0, 1, 4);
        return;
    }

    const int viewW = m_productScroll->viewport()->width();
    const int cols = viewW > 120 ? qMax(2, viewW / 158) : 4;
    m_lastProductGridCols = cols;
    int col = 0;
    int row = 0;
    for (const ProductCard &product : m_bootstrap.products) {
        auto *card = new ProductCardWidget(product, m_bootstrap.currency, &m_imageNam, m_productContainer);
        connect(card, &ProductCardWidget::activated, this, [this](const ProductCard &p) {
            addProductToCart(p);
        });

        m_productGrid->addWidget(card, row, col);
        ++col;
        if (col >= cols) {
            col = 0;
            ++row;
        }
    }
}

void MainWindow::addProductToCart(const ProductCard &product)
{
    if (!product.inStock()) {
        return;
    }

    CartLine line;
    line.productId = product.id;
    line.name = product.name;
    line.sku = product.sku;

    if (product.requiresLayerPick) {
        LayerPickerDialog dlg(product, m_bootstrap.currency, this);
        if (dlg.exec() != QDialog::Accepted) {
            return;
        }
        const StockLayer layer = dlg.selectedLayer();
        if (layer.id <= 0) {
            return;
        }
        line.layerId = layer.id;
        line.layerLabel = layer.label;
        line.unitPrice = layer.unitSellPrice;
    } else if (!product.layers.isEmpty()) {
        line.layerId = product.layers.first().id;
        line.layerLabel = product.layers.first().label;
        line.unitPrice = product.layers.first().unitSellPrice;
    } else {
        line.unitPrice = product.unitSellPrice;
    }

    m_cart.addOrIncrement(line, 1.0);
    PosBeep::play();
}

void MainWindow::updateSalePanelChrome()
{
    const int lineCount = m_cart.lines().size();
    double unitSum = 0.0;
    for (const CartLine &line : m_cart.lines()) {
        unitSum += line.quantity;
    }
    const double sub = m_cart.subtotal();
    const QString subText = money(sub);

    const QString countLabel = lineCount == 1 ? tr("1 item") : tr("%1 items").arg(lineCount);
    const QString qtyLabel = tr("%1 lines · %2 units")
                                 .arg(lineCount)
                                 .arg(unitSum, 0, 'g', 3);

    if (m_saleItemBadge) {
        m_saleItemBadge->setText(lineCount > 0 ? qtyLabel : tr("No items yet"));
    }
    if (m_saleHeaderSubtotal) {
        m_saleHeaderSubtotal->setText(subText);
        m_saleHeaderSubtotal->setVisible(lineCount > 0);
    }
    if (m_saleFooterItems) {
        m_saleFooterItems->setText(countLabel);
    }
    if (m_saleFooterSubtotal) {
        m_saleFooterSubtotal->setText(subText);
    }
    if (m_clearCartBtn) {
        m_clearCartBtn->setEnabled(!m_cart.isEmpty());
    }
}

void MainWindow::rebuildCart()
{
    while (QLayoutItem *item = m_cartLayout->takeAt(0)) {
        if (item->widget() && item->widget() != m_cartEmpty) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    if (m_cart.isEmpty()) {
        m_cartEmpty->show();
        m_cartLayout->addWidget(m_cartEmpty);
        m_cartLayout->addStretch();
        updateSalePanelChrome();
        return;
    }
    m_cartEmpty->hide();

    for (const CartLine &line : m_cart.lines()) {
        const QString unitText = money(line.unitPrice) + tr(" each");
        const QString lineTotalText = money(line.lineTotal());
        auto *row = new CartLineWidget(line, unitText, lineTotalText, m_cartContainer);
        const QString key = line.cartKey();
        connect(row, &CartLineWidget::quantityChanged, this, [this, key](double v) {
            m_cart.setQuantity(key, v);
        });
        connect(row, &CartLineWidget::removeRequested, this, [this, key]() {
            m_cart.removeLine(key);
        });
        m_cartLayout->addWidget(row);
    }
    m_cartLayout->addStretch();
    updateSalePanelChrome();
}

void MainWindow::updateTotals()
{
    const double sub = m_cart.subtotal();
    const double disc = m_cart.discountAmount(m_discountPercent);
    const double total = m_cart.total(m_discountPercent);
    m_subtotalVal->setText(money(sub));
    m_discountVal->setText(money(disc));
    m_totalVal->setText(money(total));
    updatePaymentUi();
}

void MainWindow::updatePaymentUi()
{
    const double total = m_cart.total(m_discountPercent);
    m_dueLabel->setText(money(total));

    const double tendered = m_tenderedEdit->text().toDouble();
    const double change = tendered - total;
    m_changeLabel->setText(money(change));

    bool canComplete = !m_cart.isEmpty();
    if (m_paymentMethod == QStringLiteral("cash")) {
        m_cashPanel->show();
        if (tendered + 0.0001 < total) {
            m_cashHint->setText(tr("Amount received is less than the total due."));
            m_cashHint->show();
            canComplete = false;
        } else {
            m_cashHint->hide();
        }
    } else {
        m_cashPanel->setVisible(m_paymentMethod == QStringLiteral("cash"));
        m_cashHint->hide();
    }

    if (m_paymentMethod != QStringLiteral("cash") && defaultAccountId() <= 0) {
        canComplete = false;
    }

    m_completeBtn->setEnabled(canComplete);
}

void MainWindow::onCartChanged()
{
    rebuildCart();
    updateTotals();
    updateSalePanelChrome();
}

void MainWindow::onCategoryClicked(int id)
{
    m_activeCategoryId = id;
    reloadBootstrap();
}

void MainWindow::onSearchTriggered()
{
    reloadBootstrap();
}

void MainWindow::onSkuAdd()
{
    const QString sku = m_skuEdit->text().trimmed();
    if (sku.isEmpty()) {
        return;
    }
    if (m_productsBySku.contains(sku)) {
        addProductToCart(m_productsBySku.value(sku));
        m_skuEdit->clear();
        return;
    }
    m_api->productBySku(
        sku,
        [this, sku](const QJsonObject &root) {
            const ProductCard p = ProductCard::fromJson(root.value(QStringLiteral("data")).toObject());
            if (p.id > 0) {
                m_productsBySku.insert(sku, p);
                m_productsById.insert(p.id, p);
                addProductToCart(p);
            }
            m_skuEdit->clear();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("SKU"), msg);
        });
}

void MainWindow::onClearCart()
{
    if (m_cart.isEmpty()) {
        return;
    }
    if (QMessageBox::question(this, tr("Clear sale"), tr("Remove all items from the cart?")) == QMessageBox::Yes) {
        m_cart.clear();
        m_tenderedEdit->clear();
    }
}

void MainWindow::onPaymentMethodChanged(int id)
{
    switch (id) {
    case 1:
        m_paymentMethod = QStringLiteral("card");
        break;
    case 2:
        m_paymentMethod = QStringLiteral("credit");
        break;
    default:
        m_paymentMethod = QStringLiteral("cash");
        break;
    }
    m_cashPanel->setVisible(m_paymentMethod == QStringLiteral("cash"));
    updatePaymentUi();
}

void MainWindow::onNumpadKey()
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
    } else {
        val += key;
    }
    m_tenderedEdit->setText(val);
    updatePaymentUi();
}

void MainWindow::onExactDue()
{
    m_tenderedEdit->setText(QString::number(m_cart.total(m_discountPercent), 'f', 2));
    updatePaymentUi();
}

void MainWindow::onClearTendered()
{
    m_tenderedEdit->clear();
    updatePaymentUi();
}

void MainWindow::onCompleteSale()
{
    QJsonArray items;
    for (const CartLine &line : m_cart.lines()) {
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

            m_cart.clear();
            m_tenderedEdit->clear();
            m_notesEdit->clear();
            reloadBootstrap();

            ReceiptDialog receiptDlg(receipt, this);
            receiptDlg.exec();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Checkout"), msg);
            updatePaymentUi();
        });
}

} // namespace pos
