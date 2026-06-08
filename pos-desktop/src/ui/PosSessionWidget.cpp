#include "ui/PosSessionWidget.h"

#include "core/PosBeep.h"
#include "ui/AddProductDialog.h"
#include "ui/CheckoutDialog.h"
#include "ui/LayerPickerDialog.h"
#include "ui/ReceiptDialog.h"
#include "core/SaleReceipt.h"
#include "ui/CartLineWidget.h"
#include "ui/ProductCardWidget.h"
#include "ui/ReturnDialog.h"

#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSizePolicy>
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

PosSessionWidget::PosSessionWidget(ApiClient *api, int sessionNumber, QWidget *parent)
    : QWidget(parent)
    , m_api(api)
    , m_sessionNumber(sessionNumber)
{
    setObjectName(QStringLiteral("posSessionWidget"));
    buildUi();
    connect(&m_cart, &Cart::changed, this, &PosSessionWidget::onCartChanged);
    reloadBootstrap();
}

void PosSessionWidget::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    // ── Top bar ───────────────────────────────────────────────────────────────
    auto *top = new QFrame(this);
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
                                      QStringLiteral("⌕"), tr("Search products  [F2]"), this,
                                      SLOT(onSearchTriggered()));
    searchGroup->setMinimumWidth(220);
    fieldsLay->addWidget(searchGroup, 2);

    auto *skuGroup = makeInputGroup(fieldsBlock, tr("SKU / scan…"), &m_skuEdit,
                                    QStringLiteral("▥"), tr("Add product by SKU  [F3]"), this,
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

    auto *addProductBtn = new QPushButton(QStringLiteral("+"), top);
    addProductBtn->setObjectName(QStringLiteral("addProductActionBtn"));
    addProductBtn->setToolTip(tr("Add new product  [F4]"));
    addProductBtn->setCursor(Qt::PointingHandCursor);
    addProductBtn->setFixedSize(36, 36);
    connect(addProductBtn, &QPushButton::clicked, this, &PosSessionWidget::onAddProduct);

    auto *refreshBtn = new QPushButton(QStringLiteral("↻"), top);
    refreshBtn->setObjectName(QStringLiteral("topActionBtn"));
    refreshBtn->setToolTip(tr("Refresh catalog  [F5]"));
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setFixedSize(36, 36);
    connect(refreshBtn, &QPushButton::clicked, this, &PosSessionWidget::reloadBootstrap);

    auto *clearSearchBtn = new QPushButton(QStringLiteral("✕"), top);
    clearSearchBtn->setObjectName(QStringLiteral("topActionBtn"));
    clearSearchBtn->setToolTip(tr("Clear search & filters  [F6]"));
    clearSearchBtn->setCursor(Qt::PointingHandCursor);
    clearSearchBtn->setFixedSize(36, 36);
    connect(clearSearchBtn, &QPushButton::clicked, this, [this]() {
        m_searchEdit->clear();
        m_skuEdit->clear();
        m_activeCategoryId = 0;
        m_currentPage = 1;
        reloadBootstrap();
    });

    actionsLay->addWidget(addProductBtn);
    actionsLay->addWidget(refreshBtn);
    actionsLay->addWidget(clearSearchBtn);
    topLay->addWidget(actionsBlock);

    root->addWidget(top);

    connect(m_searchEdit, &QLineEdit::returnPressed, this, &PosSessionWidget::onSearchTriggered);
    connect(m_skuEdit, &QLineEdit::returnPressed, this, &PosSessionWidget::onSkuAdd);

    // ── Two panels ────────────────────────────────────────────────────────────
    auto *panels = new QHBoxLayout();
    panels->setSpacing(10);

    // Left — current sale
    m_salePanel = new QFrame(this);
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

    auto *secondaryRow = new QHBoxLayout();
    secondaryRow->setSpacing(6);

    m_clearCartBtn = new QPushButton(tr("Clear sale"), saleFoot);
    m_clearCartBtn->setObjectName(QStringLiteral("saleClearBtn"));
    m_clearCartBtn->setEnabled(false);
    m_clearCartBtn->setCursor(Qt::PointingHandCursor);
    m_clearCartBtn->setToolTip(tr("Clear all items from cart  [F8]"));
    connect(m_clearCartBtn, &QPushButton::clicked, this, &PosSessionWidget::onClearCart);
    secondaryRow->addWidget(m_clearCartBtn, 1);

    m_returnBtn = new QPushButton(tr("Return  ↩"), saleFoot);
    m_returnBtn->setObjectName(QStringLiteral("saleReturnBtn"));
    m_returnBtn->setCursor(Qt::PointingHandCursor);
    m_returnBtn->setToolTip(tr("Process a return / refund  [F9]"));
    connect(m_returnBtn, &QPushButton::clicked, this, &PosSessionWidget::onReturn);
    secondaryRow->addWidget(m_returnBtn, 1);

    footLay->addLayout(secondaryRow);

    m_checkoutBtn = new QPushButton(tr("Checkout  ▶"), saleFoot);
    m_checkoutBtn->setObjectName(QStringLiteral("checkoutBtn"));
    m_checkoutBtn->setEnabled(false);
    m_checkoutBtn->setFixedHeight(44);
    m_checkoutBtn->setCursor(Qt::PointingHandCursor);
    m_checkoutBtn->setToolTip(tr("Proceed to checkout  [F12]"));
    connect(m_checkoutBtn, &QPushButton::clicked, this, &PosSessionWidget::onCheckout);
    footLay->addWidget(m_checkoutBtn);

    leftLay->addWidget(saleFoot);
    panels->addWidget(m_salePanel, 0);

    // Center — catalog
    auto *center = new QFrame(this);
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
    connect(m_categoryGroup, &QButtonGroup::idClicked, this, &PosSessionWidget::onCategoryClicked);
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

    // ── Pagination bar ────────────────────────────────────────────────────────
    m_paginationBar = new QWidget(center);
    m_paginationBar->setObjectName(QStringLiteral("paginationBar"));
    m_paginationBar->setFixedHeight(52);
    auto *pagLay = new QHBoxLayout(m_paginationBar);
    pagLay->setContentsMargins(16, 0, 16, 0);
    pagLay->setSpacing(0);

    // Left — total products chip
    m_pageLabel = new QLabel(m_paginationBar);
    m_pageLabel->setObjectName(QStringLiteral("pageTotalChip"));
    m_pageLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    // Center — nav controls
    auto *navBlock = new QWidget(m_paginationBar);
    navBlock->setObjectName(QStringLiteral("pageNavBlock"));
    auto *navLay = new QHBoxLayout(navBlock);
    navLay->setContentsMargins(0, 0, 0, 0);
    navLay->setSpacing(12);

    m_prevPageBtn = new QPushButton(QStringLiteral("‹"), m_paginationBar);
    m_prevPageBtn->setObjectName(QStringLiteral("pageNavBtn"));
    m_prevPageBtn->setCursor(Qt::PointingHandCursor);
    m_prevPageBtn->setFixedSize(36, 36);
    m_prevPageBtn->setToolTip(tr("Previous page"));
    connect(m_prevPageBtn, &QPushButton::clicked, this, &PosSessionWidget::onPrevPage);

    auto *pageInfoLabel = new QLabel(m_paginationBar);
    pageInfoLabel->setObjectName(QStringLiteral("pageInfoLabel"));
    pageInfoLabel->setAlignment(Qt::AlignCenter);
    pageInfoLabel->setMinimumWidth(110);
    // We'll store it so updatePaginationBar can update it
    // Reuse m_pageLabel slot — rename meaning: m_pageLabel = total chip, pageInfoLabel = center
    // Actually store pageInfoLabel via the navBlock's first label child (found by objectName)
    navLay->addWidget(m_prevPageBtn);
    navLay->addWidget(pageInfoLabel);

    m_nextPageBtn = new QPushButton(QStringLiteral("›"), m_paginationBar);
    m_nextPageBtn->setObjectName(QStringLiteral("pageNavBtn"));
    m_nextPageBtn->setCursor(Qt::PointingHandCursor);
    m_nextPageBtn->setFixedSize(36, 36);
    m_nextPageBtn->setToolTip(tr("Next page"));
    connect(m_nextPageBtn, &QPushButton::clicked, this, &PosSessionWidget::onNextPage);

    navLay->addWidget(m_nextPageBtn);

    pagLay->addWidget(m_pageLabel, 1);
    pagLay->addWidget(navBlock, 0, Qt::AlignCenter);
    pagLay->addStretch(1);

    m_paginationBar->setVisible(false);
    centerLay->addWidget(m_paginationBar);

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

    root->addLayout(panels, 1);
}

