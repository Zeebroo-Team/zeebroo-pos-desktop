#include "ui/PurchaseOrderDialog.h"
#include "ui/PurchaseOrderFormDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
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

PurchaseOrderDialog::PurchaseOrderDialog(ApiClient *api, const BootstrapData &bootstrap,
                                         QWidget *parent)
    : QDialog(parent)
    , m_api(api)
    , m_bootstrap(bootstrap)
{
    setWindowTitle(tr("Purchase Orders"));
    setMinimumSize(960, 620);
    resize(1100, 680);
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

    // ── Header bar ───────────────────────────────────────────────────────────
    auto *headerBar = new QWidget(this);
    headerBar->setObjectName(QStringLiteral("poDialogHeader"));
    auto *headerLayout = new QHBoxLayout(headerBar);
    headerLayout->setContentsMargins(16, 12, 16, 12);

    auto *titleLbl = new QLabel(tr("Purchase Orders"), headerBar);
    titleLbl->setObjectName(QStringLiteral("poDialogTitle"));

    m_filterCombo = new QComboBox(headerBar);
    m_filterCombo->setObjectName(QStringLiteral("poFilterCombo"));
    m_filterCombo->addItem(tr("All"),                QStringLiteral("all"));
    m_filterCombo->addItem(tr("Draft"),              QStringLiteral("draft"));
    m_filterCombo->addItem(tr("Ordered"),            QStringLiteral("ordered"));
    m_filterCombo->addItem(tr("Partially Received"), QStringLiteral("partially_received"));
    m_filterCombo->addItem(tr("Received"),           QStringLiteral("received"));
    m_filterCombo->addItem(tr("Cancelled"),          QStringLiteral("cancelled"));
    connect(m_filterCombo, &QComboBox::currentIndexChanged, this, &PurchaseOrderDialog::onFilterChanged);

    m_refreshBtn = new QPushButton(tr("⟳  Refresh"), headerBar);
    m_refreshBtn->setObjectName(QStringLiteral("poRefreshBtn"));
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(m_refreshBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::refreshList);

    m_newPoBtn = new QPushButton(tr("＋  New PO"), headerBar);
    m_newPoBtn->setObjectName(QStringLiteral("poNewBtn"));
    m_newPoBtn->setCursor(Qt::PointingHandCursor);
    connect(m_newPoBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onNewPo);

    headerLayout->addWidget(titleLbl);
    headerLayout->addStretch();
    headerLayout->addWidget(m_filterCombo);
    headerLayout->addWidget(m_refreshBtn);
    headerLayout->addWidget(m_newPoBtn);
    root->addWidget(headerBar);

    // ── Divider ──────────────────────────────────────────────────────────────
    auto *divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName(QStringLiteral("poDivider"));
    root->addWidget(divider);

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
    m_list->setSpacing(2);
    connect(m_list, &QListWidget::itemClicked, this, &PurchaseOrderDialog::onListItemClicked);
    leftLayout->addWidget(m_list, 1);

    splitter->addWidget(leftPanel);

    // ── Right: detail stack ──────────────────────────────────────────────────
    m_stack = new QStackedWidget(splitter);
    m_stack->setObjectName(QStringLiteral("poDetailStack"));

    // Page 0 – placeholder
    auto *placeholderPage = new QWidget;
    auto *placeholderLayout = new QVBoxLayout(placeholderPage);
    placeholderLayout->setAlignment(Qt::AlignCenter);
    auto *placeholderLbl = new QLabel(tr("Select a purchase order\nfrom the list"));
    placeholderLbl->setObjectName(QStringLiteral("poPlaceholderLabel"));
    placeholderLbl->setAlignment(Qt::AlignCenter);
    placeholderLayout->addWidget(placeholderLbl);
    m_stack->addWidget(placeholderPage);

    // Page 1 – detail
    auto *detailPage   = new QWidget;
    auto *detailScroll = new QScrollArea;
    detailScroll->setWidget(detailPage);
    detailScroll->setWidgetResizable(true);
    detailScroll->setFrameShape(QFrame::NoFrame);
    detailScroll->setObjectName(QStringLiteral("poDetailScroll"));

    auto *detailLayout = new QVBoxLayout(detailPage);
    detailLayout->setContentsMargins(20, 16, 20, 16);
    detailLayout->setSpacing(10);

    // PO# + status row
    auto *poHeaderRow = new QHBoxLayout;
    m_detPoNumber = new QLabel(detailPage);
    m_detPoNumber->setObjectName(QStringLiteral("detPoNumber"));
    m_detStatus = new QLabel(detailPage);
    m_detStatus->setObjectName(QStringLiteral("detStatus"));
    poHeaderRow->addWidget(m_detPoNumber);
    poHeaderRow->addStretch();
    poHeaderRow->addWidget(m_detStatus);
    detailLayout->addLayout(poHeaderRow);

    // Meta grid
    auto *metaGrid = new QHBoxLayout;
    auto makeMetaCol = [&](QLabel *&val, const QString &labelText) {
        auto *col = new QVBoxLayout;
        col->setSpacing(2);
        auto *lbl = new QLabel(labelText, detailPage);
        lbl->setObjectName(QStringLiteral("detMetaKey"));
        val = new QLabel(detailPage);
        val->setObjectName(QStringLiteral("detMetaVal"));
        val->setWordWrap(true);
        col->addWidget(lbl);
        col->addWidget(val);
        metaGrid->addLayout(col);
        metaGrid->addStretch(1);
    };
    makeMetaCol(m_detSupplier, tr("Supplier"));
    makeMetaCol(m_detDate,     tr("Date"));
    makeMetaCol(m_detExpected, tr("Expected Delivery"));
    makeMetaCol(m_detNotes,    tr("Notes"));
    detailLayout->addLayout(metaGrid);

    // Items table
    auto *itemsLbl = new QLabel(tr("Line Items"), detailPage);
    itemsLbl->setObjectName(QStringLiteral("detSectionLabel"));
    detailLayout->addWidget(itemsLbl);

    m_detItemsTable = new QTableWidget(0, 5, detailPage);
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
    m_detItemsTable->setMinimumHeight(120);
    detailLayout->addWidget(m_detItemsTable);

    // Total row
    auto *totalRow = new QHBoxLayout;
    totalRow->addStretch();
    m_detTotal = new QLabel(detailPage);
    m_detTotal->setObjectName(QStringLiteral("detTotalLabel"));
    totalRow->addWidget(m_detTotal);
    detailLayout->addLayout(totalRow);

    detailLayout->addStretch();

    // Action buttons
    auto *actionDivider = new QFrame(detailPage);
    actionDivider->setFrameShape(QFrame::HLine);
    actionDivider->setObjectName(QStringLiteral("poDivider"));
    detailLayout->addWidget(actionDivider);

    auto *actionRow = new QHBoxLayout;
    m_placeBtn = new QPushButton(tr("Place Order"), detailPage);
    m_placeBtn->setObjectName(QStringLiteral("poPlaceBtn"));
    m_placeBtn->setCursor(Qt::PointingHandCursor);
    connect(m_placeBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onPlaceOrder);

    m_receiveBtn = new QPushButton(tr("Mark Received"), detailPage);
    m_receiveBtn->setObjectName(QStringLiteral("poReceiveBtn"));
    m_receiveBtn->setCursor(Qt::PointingHandCursor);
    connect(m_receiveBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onReceive);

    m_cancelPoBtn = new QPushButton(tr("Cancel PO"), detailPage);
    m_cancelPoBtn->setObjectName(QStringLiteral("poCancelPoBtn"));
    m_cancelPoBtn->setCursor(Qt::PointingHandCursor);
    connect(m_cancelPoBtn, &QPushButton::clicked, this, &PurchaseOrderDialog::onCancelPo);

    actionRow->addWidget(m_placeBtn);
    actionRow->addWidget(m_receiveBtn);
    actionRow->addStretch();
    actionRow->addWidget(m_cancelPoBtn);
    detailLayout->addLayout(actionRow);

    m_stack->addWidget(detailScroll);

    splitter->addWidget(m_stack);
    splitter->setSizes({320, 780});
    root->addWidget(splitter, 1);

    // ── Bottom bar ───────────────────────────────────────────────────────────
    auto *bottomBar = new QWidget(this);
    bottomBar->setObjectName(QStringLiteral("poBottomBar"));
    auto *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(16, 8, 16, 8);
    bottomLayout->addStretch();
    auto *closeBtn = new QPushButton(tr("Close"), bottomBar);
    closeBtn->setObjectName(QStringLiteral("poCloseBtn"));
    closeBtn->setDefault(true);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    bottomLayout->addWidget(closeBtn);
    root->addWidget(bottomBar);
}

