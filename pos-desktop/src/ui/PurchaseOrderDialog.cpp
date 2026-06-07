#include "ui/PurchaseOrderDialog.h"
#include "ui/PurchaseOrderFormDialog.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace pos {

// ── Status helpers ─────────────────────────────────────────────────────────────

static QString statusColor(const PurchaseOrder &po)
{
    if (po.isDraft())             return QStringLiteral("#64748b");
    if (po.isOrdered())           return QStringLiteral("#2563eb");
    if (po.isPartiallyReceived()) return QStringLiteral("#d97706");
    if (po.isReceived())          return QStringLiteral("#059669");
    if (po.isCancelled())         return QStringLiteral("#94a3b8");
    return QStringLiteral("#6366f1");
}

static QString statusBgColor(const PurchaseOrder &po)
{
    if (po.isDraft())             return QStringLiteral("#f1f5f9");
    if (po.isOrdered())           return QStringLiteral("#dbeafe");
    if (po.isPartiallyReceived()) return QStringLiteral("#fef3c7");
    if (po.isReceived())          return QStringLiteral("#d1fae5");
    if (po.isCancelled())         return QStringLiteral("#f1f5f9");
    return QStringLiteral("#ede9fe");
}

// ── Constructor ────────────────────────────────────────────────────────────────

PurchaseOrderDialog::PurchaseOrderDialog(ApiClient *api, const BootstrapData &bootstrap,
                                         QWidget *parent)
    : QDialog(parent)
    , m_api(api)
    , m_bootstrap(bootstrap)
{
    setWindowTitle(tr("Purchase Orders"));
    setMinimumSize(1000, 640);
    resize(1160, 720);
    setObjectName(QStringLiteral("purchaseOrderDialog"));
    buildUi();
    refreshList();
}

// ── UI construction ────────────────────────────────────────────────────────────

