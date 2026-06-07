#include "ui/PurchaseOrderFormDialog.h"
#include "ui/AddSupplierDialog.h"

#include <QComboBox>
#include <QDate>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

namespace pos {

PurchaseOrderFormDialog::PurchaseOrderFormDialog(ApiClient *api, const BootstrapData &bootstrap,
                                                 QWidget *parent)
    : QDialog(parent)
    , m_api(api)
    , m_bootstrap(bootstrap)
{
    setWindowTitle(tr("New Purchase Order"));
    setMinimumSize(660, 520);
    resize(760, 610);
    setObjectName(QStringLiteral("poFormDialog"));
    buildUi();
    loadSuppliers();
}

void PurchaseOrderFormDialog::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Header ────────────────────────────────────────────────────────────────
    auto *header = new QWidget(this);
    header->setObjectName(QStringLiteral("poGradientHeader"));
    auto *hl = new QHBoxLayout(header);
    hl->setContentsMargins(20, 14, 20, 14);
    hl->setSpacing(0);
    auto *titleCol = new QVBoxLayout;
    titleCol->setSpacing(2);
    auto *titleLbl = new QLabel(tr("New Purchase Order"), header);
    titleLbl->setObjectName(QStringLiteral("poHeaderTitle"));
    auto *subLbl = new QLabel(tr("Fill in order details and add product lines"), header);
    subLbl->setObjectName(QStringLiteral("poHeaderSubtitle"));
    titleCol->addWidget(titleLbl);
    titleCol->addWidget(subLbl);
    hl->addLayout(titleCol, 1);
    root->addWidget(header);

    // ── Scroll area ───────────────────────────────────────────────────────────
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName(QStringLiteral("poFormScroll"));

    auto *canvas = new QWidget;
    canvas->setObjectName(QStringLiteral("poFormCanvas"));
    auto *canvasLayout = new QVBoxLayout(canvas);
    canvasLayout->setContentsMargins(16, 16, 16, 16);
    canvasLayout->setSpacing(12);

