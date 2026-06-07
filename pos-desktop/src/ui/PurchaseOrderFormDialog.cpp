#include "ui/PurchaseOrderFormDialog.h"

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
    setMinimumSize(720, 580);
    resize(800, 640);
    setObjectName(QStringLiteral("poFormDialog"));
    buildUi();
    loadSuppliers();
}

void PurchaseOrderFormDialog::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Header ───────────────────────────────────────────────────────────────
    auto *header = new QWidget(this);
    header->setObjectName(QStringLiteral("poDialogHeader"));
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 14, 20, 14);
    auto *titleLbl = new QLabel(tr("New Purchase Order"), header);
    titleLbl->setObjectName(QStringLiteral("poDialogTitle"));
    headerLayout->addWidget(titleLbl);
    root->addWidget(header);

    auto *divTop = new QFrame(this);
    divTop->setFrameShape(QFrame::HLine);
    divTop->setObjectName(QStringLiteral("poDivider"));
    root->addWidget(divTop);

    // ── Scroll area for form ─────────────────────────────────────────────────
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *form = new QWidget;
    auto *formLayout = new QVBoxLayout(form);
    formLayout->setContentsMargins(20, 16, 20, 16);
    formLayout->setSpacing(12);

    // ── Row 1: supplier + date + expected ────────────────────────────────────
    auto *row1 = new QHBoxLayout;
    row1->setSpacing(16);

    auto makeField = [&](const QString &labelText, QWidget *widget) -> QVBoxLayout * {
        auto *col = new QVBoxLayout;
        col->setSpacing(4);
        auto *lbl = new QLabel(labelText, form);
        lbl->setObjectName(QStringLiteral("poFormLabel"));
        col->addWidget(lbl);
        col->addWidget(widget);
        return col;
    };

    m_supplierCombo = new QComboBox(form);
    m_supplierCombo->setObjectName(QStringLiteral("poSupplierCombo"));
    m_supplierCombo->addItem(tr("— No supplier —"), 0);
    m_supplierCombo->setMinimumWidth(200);
    row1->addLayout(makeField(tr("Supplier"), m_supplierCombo), 2);

    m_dateEdit = new QLineEdit(form);
    m_dateEdit->setObjectName(QStringLiteral("poDateEdit"));
    m_dateEdit->setPlaceholderText(QStringLiteral("YYYY-MM-DD"));
    m_dateEdit->setText(QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd")));
    row1->addLayout(makeField(tr("Purchase Date *"), m_dateEdit), 1);

    m_expectedEdit = new QLineEdit(form);
    m_expectedEdit->setObjectName(QStringLiteral("poExpectedEdit"));
    m_expectedEdit->setPlaceholderText(tr("YYYY-MM-DD (optional)"));
    row1->addLayout(makeField(tr("Expected Delivery"), m_expectedEdit), 1);

    formLayout->addLayout(row1);

    // ── Notes ────────────────────────────────────────────────────────────────
    m_notesEdit = new QTextEdit(form);
    m_notesEdit->setObjectName(QStringLiteral("poNotesEdit"));
    m_notesEdit->setPlaceholderText(tr("Notes (optional)"));
    m_notesEdit->setFixedHeight(60);
    formLayout->addLayout(makeField(tr("Notes"), m_notesEdit));

    // ── Section: line items ──────────────────────────────────────────────────
    auto *itemsHeaderDiv = new QFrame(form);
    itemsHeaderDiv->setFrameShape(QFrame::HLine);
    itemsHeaderDiv->setObjectName(QStringLiteral("poDivider"));
    formLayout->addWidget(itemsHeaderDiv);

    auto *itemsLbl = new QLabel(tr("Line Items"), form);
    itemsLbl->setObjectName(QStringLiteral("detSectionLabel"));
    formLayout->addWidget(itemsLbl);

    // Add-product row
    auto *addRow = new QHBoxLayout;
    addRow->setSpacing(8);

    m_productCombo = new QComboBox(form);
    m_productCombo->setObjectName(QStringLiteral("poProductCombo"));
    m_productCombo->setEditable(true);
    m_productCombo->setInsertPolicy(QComboBox::NoInsert);
    m_productCombo->setPlaceholderText(tr("Search product…"));
    m_productCombo->setMinimumWidth(240);

    // Populate from bootstrap data
    for (const ProductCard &p : m_bootstrap.products) {
        m_productCombo->addItem(
            QStringLiteral("[%1]  %2").arg(p.sku, p.name),
            p.id);
    }
    if (m_productCombo->count() > 0) {
        m_productCombo->setCurrentIndex(-1);
        m_productCombo->clearEditText();
    }

    m_addQtyEdit = new QLineEdit(form);
    m_addQtyEdit->setObjectName(QStringLiteral("poAddQty"));
    m_addQtyEdit->setPlaceholderText(tr("Qty"));
    m_addQtyEdit->setText(QStringLiteral("1"));
    m_addQtyEdit->setFixedWidth(72);

    m_addCostEdit = new QLineEdit(form);
    m_addCostEdit->setObjectName(QStringLiteral("poAddCost"));
    m_addCostEdit->setPlaceholderText(tr("Unit cost"));
    m_addCostEdit->setText(QStringLiteral("0.00"));
    m_addCostEdit->setFixedWidth(96);

    m_addLineBtn = new QPushButton(tr("＋ Add"), form);
    m_addLineBtn->setObjectName(QStringLiteral("poAddLineBtn"));
    m_addLineBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addLineBtn, &QPushButton::clicked, this, &PurchaseOrderFormDialog::onAddLine);

    addRow->addWidget(m_productCombo, 1);
    addRow->addWidget(m_addQtyEdit);
    addRow->addWidget(m_addCostEdit);
    addRow->addWidget(m_addLineBtn);
    formLayout->addLayout(addRow);

    // Items table
    m_itemsTable = new QTableWidget(0, 6, form);
    m_itemsTable->setObjectName(QStringLiteral("detItemsTable"));
    m_itemsTable->setHorizontalHeaderLabels({tr("Product"), tr("SKU"), tr("Qty"), tr("Unit Cost"), tr("Total"), tr("")});
    m_itemsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    m_itemsTable->setColumnWidth(5, 36);
    m_itemsTable->verticalHeader()->hide();
    m_itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_itemsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_itemsTable->setAlternatingRowColors(true);
    m_itemsTable->setMinimumHeight(140);
    formLayout->addWidget(m_itemsTable);

    // Total row
    auto *totalRow = new QHBoxLayout;
    totalRow->addStretch();
    m_totalLabel = new QLabel(tr("Total:  0.00"), form);
    m_totalLabel->setObjectName(QStringLiteral("detTotalLabel"));
    totalRow->addWidget(m_totalLabel);
    formLayout->addLayout(totalRow);

    scroll->setWidget(form);
    root->addWidget(scroll, 1);

    // ── Bottom bar ───────────────────────────────────────────────────────────
    auto *divBottom = new QFrame(this);
    divBottom->setFrameShape(QFrame::HLine);
    divBottom->setObjectName(QStringLiteral("poDivider"));
    root->addWidget(divBottom);

    auto *bottomBar = new QWidget(this);
    bottomBar->setObjectName(QStringLiteral("poBottomBar"));
    auto *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(20, 10, 20, 10);

    auto *cancelBtn = new QPushButton(tr("Cancel"), bottomBar);
    cancelBtn->setObjectName(QStringLiteral("poCloseBtn"));
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_saveDraftBtn = new QPushButton(tr("Save as Draft"), bottomBar);
    m_saveDraftBtn->setObjectName(QStringLiteral("poSaveDraftBtn"));
    m_saveDraftBtn->setCursor(Qt::PointingHandCursor);
    connect(m_saveDraftBtn, &QPushButton::clicked, this, &PurchaseOrderFormDialog::onSaveDraft);

    m_placeOrderBtn = new QPushButton(tr("Save & Place Order"), bottomBar);
    m_placeOrderBtn->setObjectName(QStringLiteral("poPlaceBtn"));
    m_placeOrderBtn->setCursor(Qt::PointingHandCursor);
    m_placeOrderBtn->setDefault(true);
    connect(m_placeOrderBtn, &QPushButton::clicked, this, &PurchaseOrderFormDialog::onPlaceOrder);

    bottomLayout->addWidget(cancelBtn);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_saveDraftBtn);
    bottomLayout->addWidget(m_placeOrderBtn);
    root->addWidget(bottomBar);
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
        [](const QString &, int) {
            // Non-fatal: supplier list just stays empty
        });
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

    m_itemsTable->setItem(row, 0, new QTableWidgetItem(product.name));
    m_itemsTable->setItem(row, 1, new QTableWidgetItem(product.sku));

    auto *qtyItem = new QTableWidgetItem(QString::number(qty, 'f', 2));
    qtyItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_itemsTable->setItem(row, 2, qtyItem);

    auto *costItem = new QTableWidgetItem(QString::number(unitCost, 'f', 2));
    costItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_itemsTable->setItem(row, 3, costItem);

    const double lineTotal = qty * unitCost;
    auto *totalItem = new QTableWidgetItem(money(lineTotal));
    totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_itemsTable->setItem(row, 4, totalItem);

    // Store product id + raw values in column 0 user role for payload building
    m_itemsTable->item(row, 0)->setData(Qt::UserRole, product.id);
    m_itemsTable->item(row, 2)->setData(Qt::UserRole, qty);
    m_itemsTable->item(row, 3)->setData(Qt::UserRole, unitCost);

    auto *delBtn = new QPushButton(QStringLiteral("×"), this);
    delBtn->setObjectName(QStringLiteral("poDelLineBtn"));
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setFixedSize(28, 28);
    connect(delBtn, &QPushButton::clicked, this, [this, delBtn]() {
        for (int r = 0; r < m_itemsTable->rowCount(); ++r) {
            if (m_itemsTable->cellWidget(r, 5) == delBtn) {
                onRemoveLine(r);
                return;
            }
        }
    });
    m_itemsTable->setCellWidget(row, 5, delBtn);
    m_itemsTable->setRowHeight(row, 36);
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
        const double qty  = m_itemsTable->item(r, 2)->data(Qt::UserRole).toDouble();
        const double cost = m_itemsTable->item(r, 3)->data(Qt::UserRole).toDouble();
        total += qty * cost;
    }
    m_totalLabel->setText(tr("Total:  %1").arg(money(total)));
}