void PurchaseOrderDialog::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Gradient header ──────────────────────────────────────────────────────
    auto *headerBar = new QWidget(this);
    headerBar->setObjectName(QStringLiteral("poGradientHeader"));
    auto *headerLayout = new QVBoxLayout(headerBar);
    headerLayout->setContentsMargins(20, 16, 20, 0);
    headerLayout->setSpacing(0);

    // Top row: title + buttons
    auto *topRow = new QHBoxLayout;
    topRow->setSpacing(10);

    auto *titleCol = new QVBoxLayout;
    titleCol->setSpacing(2);
    auto *titleLbl = new QLabel(tr("Purchase Orders"), headerBar);
    titleLbl->setObjectName(QStringLiteral("poHeaderTitle"));
    auto *subtitleLbl = new QLabel(tr("Manage supplier orders and stock receiving"), headerBar);
    subtitleLbl->setObjectName(QStringLiteral("poHeaderSubtitle"));
    titleCol->addWidget(titleLbl);
    titleCol->addWidget(subtitleLbl);

    m_refreshBtn = new QPushButton(tr("⟳  Refresh"), headerBar);
    m_refreshBtn->setObjectName(QStringLiteral("poHeaderRefreshBtn"));
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setFixedHeight(36);
    connect(m_refreshBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::refreshList);

    m_newPoBtn = new QPushButton(tr("＋  New Purchase Order"), headerBar);
    m_newPoBtn->setObjectName(QStringLiteral("poHeaderNewBtn"));
    m_newPoBtn->setCursor(Qt::PointingHandCursor);
    m_newPoBtn->setFixedHeight(36);
    connect(m_newPoBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onNewPo);

    topRow->addLayout(titleCol, 1);
    topRow->addWidget(m_refreshBtn);
    topRow->addWidget(m_newPoBtn);
    headerLayout->addLayout(topRow);
    headerLayout->addSpacing(12);

    // Filter pill row
    auto *pillRow = new QHBoxLayout;
    pillRow->setSpacing(4);
    pillRow->setContentsMargins(0, 0, 0, 0);

    m_filterGroup = new QButtonGroup(this);
    m_filterGroup->setExclusive(true);

    const QVector<QPair<QString,QString>> filters = {
        {tr("All"),                QStringLiteral("all")},
        {tr("Draft"),              QStringLiteral("draft")},
        {tr("Ordered"),            QStringLiteral("ordered")},
        {tr("Partially Received"), QStringLiteral("partially_received")},
        {tr("Received"),           QStringLiteral("received")},
        {tr("Cancelled"),          QStringLiteral("cancelled")},
    };

    for (const auto &[label, value] : filters) {
        auto *btn = new QPushButton(label, headerBar);
        btn->setObjectName(QStringLiteral("poFilterPill"));
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(30);
        btn->setProperty("filterValue", value);
        if (value == QStringLiteral("all")) btn->setChecked(true);
        connect(btn, &QPushButton::clicked, this, &PurchaseOrderDialog::onFilterClicked);
        m_filterGroup->addButton(btn);
        pillRow->addWidget(btn);
    }
    pillRow->addStretch();
    headerLayout->addLayout(pillRow);
    headerLayout->addSpacing(0);

    root->addWidget(headerBar);

    // ── Splitter ─────────────────────────────────────────────────────────────
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setObjectName(QStringLiteral("poSplitter"));
    splitter->setHandleWidth(1);

    // ── Left: PO list ────────────────────────────────────────────────────────
    auto *leftPanel = new QWidget(splitter);
    leftPanel->setObjectName(QStringLiteral("poListPanel"));
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    m_list = new QListWidget(leftPanel);
    m_list->setObjectName(QStringLiteral("poList"));
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_list->setSpacing(0);
    m_list->setUniformItemSizes(false);
    connect(m_list, &QListWidget::itemClicked, this, &PurchaseOrderDialog::onListItemClicked);

    // Sync item widget visual selection on row change
    connect(m_list, &QListWidget::currentRowChanged, this, [this](int currentRow) {
        for (int i = 0; i < m_list->count(); ++i) {
            auto *w = m_list->itemWidget(m_list->item(i));
            if (!w) continue;
            const bool sel = (i == currentRow);
            w->setProperty("selected", sel);
            w->style()->unpolish(w);
            w->style()->polish(w);
            w->update();
            // also update children
            const auto children = w->findChildren<QWidget*>();
            for (auto *c : children) {
                c->style()->unpolish(c);
                c->style()->polish(c);
                c->update();
            }
        }
    });

    leftLayout->addWidget(m_list, 1);
    splitter->addWidget(leftPanel);

    // ── Right: detail stack ──────────────────────────────────────────────────
    m_stack = new QStackedWidget(splitter);
    m_stack->setObjectName(QStringLiteral("poDetailStack"));

    // Page 0 – placeholder
    auto *placeholderPage = new QWidget;
    placeholderPage->setObjectName(QStringLiteral("poPlaceholderPage"));
    auto *placeholderLayout = new QVBoxLayout(placeholderPage);
    placeholderLayout->setAlignment(Qt::AlignCenter);
    auto *phIcon = new QLabel(QStringLiteral("📋"), placeholderPage);
    phIcon->setObjectName(QStringLiteral("poPlaceholderIcon"));
    phIcon->setAlignment(Qt::AlignCenter);
    auto *placeholderLbl = new QLabel(tr("Select a purchase order\nfrom the list to view details"), placeholderPage);
    placeholderLbl->setObjectName(QStringLiteral("poPlaceholderLabel"));
    placeholderLbl->setAlignment(Qt::AlignCenter);
    placeholderLayout->addWidget(phIcon);
    placeholderLayout->addWidget(placeholderLbl);
    m_stack->addWidget(placeholderPage);

    // Page 1 – detail
    auto *detailPage   = new QWidget;
    detailPage->setObjectName(QStringLiteral("poDetailPage"));
    auto *detailScroll = new QScrollArea;
    detailScroll->setWidget(detailPage);
    detailScroll->setWidgetResizable(true);
    detailScroll->setFrameShape(QFrame::NoFrame);
    detailScroll->setObjectName(QStringLiteral("poDetailScroll"));

    auto *detailLayout = new QVBoxLayout(detailPage);
    detailLayout->setContentsMargins(20, 20, 20, 20);
    detailLayout->setSpacing(14);

    // ── PO summary card ──────────────────────────────────────────────────────
    auto *summaryCard = new QFrame(detailPage);
    summaryCard->setObjectName(QStringLiteral("poDetailCard"));
    auto *summaryLayout = new QVBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(16, 14, 16, 14);
    summaryLayout->setSpacing(10);

    auto *poHeaderRow = new QHBoxLayout;
    m_detPoNumber = new QLabel(summaryCard);
    m_detPoNumber->setObjectName(QStringLiteral("detPoNumber"));
    m_detStatus = new QLabel(summaryCard);
    m_detStatus->setObjectName(QStringLiteral("detStatusBadge"));
    poHeaderRow->addWidget(m_detPoNumber);
    poHeaderRow->addStretch();
    poHeaderRow->addWidget(m_detStatus);
    summaryLayout->addLayout(poHeaderRow);

    // Meta info row
    auto *metaRow = new QHBoxLayout;
    metaRow->setSpacing(24);

    auto makeMetaCell = [&](QLabel *&val, const QString &keyText) {
        auto *cell = new QVBoxLayout;
        cell->setSpacing(3);
        auto *key = new QLabel(keyText, summaryCard);
        key->setObjectName(QStringLiteral("detMetaKey"));
        val = new QLabel(summaryCard);
        val->setObjectName(QStringLiteral("detMetaVal"));
        val->setWordWrap(true);
        cell->addWidget(key);
        cell->addWidget(val);
        metaRow->addLayout(cell);
    };

    makeMetaCell(m_detSupplier, tr("SUPPLIER"));
    makeMetaCell(m_detDate,     tr("DATE"));
    makeMetaCell(m_detExpected, tr("EXPECTED DELIVERY"));
    makeMetaCell(m_detNotes,    tr("NOTES"));
    metaRow->addStretch();
    summaryLayout->addLayout(metaRow);
    detailLayout->addWidget(summaryCard);

    // ── Line items card ──────────────────────────────────────────────────────
    auto *itemsCard = new QFrame(detailPage);
    itemsCard->setObjectName(QStringLiteral("poDetailCard"));
    auto *itemsCardLayout = new QVBoxLayout(itemsCard);
    itemsCardLayout->setContentsMargins(16, 14, 16, 14);
    itemsCardLayout->setSpacing(10);

    auto *itemsHeader = new QHBoxLayout;
    auto *itemsLbl = new QLabel(tr("LINE ITEMS"), itemsCard);
    itemsLbl->setObjectName(QStringLiteral("detSectionLabel"));
    itemsHeader->addWidget(itemsLbl);
    itemsHeader->addStretch();
    itemsCardLayout->addLayout(itemsHeader);

    m_detItemsTable = new QTableWidget(0, 5, itemsCard);
    m_detItemsTable->setObjectName(QStringLiteral("detItemsTable"));
    m_detItemsTable->setHorizontalHeaderLabels({tr("Product"), tr("SKU"), tr("Qty"), tr("Unit Cost"), tr("Total")});
    m_detItemsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_detItemsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_detItemsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_detItemsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_detItemsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_detItemsTable->verticalHeader()->hide();
    m_detItemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_detItemsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_detItemsTable->setAlternatingRowColors(true);
    m_detItemsTable->setMinimumHeight(140);
    m_detItemsTable->setShowGrid(false);
    itemsCardLayout->addWidget(m_detItemsTable);

    // Total row
    auto *totalRow = new QHBoxLayout;
    totalRow->addStretch();
    m_detTotal = new QLabel(itemsCard);
    m_detTotal->setObjectName(QStringLiteral("detTotalLabel"));
    totalRow->addWidget(m_detTotal);
    itemsCardLayout->addLayout(totalRow);
    detailLayout->addWidget(itemsCard);

    detailLayout->addStretch();

    // ── Action buttons ───────────────────────────────────────────────────────
    auto *actionCard = new QFrame(detailPage);
    actionCard->setObjectName(QStringLiteral("poActionBar"));
    auto *actionLayout = new QHBoxLayout(actionCard);
    actionLayout->setContentsMargins(16, 12, 16, 12);
    actionLayout->setSpacing(10);

    m_placeBtn = new QPushButton(tr("↑  Place Order"), actionCard);
    m_placeBtn->setObjectName(QStringLiteral("poPlaceBtn"));
    m_placeBtn->setCursor(Qt::PointingHandCursor);
    m_placeBtn->setFixedHeight(40);
    connect(m_placeBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onPlaceOrder);

    m_receiveBtn = new QPushButton(tr("✓  Mark as Received"), actionCard);
    m_receiveBtn->setObjectName(QStringLiteral("poReceiveBtn"));
    m_receiveBtn->setCursor(Qt::PointingHandCursor);
    m_receiveBtn->setFixedHeight(40);
    connect(m_receiveBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onReceive);

    m_cancelPoBtn = new QPushButton(tr("✕  Cancel PO"), actionCard);
    m_cancelPoBtn->setObjectName(QStringLiteral("poCancelPoBtn"));
    m_cancelPoBtn->setCursor(Qt::PointingHandCursor);
    m_cancelPoBtn->setFixedHeight(40);
    connect(m_cancelPoBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onCancelPo);

    actionLayout->addWidget(m_placeBtn);
    actionLayout->addWidget(m_receiveBtn);
    actionLayout->addStretch();
    actionLayout->addWidget(m_cancelPoBtn);
    detailLayout->addWidget(actionCard);

    m_stack->addWidget(detailScroll);
    splitter->addWidget(m_stack);
    splitter->setSizes({320, 840});
    root->addWidget(splitter, 1);

    // ── Bottom bar ───────────────────────────────────────────────────────────
    auto *bottomBar = new QWidget(this);
    bottomBar->setObjectName(QStringLiteral("poBottomBar"));
    auto *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(20, 10, 20, 10);
    bottomLayout->addStretch();
    auto *closeBtn = new QPushButton(tr("Close"), bottomBar);
    closeBtn->setObjectName(QStringLiteral("poCloseBtn"));
    closeBtn->setFixedHeight(38);
    closeBtn->setMinimumWidth(100);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    bottomLayout->addWidget(closeBtn);
    root->addWidget(bottomBar);
}