    // ── Helpers ───────────────────────────────────────────────────────────────
    auto makeCard = [&](const QString &title, const QString &objName) -> QVBoxLayout * {
        auto *card = new QFrame(canvas);
        card->setObjectName(objName);
        auto *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 14, 16, 16);
        cl->setSpacing(12);
        if (!title.isEmpty()) {
            auto *hr = new QHBoxLayout;
            hr->setSpacing(8);
            auto *accent = new QWidget(card);
            accent->setObjectName(QStringLiteral("poFormSectionAccent"));
            accent->setFixedSize(3, 14);
            auto *lbl = new QLabel(title, card);
            lbl->setObjectName(QStringLiteral("poFormSectionLabel"));
            hr->addWidget(accent);
            hr->addWidget(lbl);
            hr->addStretch();
            cl->addLayout(hr);
        }
        canvasLayout->addWidget(card);
        return cl;
    };

    auto makeField = [](const QString &labelText, QWidget *field, QWidget *parent) -> QVBoxLayout * {
        auto *col = new QVBoxLayout;
        col->setSpacing(5);
        auto *lbl = new QLabel(labelText, parent);
        lbl->setObjectName(QStringLiteral("poFormLabel"));
        col->addWidget(lbl);
        col->addWidget(field);
        return col;
    };

    // ── Card 1: Order Information ─────────────────────────────────────────────
    auto *infoCard = makeCard(tr("Order Information"), QStringLiteral("poFormCard"));

    auto *row1 = new QHBoxLayout;
    row1->setSpacing(12);

    // Supplier: [combo + new button]
    auto *supplierCol = new QVBoxLayout;
    supplierCol->setSpacing(5);
    auto *supplierLbl = new QLabel(tr("Supplier"), canvas);
    supplierLbl->setObjectName(QStringLiteral("poFormLabel"));
    supplierCol->addWidget(supplierLbl);
    auto *supplierInline = new QHBoxLayout;
    supplierInline->setSpacing(6);
    m_supplierCombo = new QComboBox(canvas);
    m_supplierCombo->setObjectName(QStringLiteral("poSupplierCombo"));
    m_supplierCombo->addItem(tr("— Select supplier —"), 0);
    m_supplierCombo->setFixedHeight(34);
    m_addSupplierBtn = new QPushButton(tr("+"), canvas);
    m_addSupplierBtn->setObjectName(QStringLiteral("poAddSupplierBtn"));
    m_addSupplierBtn->setFixedSize(34, 34);
    m_addSupplierBtn->setCursor(Qt::PointingHandCursor);
    m_addSupplierBtn->setToolTip(tr("Add new supplier"));
    connect(m_addSupplierBtn, &QPushButton::clicked, this, &PurchaseOrderFormDialog::onAddSupplier);
    supplierInline->addWidget(m_supplierCombo, 1);
    supplierInline->addWidget(m_addSupplierBtn);
    supplierCol->addLayout(supplierInline);
    row1->addLayout(supplierCol, 2);

    m_dateEdit = new QLineEdit(canvas);
    m_dateEdit->setObjectName(QStringLiteral("poDateEdit"));
    m_dateEdit->setPlaceholderText(QStringLiteral("YYYY-MM-DD"));
    m_dateEdit->setText(QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd")));
    m_dateEdit->setFixedHeight(34);
    row1->addLayout(makeField(tr("Purchase Date  ✱"), m_dateEdit, canvas), 1);

    m_expectedEdit = new QLineEdit(canvas);
    m_expectedEdit->setObjectName(QStringLiteral("poExpectedEdit"));
    m_expectedEdit->setPlaceholderText(tr("YYYY-MM-DD"));
    m_expectedEdit->setFixedHeight(34);
    row1->addLayout(makeField(tr("Expected Delivery"), m_expectedEdit, canvas), 1);

    infoCard->addLayout(row1);

    m_notesEdit = new QTextEdit(canvas);
    m_notesEdit->setObjectName(QStringLiteral("poNotesEdit"));
    m_notesEdit->setPlaceholderText(tr("Order notes or special instructions (optional)"));
    m_notesEdit->setFixedHeight(62);
    infoCard->addLayout(makeField(tr("Notes"), m_notesEdit, canvas));

    // ── Card 2: Add Product Line ──────────────────────────────────────────────
    auto *addCard = makeCard(tr("Add Product Line"), QStringLiteral("poAddProductCard"));

    // Row A – product search (full width)
    m_productCombo = new QComboBox(canvas);
    m_productCombo->setObjectName(QStringLiteral("poProductCombo"));
    m_productCombo->setEditable(true);
    m_productCombo->setInsertPolicy(QComboBox::NoInsert);
    m_productCombo->setPlaceholderText(tr("Search by product name or SKU…"));
    m_productCombo->setFixedHeight(34);
    for (const ProductCard &p : m_bootstrap.products)
        m_productCombo->addItem(QStringLiteral("[%1]  %2").arg(p.sku, p.name), p.id);
    if (m_productCombo->count() > 0) {
        m_productCombo->setCurrentIndex(-1);
        m_productCombo->clearEditText();
    }

    // Auto-fill unit cost when a product is selected
    connect(m_productCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx) {
        if (idx < 0) return;
        const int productId = m_productCombo->itemData(idx).toInt();
        for (const ProductCard &p : m_bootstrap.products) {
            if (p.id == productId) {
                m_addCostEdit->setText(QString::number(p.unitSellPrice, 'f', 2));
                break;
            }
        }
    });

    addCard->addLayout(makeField(tr("Product"), m_productCombo, canvas));

    // Row B – qty + cost + add button
    auto *addRow = new QHBoxLayout;
    addRow->setSpacing(10);

    m_addQtyEdit = new QLineEdit(canvas);
    m_addQtyEdit->setObjectName(QStringLiteral("poAddQty"));
    m_addQtyEdit->setPlaceholderText(tr("Qty"));
    m_addQtyEdit->setText(QStringLiteral("1"));
    m_addQtyEdit->setFixedHeight(34);

    m_addCostEdit = new QLineEdit(canvas);
    m_addCostEdit->setObjectName(QStringLiteral("poAddCost"));
    m_addCostEdit->setPlaceholderText(tr("Unit cost"));
    m_addCostEdit->setText(QStringLiteral("0.00"));
    m_addCostEdit->setFixedHeight(34);

    m_addLineBtn = new QPushButton(tr("＋  Add Line"), canvas);
    m_addLineBtn->setObjectName(QStringLiteral("poAddLineBtn"));
    m_addLineBtn->setCursor(Qt::PointingHandCursor);
    m_addLineBtn->setFixedHeight(34);
    m_addLineBtn->setMinimumWidth(110);
    connect(m_addLineBtn, &QPushButton::clicked, this, &PurchaseOrderFormDialog::onAddLine);

    // Align button baseline to bottom of field (add label-height spacer above btn)
    auto *btnCol = new QVBoxLayout;
    btnCol->setSpacing(5);
    auto *btnSpacer = new QLabel(QStringLiteral(" "), canvas); // same height as a label
    btnSpacer->setObjectName(QStringLiteral("poFormLabel"));
    btnCol->addWidget(btnSpacer);
    btnCol->addWidget(m_addLineBtn);

    addRow->addLayout(makeField(tr("Quantity"), m_addQtyEdit, canvas), 1);
    addRow->addLayout(makeField(tr("Unit Cost"), m_addCostEdit, canvas), 2);
    addRow->addStretch(1);
    addRow->addLayout(btnCol);
    addCard->addLayout(addRow);

    // ── Card 3: Line Items ────────────────────────────────────────────────────
    auto *tableCard = makeCard(tr("Line Items"), QStringLiteral("poFormCard"));

    m_itemsTable = new QTableWidget(0, 6, canvas);
    m_itemsTable->setObjectName(QStringLiteral("detItemsTable"));
    m_itemsTable->setHorizontalHeaderLabels(
        {tr("Product"), tr("SKU"), tr("Qty"), tr("Unit Cost"), tr("Total"), tr("")});
    m_itemsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    m_itemsTable->setColumnWidth(5, 32);
    m_itemsTable->verticalHeader()->hide();
    m_itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_itemsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_itemsTable->setAlternatingRowColors(true);
    m_itemsTable->setShowGrid(false);
    m_itemsTable->setMinimumHeight(120);
    tableCard->addWidget(m_itemsTable);

    // Total summary row
    auto *totalRow = new QHBoxLayout;
    totalRow->setContentsMargins(0, 4, 4, 0);
    totalRow->addStretch();
    m_totalLabel = new QLabel(tr("Total:  0.00"), canvas);
    m_totalLabel->setObjectName(QStringLiteral("detTotalLabel"));
    totalRow->addWidget(m_totalLabel);
    tableCard->addLayout(totalRow);

    scroll->setWidget(canvas);
    root->addWidget(scroll, 1);

    // ── Bottom bar ────────────────────────────────────────────────────────────
    auto *divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName(QStringLiteral("poDivider"));
    root->addWidget(divider);

    auto *bar = new QWidget(this);
    bar->setObjectName(QStringLiteral("poBottomBar"));
    auto *barLayout = new QHBoxLayout(bar);
    barLayout->setContentsMargins(16, 10, 16, 10);
    barLayout->setSpacing(8);

    auto *cancelBtn = new QPushButton(tr("Cancel"), bar);
    cancelBtn->setObjectName(QStringLiteral("poCloseBtn"));
    cancelBtn->setFixedHeight(34);
    cancelBtn->setMinimumWidth(80);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_saveDraftBtn = new QPushButton(tr("Save Draft"), bar);
    m_saveDraftBtn->setObjectName(QStringLiteral("poSaveDraftBtn"));
    m_saveDraftBtn->setCursor(Qt::PointingHandCursor);
    m_saveDraftBtn->setFixedHeight(34);
    m_saveDraftBtn->setMinimumWidth(100);
    connect(m_saveDraftBtn, &QPushButton::clicked, this, &PurchaseOrderFormDialog::onSaveDraft);

    m_placeOrderBtn = new QPushButton(tr("↑  Place Order"), bar);
    m_placeOrderBtn->setObjectName(QStringLiteral("poPlaceBtn"));
    m_placeOrderBtn->setCursor(Qt::PointingHandCursor);
    m_placeOrderBtn->setFixedHeight(34);
    m_placeOrderBtn->setMinimumWidth(130);
    m_placeOrderBtn->setDefault(true);
    connect(m_placeOrderBtn, &QPushButton::clicked, this, &PurchaseOrderFormDialog::onPlaceOrder);

    barLayout->addWidget(cancelBtn);
    barLayout->addStretch();
    barLayout->addWidget(m_saveDraftBtn);
    barLayout->addWidget(m_placeOrderBtn);
    root->addWidget(bar);
}