// ── Public interface (keyboard delegates from MainWindow) ─────────────────────

void PosSessionWidget::focusSearch()
{
    if (m_searchEdit) { m_searchEdit->setFocus(); m_searchEdit->selectAll(); }
}

void PosSessionWidget::focusSku()
{
    if (m_skuEdit) { m_skuEdit->setFocus(); m_skuEdit->selectAll(); }
}

void PosSessionWidget::triggerAddProduct()    { onAddProduct(); }
void PosSessionWidget::triggerRefresh()       { reloadBootstrap(); }

void PosSessionWidget::triggerClearFilters()
{
    if (m_searchEdit) m_searchEdit->clear();
    if (m_skuEdit)    m_skuEdit->clear();
    m_activeCategoryId = 0;
    m_currentPage = 1;
    reloadBootstrap();
}

void PosSessionWidget::triggerClearCart()  { onClearCart(); }
void PosSessionWidget::triggerReturn()     { onReturn(); }
void PosSessionWidget::triggerCheckout()   { onCheckout(); }

void PosSessionWidget::navigatePreviousCategory()
{
    const auto btns = m_categoryGroup->buttons();
    if (btns.size() < 2) return;
    int cur = 0;
    for (int i = 0; i < btns.size(); ++i) {
        if (btns[i]->isChecked()) { cur = i; break; }
    }
    btns[(cur - 1 + btns.size()) % btns.size()]->click();
}