// ── Filter ─────────────────────────────────────────────────────────────────────

void PurchaseOrderDialog::onFilterClicked()
{
    auto *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;
    m_activeFilter = btn->property("filterValue").toString();
    showDetailPlaceholder();
    m_selectedPoId = 0;
    refreshList();
}

// ── Data loading ───────────────────────────────────────────────────────────────

void PurchaseOrderDialog::refreshList()
{
    m_refreshBtn->setEnabled(false);
    m_list->setEnabled(false);

    m_api->fetchPurchaseOrders(m_activeFilter,
        [this](const QJsonObject &root) {
            m_orders.clear();
            for (const QJsonValue &v : root.value(QStringLiteral("data")).toArray()) {
                m_orders.append(PurchaseOrder::fromJson(v.toObject()));
            }
            populateList(m_orders);
            m_refreshBtn->setEnabled(true);
            m_list->setEnabled(true);
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            m_refreshBtn->setEnabled(true);
            m_list->setEnabled(true);
        });
}

void PurchaseOrderDialog::populateList(const QVector<PurchaseOrder> &orders)
{
    m_list->clear();

    if (orders.isEmpty()) {
        auto *item = new QListWidgetItem(m_list);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint({0, 60});

        auto *w = new QWidget;
        w->setObjectName(QStringLiteral("poListEmpty"));
        auto *lbl = new QLabel(tr("No purchase orders found."), w);
        lbl->setObjectName(QStringLiteral("poEmptyLabel"));
        lbl->setAlignment(Qt::AlignCenter);
        auto *lay = new QVBoxLayout(w);
        lay->addWidget(lbl);
        m_list->setItemWidget(item, w);
        return;
    }

    for (const PurchaseOrder &po : orders) {
        auto *item = new QListWidgetItem(m_list);
        item->setData(Qt::UserRole, po.id);
        item->setSizeHint({0, 82});
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        // ── Custom card widget for each list item ──────────────────────────
        auto *w = new QWidget;
        w->setObjectName(QStringLiteral("poListItem"));
        w->setProperty("selected", false);

        // Colored left status strip
        auto *strip = new QWidget(w);
        strip->setFixedWidth(4);
        strip->setObjectName(QStringLiteral("poListStrip"));
        strip->setStyleSheet(
            QStringLiteral("background:%1; border-radius:2px;").arg(statusColor(po)));

        // Body column
        auto *body = new QWidget(w);
        auto *bodyLayout = new QVBoxLayout(body);
        bodyLayout->setContentsMargins(10, 10, 8, 10);
        bodyLayout->setSpacing(3);

        auto *poNumRow = new QHBoxLayout;
        auto *poNumLbl = new QLabel(po.poNumber, body);
        poNumLbl->setObjectName(QStringLiteral("poListPoNum"));

        // Status badge chip
        auto *statusChip = new QLabel(statusLabel(po), body);
        statusChip->setObjectName(QStringLiteral("poListStatusChip"));
        statusChip->setStyleSheet(
            QStringLiteral("color:%1; background:%2; border-radius:8px; padding:1px 8px; font-size:10px; font-weight:700;")
                .arg(statusColor(po), statusBgColor(po)));

        poNumRow->addWidget(poNumLbl);
        poNumRow->addStretch();
        poNumRow->addWidget(statusChip);
        bodyLayout->addLayout(poNumRow);

        const QString supplierStr = po.supplierName.isEmpty() ? tr("No supplier") : po.supplierName;
        auto *supplierLbl = new QLabel(supplierStr, body);
        supplierLbl->setObjectName(QStringLiteral("poListSupplier"));
        bodyLayout->addWidget(supplierLbl);

        auto *bottomRow = new QHBoxLayout;
        auto *dateLbl = new QLabel(po.purchaseDate.isEmpty() ? tr("—") : po.purchaseDate, body);
        dateLbl->setObjectName(QStringLiteral("poListDate"));
        auto *amountLbl = new QLabel(money(po.total), body);
        amountLbl->setObjectName(QStringLiteral("poListAmount"));
        bottomRow->addWidget(dateLbl);
        bottomRow->addStretch();
        bottomRow->addWidget(amountLbl);
        bodyLayout->addLayout(bottomRow);

        // Compose item widget
        auto *hLayout = new QHBoxLayout(w);
        hLayout->setContentsMargins(0, 0, 0, 0);
        hLayout->setSpacing(0);
        hLayout->addWidget(strip);
        hLayout->addWidget(body, 1);

        m_list->setItemWidget(item, w);
    }
}