void PurchaseOrderFormDialog::loadSuppliers()
{
    m_api->fetchSuppliers(
        [this](const QJsonObject &root) {
            m_suppliers.clear();
            for (const QJsonValue &v : root.value(QStringLiteral("data")).toArray()) {
                m_suppliers.append(Supplier::fromJson(v.toObject()));
            }
            for (const Supplier &s : m_suppliers) {
                m_supplierCombo->addItem(s.name, s.id);
            }
        },
        [](const QString &, int) { /* non-fatal */ });
}

void PurchaseOrderFormDialog::onAddLine()
{
    const int idx = m_productCombo->currentIndex();
    if (idx < 0) {
        QMessageBox::information(this, tr("Select Product"), tr("Please select a product first."));
        return;
    }
    const int productId = m_productCombo->currentData().toInt();
    const ProductCard *found = nullptr;
    for (const ProductCard &p : m_bootstrap.products) {
        if (p.id == productId) { found = &p; break; }
    }
    if (!found) return;

    const double qty  = m_addQtyEdit->text().toDouble();
    const double cost = m_addCostEdit->text().toDouble();
    if (qty <= 0.0) {
        QMessageBox::information(this, tr("Invalid Quantity"), tr("Quantity must be greater than zero."));
        return;
    }

    addProductRow(*found, qty, cost);
    recalcTotal();
    m_productCombo->setCurrentIndex(-1);
    m_productCombo->clearEditText();
    m_addQtyEdit->setText(QStringLiteral("1"));
    m_addCostEdit->setText(QStringLiteral("0.00"));
}