QJsonObject PurchaseOrderFormDialog::buildPayload(const QString &status) const
{
    QJsonObject payload;

    const int supplierId = m_supplierCombo->currentData().toInt();
    if (supplierId > 0) {
        payload.insert(QStringLiteral("supplier_id"), supplierId);
    }

    payload.insert(QStringLiteral("purchase_date"), m_dateEdit->text().trimmed());
    payload.insert(QStringLiteral("status"), status);

    const QString expected = m_expectedEdit->text().trimmed();
    if (!expected.isEmpty()) {
        payload.insert(QStringLiteral("expected_delivery_date"), expected);
    }

    const QString notes = m_notesEdit->toPlainText().trimmed();
    if (!notes.isEmpty()) {
        payload.insert(QStringLiteral("notes"), notes);
    }

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
        [this](const QJsonObject &) {
            accept();
        },
        [this](const QString &msg, int) {
            QMessageBox::warning(this, tr("Error"), msg);
            m_saveDraftBtn->setEnabled(true);
            m_placeOrderBtn->setEnabled(true);
        });
}

void PurchaseOrderFormDialog::onSaveDraft()
{
    submit(QStringLiteral("draft"), m_saveDraftBtn);
}

void PurchaseOrderFormDialog::onPlaceOrder()
{
    submit(QStringLiteral("ordered"), m_placeOrderBtn);
}

QString PurchaseOrderFormDialog::money(double v) const
{
    const QString cur = m_bootstrap.currency;
    return QString::number(v, 'f', 2) + (cur.isEmpty() ? QString() : QStringLiteral(" ") + cur);
}

} // namespace pos
