#include "core/Models.h"

#include <algorithm>
#include <cmath>

namespace pos {

static QVector<StockLayer> layersFromJson(const QJsonArray &arr)
{
    QVector<StockLayer> out;
    out.reserve(arr.size());
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        StockLayer layer;
        layer.id = o.value(QStringLiteral("id")).toInt();
        layer.label = o.value(QStringLiteral("label")).toString();
        layer.quantityRemaining = o.value(QStringLiteral("quantity_remaining")).toDouble();
        layer.unitSellPrice = o.value(QStringLiteral("unit_sell_price")).toDouble();
        out.append(layer);
    }
    return out;
}

ProductCard ProductCard::fromJson(const QJsonObject &o)
{
    ProductCard p;
    p.id = o.value(QStringLiteral("id")).toInt();
    p.name = o.value(QStringLiteral("name")).toString();
    p.sku = o.value(QStringLiteral("sku")).toString();
    p.imageUrl = o.value(QStringLiteral("image_url")).toString();
    p.unitSellPrice = o.value(QStringLiteral("unit_sell_price")).toDouble();
    p.stockQuantity = o.value(QStringLiteral("stock_quantity")).toDouble();
    p.layerCount = o.value(QStringLiteral("layer_count")).toInt();
    p.requiresLayerPick = o.value(QStringLiteral("requires_layer_pick")).toBool();
    p.hasMultiplePrices = o.value(QStringLiteral("has_multiple_prices")).toBool();
    p.layers = layersFromJson(o.value(QStringLiteral("layers")).toArray());
    return p;
}

QString ProductCard::priceLabel(const QString &currency) const
{
    const QString suffix = currency.isEmpty() ? QString() : QStringLiteral(" ") + currency;
    if (hasMultiplePrices && layers.size() > 1) {
        double minP = layers.first().unitSellPrice;
        double maxP = minP;
        for (const StockLayer &layer : layers) {
            minP = std::min(minP, layer.unitSellPrice);
            maxP = std::max(maxP, layer.unitSellPrice);
        }
        return QStringLiteral("%1–%2%3")
            .arg(minP, 0, 'f', 2)
            .arg(maxP, 0, 'f', 2)
            .arg(suffix);
    }
    if (unitSellPrice > 0.0) {
        return QString::number(unitSellPrice, 'f', 2) + suffix;
    }
    return QStringLiteral("—");
}

Category Category::fromJson(const QJsonObject &o)
{
    Category c;
    c.id = o.value(QStringLiteral("id")).toInt();
    c.name = o.value(QStringLiteral("name")).toString();
    return c;
}

Account Account::fromJson(const QJsonObject &o)
{
    Account a;
    a.id = o.value(QStringLiteral("id")).toInt();
    a.label = o.value(QStringLiteral("label")).toString();
    if (a.label.isEmpty()) {
        a.label = o.value(QStringLiteral("account_name")).toString();
    }
    return a;
}

Business Business::fromJson(const QJsonObject &o)
{
    Business b;
    b.id = o.value(QStringLiteral("id")).toInt();
    b.name = o.value(QStringLiteral("name")).toString();
    return b;
}

PosSettings PosSettings::fromJson(const QJsonObject &o)
{
    PosSettings s;
    s.defaultDepositAccountId = o.value(QStringLiteral("default_deposit_account_id")).toInt();
    s.discountFieldEnabled = o.value(QStringLiteral("discount_field_enabled")).toBool();
    return s;
}

TodaySummary TodaySummary::fromJson(const QJsonObject &o)
{
    TodaySummary t;
    t.onlineCount = o.value(QStringLiteral("online_count")).toInt();
    t.onlineTotal = o.value(QStringLiteral("online_total")).toDouble();
    return t;
}

BootstrapData BootstrapData::fromJson(const QJsonObject &data)
{
    BootstrapData b;
    b.business = Business::fromJson(data.value(QStringLiteral("business")).toObject());
    b.currency = data.value(QStringLiteral("currency")).toString();
    const QJsonArray cats = data.value(QStringLiteral("categories")).toArray();
    for (const QJsonValue &v : cats) {
        b.categories.append(Category::fromJson(v.toObject()));
    }
    const QJsonArray products = data.value(QStringLiteral("products")).toArray();
    for (const QJsonValue &v : products) {
        b.products.append(ProductCard::fromJson(v.toObject()));
    }
    const QJsonArray accounts = data.value(QStringLiteral("accounts")).toArray();
    for (const QJsonValue &v : accounts) {
        b.accounts.append(Account::fromJson(v.toObject()));
    }
    b.today = TodaySummary::fromJson(data.value(QStringLiteral("today")).toObject());
    b.settings = PosSettings::fromJson(data.value(QStringLiteral("settings")).toObject());
    return b;
}

QString CartLine::cartKey() const
{
    if (layerId > 0) {
        return QStringLiteral("%1:%2").arg(productId).arg(layerId);
    }
    return QString::number(productId);
}

} // namespace pos