void PurchaseOrderDialog::onListItemClicked(QListWidgetItem *item)
{
    const int id = item->data(Qt::UserRole).toInt();
    if (id <= 0) return;
    if (id == m_selectedPoId) return;
    m_selectedPoId = id;
    loadDetail(id);
}

void PurchaseOrderDialog::loadDetail(int poId)
{
    m_api->fetchPurchaseOrder(poId,
        [this](const QJsonObject &root) {
            m_selectedPo = PurchaseOrder::fromJson(root.value(QStringLiteral("data")).toObject());
            showDetail(m_selectedPo);
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            showDetailPlaceholder();
        });
}

// ── Detail rendering ───────────────────────────────────────────────────────────

void PurchaseOrderDialog::showDetail(const PurchaseOrder &po)
{
    m_detPoNumber->setText(po.poNumber);
    m_detSupplier->setText(po.supplierName.isEmpty() ? tr("—") : po.supplierName);
    m_detDate->setText(po.purchaseDate.isEmpty() ? tr("—") : po.purchaseDate);
    m_detExpected->setText(po.expectedDeliveryDate.isEmpty() ? tr("—") : po.expectedDeliveryDate);
    m_detNotes->setText(po.notes.isEmpty() ? tr("—") : po.notes);

    // Color-coded status badge
    m_detStatus->setText(statusLabel(po));
    m_detStatus->setStyleSheet(
        QStringLiteral("color:%1; background:%2; border-radius:10px; padding:3px 12px;"
                       " font-size:11px; font-weight:700;")
            .arg(statusColor(po), statusBgColor(po)));

    // Items table
    m_detItemsTable->setRowCount(0);
    for (const PurchaseOrderItem &itm : po.items) {
        const int row = m_detItemsTable->rowCount();
        m_detItemsTable->insertRow(row);
        m_detItemsTable->setRowHeight(row, 36);
        m_detItemsTable->setItem(row, 0, new QTableWidgetItem(itm.productName));
        m_detItemsTable->setItem(row, 1, new QTableWidgetItem(itm.sku));
        auto *qtyItem = new QTableWidgetItem(QString::number(itm.quantity, 'f', 2));
        qtyItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_detItemsTable->setItem(row, 2, qtyItem);
        auto *costItem = new QTableWidgetItem(money(itm.unitCost));
        costItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_detItemsTable->setItem(row, 3, costItem);
        auto *totalItem = new QTableWidgetItem(money(itm.lineTotal));
        totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_detItemsTable->setItem(row, 4, totalItem);
    }

    m_detTotal->setText(tr("Total:  %1").arg(money(po.total)));
    setDetailButtons(po);
    m_stack->setCurrentIndex(1);
}

