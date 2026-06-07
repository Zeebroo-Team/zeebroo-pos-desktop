#pragma once

#include "core/ApiClient.h"
#include "core/Models.h"

#include <QDialog>
#include <QVector>

class QComboBox;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;
class QTableWidget;

namespace pos {

class PurchaseOrderFormDialog;

class PurchaseOrderDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(PurchaseOrderDialog)
public:
    explicit PurchaseOrderDialog(ApiClient *api, const BootstrapData &bootstrap,
                                 QWidget *parent = nullptr);

private slots:
    void onFilterChanged();
    void onListItemClicked(QListWidgetItem *item);
    void onNewPo();
    void onPlaceOrder();
    void onReceive();
    void onCancelPo();
    void refreshList();
    void loadDetail(int poId);

private:
    void buildUi();
    void populateList(const QVector<PurchaseOrder> &orders);
    void showDetail(const PurchaseOrder &po);
    void showDetailPlaceholder();
    void setDetailButtons(const PurchaseOrder &po);
    QString statusBadge(const PurchaseOrder &po) const;
    QString money(double v) const;

    ApiClient    *m_api = nullptr;
    BootstrapData m_bootstrap;

    QVector<PurchaseOrder> m_orders;
    int m_selectedPoId = 0;
    PurchaseOrder m_selectedPo;

    // Left panel
    QComboBox    *m_filterCombo  = nullptr;
    QListWidget  *m_list         = nullptr;
    QPushButton  *m_newPoBtn     = nullptr;
    QPushButton  *m_refreshBtn   = nullptr;

    // Right panel
    QStackedWidget *m_stack         = nullptr;

    // Detail widgets (page 1 of stack)
    QLabel       *m_detPoNumber     = nullptr;
    QLabel       *m_detStatus       = nullptr;
    QLabel       *m_detSupplier     = nullptr;
    QLabel       *m_detDate         = nullptr;
    QLabel       *m_detExpected     = nullptr;
    QLabel       *m_detNotes        = nullptr;
    QTableWidget *m_detItemsTable   = nullptr;
    QLabel       *m_detTotal        = nullptr;
    QPushButton  *m_placeBtn        = nullptr;
    QPushButton  *m_receiveBtn      = nullptr;
    QPushButton  *m_cancelPoBtn     = nullptr;
};

} // namespace pos
