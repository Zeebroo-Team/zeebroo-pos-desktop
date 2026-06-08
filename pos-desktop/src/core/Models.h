#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

namespace pos {

struct StockLayer {
    int id = 0;
    QString label;
    double quantityRemaining = 0.0;
    double unitSellPrice = 0.0;
};

struct ProductCard {
    int id = 0;
    QString name;
    QString sku;
    QString imageUrl;
    double unitSellPrice = 0.0;
    double stockQuantity = 0.0;
    int layerCount = 0;
    bool requiresLayerPick = false;
    bool hasMultiplePrices = false;
    QVector<StockLayer> layers;

    static ProductCard fromJson(const QJsonObject &o);
    bool inStock() const { return stockQuantity > 0.0; }
    QString priceLabel(const QString &currency) const;
};

struct Category {
    int id = 0;
    QString name;

    static Category fromJson(const QJsonObject &o);
};

struct Branch {
    int id = 0;
    QString name;

    static Branch fromJson(const QJsonObject &o);
};

struct Account {
    int id = 0;
    QString label;

    static Account fromJson(const QJsonObject &o);
};

struct Business {
    int id = 0;
    QString name;

    static Business fromJson(const QJsonObject &o);
};

struct PosSettings {
    int defaultDepositAccountId = 0;
    bool discountFieldEnabled = false;

    static PosSettings fromJson(const QJsonObject &o);
};

struct TodaySummary {
    int onlineCount = 0;
    double onlineTotal = 0.0;

    static TodaySummary fromJson(const QJsonObject &o);
};

struct ProductsMeta {
    int currentPage = 1;
    int lastPage    = 1;
    int perPage     = 40;
    int total       = 0;
};

struct BootstrapData {
    Business business;
    QString currency;
    QVector<Category> categories;
    QVector<ProductCard> products;
    ProductsMeta productsMeta;
    QVector<Account> accounts;
    TodaySummary today;
    PosSettings settings;
    bool branchPosSeparate = false;
    bool branchProductSeparate = false;
    QVector<Branch> branches;
    int selectedBranchId = 0;

    static BootstrapData fromJson(const QJsonObject &data);
};

// ── Purchase Order models ────────────────────────────────────────────────────

struct Supplier {
    int     id = 0;
    QString name;
    QString contactName;
    QString email;
    QString phone;

    static Supplier fromJson(const QJsonObject &o);
};

struct PurchaseOrderItem {
    int     id          = 0;
    int     productId   = 0;
    QString productName;
    QString sku;
    double  quantity    = 0.0;
    double  unitCost    = 0.0;
    double  lineTotal   = 0.0;

    static PurchaseOrderItem fromJson(const QJsonObject &o);
};

struct PurchaseOrder {
    int     id           = 0;
    QString poNumber;
    QString status;
    QString statusLabel;
    int     supplierId   = 0;
    QString supplierName;
    QString purchaseDate;
    QString expectedDeliveryDate;
    QString notes;
    double  subtotal     = 0.0;
    double  total        = 0.0;
    int     itemsCount   = 0;
    QVector<PurchaseOrderItem> items;

    bool isDraft()             const { return status == QStringLiteral("draft"); }
    bool isOrdered()           const { return status == QStringLiteral("ordered"); }
    bool isPartiallyReceived() const { return status == QStringLiteral("partially_received"); }
    bool isReceived()          const { return status == QStringLiteral("received"); }
    bool isCancelled()         const { return status == QStringLiteral("cancelled"); }
    bool canPlaceOrder()       const { return isDraft(); }
    bool canReceive()          const { return !isCancelled() && !isReceived(); }
    bool canCancel()           const { return isDraft() || isOrdered(); }

    static PurchaseOrder fromJson(const QJsonObject &o);
};

// ── Sale / Return models ─────────────────────────────────────────────────────

struct SaleListItem {
    int id = 0;
    QString saleNumber;
    QString status;
    QString paymentMethod;
    double total = 0.0;
    QString soldAt;

    static SaleListItem fromJson(const QJsonObject &o);
};

struct SaleLineItem {
    int id = 0;
    QString productName;
    QString sku;
    double quantity = 0.0;
    double unitSellPrice = 0.0;
    double lineTotal = 0.0;
    double returnedQuantity = 0.0;

    double returnableQty() const;
    static SaleLineItem fromJson(const QJsonObject &o);
};

struct SaleDetail {
    int id = 0;
    QString saleNumber;
    QString status;
    double subtotal = 0.0;
    double total = 0.0;
    QString paymentMethod;
    QString soldAt;
    QVector<SaleLineItem> items;

    bool isVoid() const;
    static SaleDetail fromJson(const QJsonObject &o);
};

struct CartLine {
    int productId = 0;
    int layerId = 0;
    QString name;
    QString sku;
    QString layerLabel;
    double unitPrice = 0.0;
    double quantity = 1.0;

    QString cartKey() const;
    double lineTotal() const { return unitPrice * quantity; }
};

} // namespace pos
