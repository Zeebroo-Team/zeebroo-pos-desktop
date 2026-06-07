#pragma once

#include "core/ApiClient.h"
#include "core/Models.h"

#include <QDialog>
#include <QVector>

class QComboBox;
class QDateEdit;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QTextEdit;

namespace pos {

class PurchaseOrderFormDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(PurchaseOrderFormDialog)
public:
    explicit PurchaseOrderFormDialog(ApiClient *api, const BootstrapData &bootstrap,
                                     QWidget *parent = nullptr);

private slots:
    void onAddLine();
    void onRemoveLine(int row);
    void onSaveDraft();
    void onPlaceOrder();
    void onAddSupplier();

private:
    void buildUi();
    void loadSuppliers();
    void addProductRow(const ProductCard &product, double qty, double unitCost);
    void recalcTotal();
    QJsonObject buildPayload(const QString &status) const;
    void submit(const QString &status, QPushButton *btn);
    QString money(double v) const;

    ApiClient    *m_api = nullptr;
    BootstrapData m_bootstrap;
    QVector<Supplier> m_suppliers;

    QComboBox   *m_supplierCombo    = nullptr;
    QPushButton *m_addSupplierBtn   = nullptr;
    QLineEdit   *m_dateEdit         = nullptr;
    QLineEdit   *m_expectedEdit     = nullptr;
    QTextEdit   *m_notesEdit        = nullptr;

    QComboBox   *m_productCombo     = nullptr;
    QLineEdit   *m_addQtyEdit       = nullptr;
    QLineEdit   *m_addCostEdit      = nullptr;
    QPushButton *m_addLineBtn       = nullptr;

    QTableWidget *m_itemsTable      = nullptr;
    QLabel       *m_totalLabel      = nullptr;

    QPushButton  *m_saveDraftBtn    = nullptr;
    QPushButton  *m_placeOrderBtn   = nullptr;
};

} // namespace pos