void PosSessionWidget::navigateNextCategory()
{
    const auto btns = m_categoryGroup->buttons();
    if (btns.size() < 2) return;
    int cur = 0;
    for (int i = 0; i < btns.size(); ++i) {
        if (btns[i]->isChecked()) { cur = i; break; }
    }
    btns[(cur + 1) % btns.size()]->click();
}

// ── resizeEvent ───────────────────────────────────────────────────────────────

void PosSessionWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_productScroll || m_bootstrap.products.isEmpty()) return;
    const int viewW = m_productScroll->viewport()->width();
    const int cols = viewW > 120 ? qMax(2, viewW / 158) : 4;
    if (cols != m_lastProductGridCols) {
        rebuildProductGrid();
    }
}

// ── Private helpers ───────────────────────────────────────────────────────────

QString PosSessionWidget::money(double value) const
{
    return QString::number(value, 'f', 2)
           + (m_bootstrap.currency.isEmpty() ? QString() : QStringLiteral(" ") + m_bootstrap.currency);
}

void PosSessionWidget::reloadBootstrap()
{
    m_api->bootstrap(
        m_searchEdit ? m_searchEdit->text() : QString(),
        m_activeCategoryId,
        m_currentPage,
        [this](const QJsonObject &root) {
            applyBootstrap(BootstrapData::fromJson(root.value(QStringLiteral("data")).toObject()));
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Catalog"), msg);
        });
}