void PurchaseOrderFormDialog::addProductRow(const ProductCard &product, double qty, double unitCost)
{
    const int row = m_itemsTable->rowCount();
    m_itemsTable->insertRow(row);
    m_itemsTable->setRowHeight(row, 28);

    m_itemsTable->setItem(row, 0, new QTableWidgetItem(product.name));
    m_itemsTable->setItem(row, 1, new QTableWidgetItem(product.sku));

    auto *qtyItem = new QTableWidgetItem(QString::number(qty, 'f', 2));
    qtyItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_itemsTable->setItem(row, 2, qtyItem);

    auto *costItem = new QTableWidgetItem(QString::number(unitCost, 'f', 2));
    costItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_itemsTable->setItem(row, 3, costItem);

    auto *totalItem = new QTableWidgetItem(money(qty * unitCost));
    totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_itemsTable->setItem(row, 4, totalItem);

    m_itemsTable->item(row, 0)->setData(Qt::UserRole, product.id);
    m_itemsTable->item(row, 2)->setData(Qt::UserRole, qty);
    m_itemsTable->item(row, 3)->setData(Qt::UserRole, unitCost);

    auto *delBtn = new QPushButton(QStringLiteral("×"), this);
    delBtn->setObjectName(QStringLiteral("poDelLineBtn"));
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setFixedSize(24, 24);
    connect(delBtn, &QPushButton::clicked, this, [this, delBtn]() {
        for (int r = 0; r < m_itemsTable->rowCount(); ++r) {
            if (m_itemsTable->cellWidget(r, 5) == delBtn) {
                onRemoveLine(r);
                return;
            }
        }
    });
    m_itemsTable->setCellWidget(row, 5, delBtn);
}