void PurchaseOrderDialog::showDetailPlaceholder()
{
    m_stack->setCurrentIndex(0);
}

void PurchaseOrderDialog::setDetailButtons(const PurchaseOrder &po)
{
    m_placeBtn->setVisible(po.canPlaceOrder());
    m_receiveBtn->setVisible(po.canReceive());
    m_cancelPoBtn->setVisible(po.canCancel());
}

// ── PO actions ─────────────────────────────────────────────────────────────────

void PurchaseOrderDialog::onNewPo()
{
    auto *form = new PurchaseOrderFormDialog(m_api, m_bootstrap, this);
    if (form->exec() == QDialog::Accepted) {
        refreshList();
    }
    form->deleteLater();
}

void PurchaseOrderDialog::onPlaceOrder()
{
    if (m_selectedPoId <= 0) return;
    m_placeBtn->setEnabled(false);
    m_api->placePurchaseOrder(m_selectedPoId,
        [this](const QJsonObject &root) {
            m_selectedPo = PurchaseOrder::fromJson(root.value(QStringLiteral("data")).toObject());
            showDetail(m_selectedPo);
            refreshList();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            m_placeBtn->setEnabled(true);
        });
}

void PurchaseOrderDialog::onReceive()
{
    if (m_selectedPoId <= 0) return;
    const auto reply = QMessageBox::question(this, tr("Mark as Received"),
        tr("Receive all remaining items on %1 and apply stock?").arg(m_selectedPo.poNumber),
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (reply != QMessageBox::Yes) return;

    m_receiveBtn->setEnabled(false);
    m_api->receivePurchaseOrder(m_selectedPoId,
        [this](const QJsonObject &root) {
            m_selectedPo = PurchaseOrder::fromJson(root.value(QStringLiteral("data")).toObject());
            showDetail(m_selectedPo);
            refreshList();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            m_receiveBtn->setEnabled(true);
        });
}

void PurchaseOrderDialog::onCancelPo()
{
    if (m_selectedPoId <= 0) return;
    const auto reply = QMessageBox::question(this, tr("Cancel Purchase Order"),
        tr("Cancel %1? This action cannot be undone.").arg(m_selectedPo.poNumber),
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (reply != QMessageBox::Yes) return;

    m_cancelPoBtn->setEnabled(false);
    m_api->cancelPurchaseOrder(m_selectedPoId,
        [this](const QJsonObject &root) {
            m_selectedPo = PurchaseOrder::fromJson(root.value(QStringLiteral("data")).toObject());
            showDetail(m_selectedPo);
            refreshList();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            m_cancelPoBtn->setEnabled(true);
        });
}

// ── Helpers ────────────────────────────────────────────────────────────────────

void PurchaseOrderDialog::syncFilterButtons()
{
    for (auto *btn : m_filterGroup->buttons()) {
        btn->setChecked(btn->property("filterValue").toString() == m_activeFilter);
    }
}

QString PurchaseOrderDialog::statusLabel(const PurchaseOrder &po) const
{
    return po.statusLabel.isEmpty() ? po.status : po.statusLabel;
}

QString PurchaseOrderDialog::money(double v) const
{
    const QString cur = m_bootstrap.currency;
    return QString::number(v, 'f', 2) + (cur.isEmpty() ? QString() : QStringLiteral(" ") + cur);
}

} // namespace pos
