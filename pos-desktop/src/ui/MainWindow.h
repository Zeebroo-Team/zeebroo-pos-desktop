#pragma once

#include "core/ApiClient.h"
#include "core/Cart.h"
#include "core/Models.h"

#include <QHash>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

class QButtonGroup;
class QDoubleSpinBox;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;

namespace pos {

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(ApiClient *api, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void reloadBootstrap();
    void onCategoryClicked(int id);
    void onSearchTriggered();
    void onSkuAdd();
    void onClearCart();
    void onCheckout();
    void onReturn();
    void onAddProduct();
    void onCartChanged();

private:
    void buildUi();
    void applyBootstrap(const BootstrapData &data);
    void rebuildProductGrid();
    void rebuildCart();
    void updateSalePanelChrome();
    void updateTotals();
    void addProductToCart(const ProductCard &product);
    QPushButton *makeCategoryPill(const QString &text, int id, bool checked);
    void rebuildCategoryBar();
    QString money(double value) const;

    ApiClient *m_api = nullptr;
    QNetworkAccessManager m_imageNam;
    Cart m_cart;
    BootstrapData m_bootstrap;
    QHash<int, ProductCard> m_productsById;
    QHash<QString, ProductCard> m_productsBySku;

    int m_activeCategoryId = 0;
    int m_lastProductGridCols = 0;
    double m_discountPercent = 0.0;

    QLabel *m_brandLabel = nullptr;
    QLabel *m_statSalesLabel = nullptr;
    QLabel *m_statTotalLabel = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QLineEdit *m_skuEdit = nullptr;
    QFrame *m_categoryBar = nullptr;
    QScrollArea *m_categoryScroll = nullptr;
    QWidget *m_categoryStrip = nullptr;
    QHBoxLayout *m_categoryLayout = nullptr;
    QScrollArea *m_productScroll = nullptr;
    QWidget *m_productContainer = nullptr;
    QGridLayout *m_productGrid = nullptr;
    QFrame *m_salePanel = nullptr;
    QLabel *m_saleItemBadge = nullptr;
    QLabel *m_saleHeaderSubtotal = nullptr;
    QLabel *m_saleFooterItems = nullptr;
    QLabel *m_saleFooterSubtotal = nullptr;
    QScrollArea *m_cartScroll = nullptr;
    QWidget *m_cartContainer = nullptr;
    QVBoxLayout *m_cartLayout = nullptr;
    QWidget *m_cartEmpty = nullptr;
    QPushButton *m_clearCartBtn = nullptr;
    QPushButton *m_returnBtn = nullptr;
    QPushButton *m_checkoutBtn = nullptr;
    QLabel *m_subtotalVal = nullptr;
    QLabel *m_discountVal = nullptr;
    QLabel *m_totalVal = nullptr;
    QDoubleSpinBox *m_discountSpin = nullptr;
    QWidget *m_discountRow = nullptr;
    QButtonGroup *m_categoryGroup = nullptr;
};

} // namespace pos