void PurchaseOrderFormDialog::onRemoveLine(int row)
{
    m_itemsTable->removeRow(row);
    recalcTotal();
}

void PurchaseOrderFormDialog::recalcTotal()
{
    double total = 0.0;
    for (int r = 0; r < m_itemsTable->rowCount(); ++r) {
        total += m_itemsTable->item(r, 2)->data(Qt::UserRole).toDouble()
               * m_itemsTable->item(r, 3)->data(Qt::UserRole).toDouble();
    }
    m_totalLabel->setText(tr("Order Total:  %1").arg(money(total)));
}

QJsonObject PurchaseOrderFormDialog::buildPayload(const QString &status) const
{
    QJsonObject payload;
    const int supplierId = m_supplierCombo->currentData().toInt();
    if (supplierId > 0) payload.insert(QStringLiteral("supplier_id"), supplierId);
    payload.insert(QStringLiteral("purchase_date"), m_dateEdit->text().trimmed());
    payload.insert(QStringLiteral("status"), status);
    const QString expected = m_expectedEdit->text().trimmed();
    if (!expected.isEmpty()) payload.insert(QStringLiteral("expected_delivery_date"), expected);
    const QString notes = m_notesEdit->toPlainText().trimmed();
    if (!notes.isEmpty()) payload.insert(QStringLiteral("notes"), notes);

    QJsonArray items;
    for (int r = 0; r < m_itemsTable->rowCount(); ++r) {
        QJsonObject item;
        item.insert(QStringLiteral("product_id"), m_itemsTable->item(r, 0)->data(Qt::UserRole).toInt());
        item.insert(QStringLiteral("quantity"),   m_itemsTable->item(r, 2)->data(Qt::UserRole).toDouble());
        item.insert(QStringLiteral("unit_cost"),  m_itemsTable->item(r, 3)->data(Qt::UserRole).toDouble());
        items.append(item);
    }
    payload.insert(QStringLiteral("items"), items);
    return payload;
}

void PurchaseOrderFormDialog::submit(const QString &status, QPushButton *btn)
{
    if (m_dateEdit->text().trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Missing Date"), tr("Please enter a purchase date."));
        return;
    }
    if (m_itemsTable->rowCount() == 0) {
        QMessageBox::information(this, tr("No Items"), tr("Add at least one product line."));
        return;
    }

    btn->setEnabled(false);
    m_saveDraftBtn->setEnabled(false);
    m_placeOrderBtn->setEnabled(false);

    m_api->createPurchaseOrder(buildPayload(status),
        [this](const QJsonObject &) { accept(); },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            m_saveDraftBtn->setEnabled(true);
            m_placeOrderBtn->setEnabled(true);
        });
}

void PurchaseOrderFormDialog::onAddSupplier()
{
    AddSupplierDialog dlg(m_api, this);
    if (dlg.exec() != QDialog::Accepted) return;

    const Supplier &s = dlg.createdSupplier();
    m_suppliers.append(s);
    m_supplierCombo->addItem(s.name, s.id);
    m_supplierCombo->setCurrentIndex(m_supplierCombo->count() - 1);
}

void PurchaseOrderFormDialog::onSaveDraft()  { submit(QStringLiteral("draft"),   m_saveDraftBtn);  }
void PurchaseOrderFormDialog::onPlaceOrder() { submit(QStringLiteral("ordered"), m_placeOrderBtn); }

QString PurchaseOrderFormDialog::money(double v) const
{
    const QString cur = m_bootstrap.currency;
    return QString::number(v, 'f', 2) + (cur.isEmpty() ? QString() : QStringLiteral(" ") + cur);
}

} // namespace pos
