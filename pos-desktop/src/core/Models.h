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

struct BootstrapData {
    Business business;
    QString currency;
    QVector<Category> categories;
    QVector<ProductCard> products;
    QVector<Account> accounts;
    TodaySummary today;
    PosSettings settings;

    static BootstrapData fromJson(const QJsonObject &data);
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