void PosSessionWidget::applyBootstrap(const BootstrapData &data)
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
    updatePaginationBar();
    updateTotals();
}

QPushButton *PosSessionWidget::makeCategoryPill(const QString &text, int id, bool checked)
{
    QString label = text.trimmed();
    if (label.isEmpty()) label = QStringLiteral("—");

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

void PosSessionWidget::rebuildCategoryBar()
{
    while (QLayoutItem *item = m_categoryLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    for (QAbstractButton *b : m_categoryGroup->buttons()) {
        m_categoryGroup->removeButton(b);
    }

    const bool hasCategories = !m_bootstrap.categories.isEmpty();
    m_categoryBar->setVisible(hasCategories);
    if (!hasCategories) return;

    makeCategoryPill(tr("All"), 0, m_activeCategoryId == 0);
    for (const Category &cat : m_bootstrap.categories) {
        makeCategoryPill(cat.name, cat.id, m_activeCategoryId == cat.id);
    }
    m_categoryStrip->adjustSize();
}

void PosSessionWidget::rebuildProductGrid()
{
    while (QLayoutItem *item = m_productGrid->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
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
    int col = 0, row = 0;
    for (const ProductCard &product : m_bootstrap.products) {
        auto *card = new ProductCardWidget(product, m_bootstrap.currency, &m_imageNam, m_productContainer);
        connect(card, &ProductCardWidget::activated, this, [this](const ProductCard &p) {
            addProductToCart(p);
        });
        m_productGrid->addWidget(card, row, col);
        ++col;
        if (col >= cols) { col = 0; ++row; }
    }
}

void PosSessionWidget::addProductToCart(const ProductCard &product)
{
    if (!product.inStock()) return;

    CartLine line;
    line.productId = product.id;
    line.name      = product.name;
    line.sku       = product.sku;

    if (product.requiresLayerPick) {
        LayerPickerDialog dlg(product, m_bootstrap.currency, this);
        if (dlg.exec() != QDialog::Accepted) return;
        const StockLayer layer = dlg.selectedLayer();
        if (layer.id <= 0) return;
        line.layerId    = layer.id;
        line.layerLabel = layer.label;
        line.unitPrice  = layer.unitSellPrice;
    } else if (!product.layers.isEmpty()) {
        line.layerId    = product.layers.first().id;
        line.layerLabel = product.layers.first().label;
        line.unitPrice  = product.layers.first().unitSellPrice;
    } else {
        line.unitPrice = product.unitSellPrice;
    }

    m_cart.addOrIncrement(line, 1.0);
    PosBeep::play();
}

void PosSessionWidget::updateSalePanelChrome()
{
    const int lineCount = m_cart.lines().size();
    double unitSum = 0.0;
    for (const CartLine &line : m_cart.lines()) unitSum += line.quantity;
    const double sub = m_cart.subtotal();

    if (m_saleItemBadge)
        m_saleItemBadge->setText(lineCount > 0
            ? tr("%1 lines · %2 units").arg(lineCount).arg(unitSum, 0, 'g', 3)
            : tr("No items yet"));
    if (m_saleHeaderSubtotal) {
        m_saleHeaderSubtotal->setText(money(sub));
        m_saleHeaderSubtotal->setVisible(lineCount > 0);
    }
    if (m_saleFooterItems)
        m_saleFooterItems->setText(lineCount == 1 ? tr("1 item") : tr("%1 items").arg(lineCount));
    if (m_saleFooterSubtotal)
        m_saleFooterSubtotal->setText(money(sub));
    if (m_clearCartBtn)
        m_clearCartBtn->setEnabled(!m_cart.isEmpty());
    if (m_checkoutBtn)
        m_checkoutBtn->setEnabled(!m_cart.isEmpty());
}

void PosSessionWidget::rebuildCart()
{
    while (QLayoutItem *item = m_cartLayout->takeAt(0)) {
        if (item->widget() && item->widget() != m_cartEmpty)
            item->widget()->deleteLater();
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
        auto *row = new CartLineWidget(line, money(line.unitPrice) + tr(" each"),
                                       money(line.lineTotal()), m_cartContainer);
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

void PosSessionWidget::updateTotals()
{
    m_subtotalVal->setText(money(m_cart.subtotal()));
    m_discountVal->setText(money(m_cart.discountAmount(m_discountPercent)));
    m_totalVal->setText(money(m_cart.total(m_discountPercent)));
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void PosSessionWidget::onCartChanged()
{
    rebuildCart();
    updateTotals();
    updateSalePanelChrome();
    emit sessionCartChanged();
}

void PosSessionWidget::onCategoryClicked(int id)
{
    m_activeCategoryId = id;
    m_currentPage = 1;
    reloadBootstrap();
}

void PosSessionWidget::onSearchTriggered()
{
    m_currentPage = 1;
    reloadBootstrap();
}

void PosSessionWidget::onSkuAdd()
{
    const QString sku = m_skuEdit->text().trimmed();
    if (sku.isEmpty()) return;
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

void PosSessionWidget::onClearCart()
{
    if (m_cart.isEmpty()) return;
    if (QMessageBox::question(this, tr("Clear sale"), tr("Remove all items from the cart?")) == QMessageBox::Yes)
        m_cart.clear();
}

void PosSessionWidget::onCheckout()
{
    if (m_cart.isEmpty()) return;
    CheckoutDialog dlg(m_api, &m_cart, m_bootstrap, m_discountPercent, this);
    if (dlg.exec() == QDialog::Accepted)
        reloadBootstrap();
}

void PosSessionWidget::onReturn()
{
    ReturnDialog dlg(m_api, m_bootstrap.currency, this);
    if (dlg.exec() == QDialog::Accepted)
        reloadBootstrap();
}

void PosSessionWidget::onAddProduct()
{
    AddProductDialog dlg(m_api, m_bootstrap.currency,
                         m_bootstrap.branchProductSeparate,
                         m_bootstrap.branches,
                         m_bootstrap.selectedBranchId > 0 ? m_bootstrap.selectedBranchId : m_api->branchId(),
                         this);
    if (dlg.exec() == QDialog::Accepted) {
        const ProductCard p = dlg.createdProduct();
        m_productsById.insert(p.id, p);
        if (!p.sku.isEmpty()) m_productsBySku.insert(p.sku, p);
        if (p.inStock()) addProductToCart(p);
        reloadBootstrap();
    }
}

void PosSessionWidget::onPrevPage()
{
    if (m_currentPage <= 1) return;
    --m_currentPage;
    reloadBootstrap();
    m_productScroll->verticalScrollBar()->setValue(0);
}

void PosSessionWidget::onNextPage()
{
    if (m_currentPage >= m_bootstrap.productsMeta.lastPage) return;
    ++m_currentPage;
    reloadBootstrap();
    m_productScroll->verticalScrollBar()->setValue(0);
}

void PosSessionWidget::updatePaginationBar()
{
    const ProductsMeta &meta = m_bootstrap.productsMeta;
    const bool multiPage = meta.lastPage > 1;
    m_paginationBar->setVisible(multiPage);
    if (!multiPage) return;

    m_prevPageBtn->setEnabled(meta.currentPage > 1);
    m_nextPageBtn->setEnabled(meta.currentPage < meta.lastPage);

    // Left chip — total count
    m_pageLabel->setText(tr("%1 products").arg(meta.total));

    // Center label — page X of Y
    auto *infoLabel = m_paginationBar->findChild<QLabel *>(QStringLiteral("pageInfoLabel"));
    if (infoLabel) {
        infoLabel->setText(tr("Page  %1  /  %2")
            .arg(meta.currentPage)
            .arg(meta.lastPage));
    }
}

} // namespace pos