// ── Data loading ───────────────────────────────────────────────────────────────

void PurchaseOrderDialog::refreshList()
{
    m_refreshBtn->setEnabled(false);
    m_list->setEnabled(false);
    const QString status = m_filterCombo->currentData().toString();

    m_api->fetchPurchaseOrders(status,
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

void PurchaseOrderDialog::onFilterChanged()
{
    showDetailPlaceholder();
    m_selectedPoId = 0;
    refreshList();
}

void PurchaseOrderDialog::populateList(const QVector<PurchaseOrder> &orders)
{
    m_list->clear();
    for (const PurchaseOrder &po : orders) {
        auto *item = new QListWidgetItem(m_list);
        item->setData(Qt::UserRole, po.id);

        const QString supplier = po.supplierName.isEmpty() ? tr("—") : po.supplierName;
        const QString dateStr  = po.purchaseDate.isEmpty() ? QString() : QStringLiteral("  ") + po.purchaseDate;
        item->setText(QStringLiteral("%1\n%2%3\n%4   %5")
            .arg(po.poNumber)
            .arg(supplier)
            .arg(dateStr)
            .arg(statusBadge(po))
            .arg(money(po.total)));
        item->setSizeHint({0, 72});

        // Colour-code status via foreground (readable but subtle)
        if (po.isCancelled()) {
            item->setForeground(QColor(QStringLiteral("#94a3b8")));
        } else if (po.isReceived()) {
            item->setForeground(QColor(QStringLiteral("#059669")));
        } else if (po.isOrdered() || po.isPartiallyReceived()) {
            item->setForeground(QColor(QStringLiteral("#2563eb")));
        }

        m_list->addItem(item);
    }

    if (orders.isEmpty()) {
        auto *placeholder = new QListWidgetItem(tr("No purchase orders found."), m_list);
        placeholder->setFlags(Qt::NoItemFlags);
        placeholder->setForeground(QColor(QStringLiteral("#94a3b8")));
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
    m_detPoNumber->setText(QStringLiteral("<b>%1</b>").arg(po.poNumber));
    m_detStatus->setText(statusBadge(po));
    m_detSupplier->setText(po.supplierName.isEmpty() ? tr("—") : po.supplierName);
    m_detDate->setText(po.purchaseDate.isEmpty() ? tr("—") : po.purchaseDate);
    m_detExpected->setText(po.expectedDeliveryDate.isEmpty() ? tr("—") : po.expectedDeliveryDate);
    m_detNotes->setText(po.notes.isEmpty() ? tr("—") : po.notes);

    m_detItemsTable->setRowCount(0);
    for (const PurchaseOrderItem &item : po.items) {
        const int row = m_detItemsTable->rowCount();
        m_detItemsTable->insertRow(row);
        m_detItemsTable->setItem(row, 0, new QTableWidgetItem(item.productName));
        m_detItemsTable->setItem(row, 1, new QTableWidgetItem(item.sku));
        auto *qtyItem = new QTableWidgetItem(QString::number(item.quantity, 'f', 2));
        qtyItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_detItemsTable->setItem(row, 2, qtyItem);
        auto *costItem = new QTableWidgetItem(money(item.unitCost));
        costItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_detItemsTable->setItem(row, 3, costItem);
        auto *totalItem = new QTableWidgetItem(money(item.lineTotal));
        totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_detItemsTable->setItem(row, 4, totalItem);
    }

    m_detTotal->setText(tr("<b>Total:  %1</b>").arg(money(po.total)));

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
    const auto reply = QMessageBox::question(this, tr("Mark Received"),
        tr("Mark all remaining items on %1 as received and apply stock?").arg(m_selectedPo.poNumber),
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
        tr("Cancel %1? This cannot be undone.").arg(m_selectedPo.poNumber),
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

QString PurchaseOrderDialog::statusBadge(const PurchaseOrder &po) const
{
    return po.statusLabel.isEmpty() ? po.status : po.statusLabel;
}

QString PurchaseOrderDialog::money(double v) const
{
    const QString cur = m_bootstrap.currency;
    return QString::number(v, 'f', 2) + (cur.isEmpty() ? QString() : QStringLiteral(" ") + cur);
}

} // namespace pos
