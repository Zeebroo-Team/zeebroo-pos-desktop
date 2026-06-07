#pragma once

#include "core/ApiClient.h"
#include "core/Cart.h"
#include "core/Models.h"

#include <QHash>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

class QButtonGroup;
class QDoubleSpinBox;
class QFrame;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;

namespace pos {

class PosSessionWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PosSessionWidget)
public:
    explicit PosSessionWidget(ApiClient *api, int sessionNumber, QWidget *parent = nullptr);

    int sessionNumber() const { return m_sessionNumber; }
    int cartItemCount() const { return m_cart.lines().size(); }
    QString currency() const  { return m_bootstrap.currency; }
    const BootstrapData &bootstrap() const { return m_bootstrap; }

    // Called by MainWindow keyboard shortcuts
    void focusSearch();
    void focusSku();
    void triggerAddProduct();
    void triggerRefresh();
    void triggerClearFilters();
    void triggerClearCart();
    void triggerReturn();
    void triggerCheckout();
    void navigatePreviousCategory();
    void navigateNextCategory();

signals:
    void sessionCartChanged();

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
    void onPrevPage();
    void onNextPage();

private:
    void buildUi();
    void applyBootstrap(const BootstrapData &data);
    void rebuildProductGrid();
    void rebuildCart();
    void updateSalePanelChrome();
    void updateTotals();
    void updatePaginationBar();
    void addProductToCart(const ProductCard &product);
    QPushButton *makeCategoryPill(const QString &text, int id, bool checked);
    void rebuildCategoryBar();
    QString money(double value) const;

    ApiClient *m_api = nullptr;
    int m_sessionNumber = 1;
    QNetworkAccessManager m_imageNam;
    Cart m_cart;
    BootstrapData m_bootstrap;
    QHash<int, ProductCard> m_productsById;
    QHash<QString, ProductCard> m_productsBySku;

    int m_activeCategoryId    = 0;
    int m_currentPage         = 1;
    int m_lastProductGridCols = 0;
    double m_discountPercent  = 0.0;

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

    // Pagination bar
    QWidget    *m_paginationBar  = nullptr;
    QPushButton *m_prevPageBtn   = nullptr;
    QPushButton *m_nextPageBtn   = nullptr;
    QLabel      *m_pageLabel     = nullptr;
};

} // namespace pos
