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

    const QJsonObject discount = o.value(QStringLiteral("discount")).toObject();
    if (!discount.isEmpty()) {
        p.discountAmount = discount.value(QStringLiteral("amount")).toDouble();
        p.discountedSellPrice = discount.value(QStringLiteral("final_price")).toDouble();
    }

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
    const double effective = effectiveSellPrice();
    if (effective > 0.0) {
        return QString::number(effective, 'f', 2) + suffix;
    }
    return QStringLiteral("—");
}

QString ProductCard::priceLabelHtml(const QString &currency) const
{
    const QString suffix = currency.isEmpty() ? QString() : QStringLiteral(" ") + currency;
    if (hasMultiplePrices && layers.size() > 1) {
        double minP = layers.first().unitSellPrice;
        double maxP = minP;
        for (const StockLayer &layer : layers) {
            minP = std::min(minP, layer.unitSellPrice);
            maxP = std::max(maxP, layer.unitSellPrice);
        }
        return QStringLiteral("<b>%1–%2%3</b>")
            .arg(minP, 0, 'f', 2)
            .arg(maxP, 0, 'f', 2)
            .arg(suffix.toHtmlEscaped());
    }
    if (hasDiscount() && unitSellPrice > 0.0) {
        return QStringLiteral("<span style='text-decoration:line-through;color:#999;font-size:9pt;'>%1%3</span> "
                              "<b style='color:#16a34a;'>%2%3</b>")
            .arg(unitSellPrice, 0, 'f', 2)
            .arg(discountedSellPrice, 0, 'f', 2)
            .arg(suffix.toHtmlEscaped());
    }
    if (unitSellPrice > 0.0) {
        return QStringLiteral("<b>%1%2</b>")
            .arg(unitSellPrice, 0, 'f', 2)
            .arg(suffix.toHtmlEscaped());
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

Branch Branch::fromJson(const QJsonObject &o)
{
    Branch b;
    b.id = o.value(QStringLiteral("id")).toInt();
    b.name = o.value(QStringLiteral("name")).toString();
    return b;
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
    const QJsonObject meta = data.value(QStringLiteral("products_meta")).toObject();
    b.productsMeta.currentPage = meta.value(QStringLiteral("current_page")).toInt(1);
    b.productsMeta.lastPage    = meta.value(QStringLiteral("last_page")).toInt(1);
    b.productsMeta.perPage     = meta.value(QStringLiteral("per_page")).toInt(40);
    b.productsMeta.total       = meta.value(QStringLiteral("total")).toInt(0);
    b.today = TodaySummary::fromJson(data.value(QStringLiteral("today")).toObject());
    b.settings = PosSettings::fromJson(data.value(QStringLiteral("settings")).toObject());
    b.branchPosSeparate = data.value(QStringLiteral("branch_pos_separate")).toBool();
    b.branchProductSeparate = data.value(QStringLiteral("branch_product_separate")).toBool();
    b.selectedBranchId = data.value(QStringLiteral("selected_branch_id")).toInt(0);
    const QJsonArray branches = data.value(QStringLiteral("branches")).toArray();
    for (const QJsonValue &v : branches) {
        b.branches.append(Branch::fromJson(v.toObject()));
    }
    return b;
}

Supplier Supplier::fromJson(const QJsonObject &o)
{
    Supplier s;
    s.id          = o.value(QStringLiteral("id")).toInt();
    s.name        = o.value(QStringLiteral("name")).toString();
    s.contactName = o.value(QStringLiteral("contact_name")).toString();
    s.email       = o.value(QStringLiteral("email")).toString();
    s.phone       = o.value(QStringLiteral("phone")).toString();
    return s;
}

PurchaseOrderItem PurchaseOrderItem::fromJson(const QJsonObject &o)
{
    PurchaseOrderItem i;
    i.id          = o.value(QStringLiteral("id")).toInt();
    i.productId   = o.value(QStringLiteral("product_id")).toInt();
    i.productName = o.value(QStringLiteral("product_name")).toString();
    i.sku         = o.value(QStringLiteral("sku")).toString();
    i.quantity    = o.value(QStringLiteral("quantity")).toDouble();
    i.unitCost    = o.value(QStringLiteral("unit_cost")).toDouble();
    i.lineTotal   = o.value(QStringLiteral("line_total")).toDouble();
    return i;
}

PurchaseOrder PurchaseOrder::fromJson(const QJsonObject &o)
{
    PurchaseOrder p;
    p.id                   = o.value(QStringLiteral("id")).toInt();
    p.poNumber             = o.value(QStringLiteral("po_number")).toString();
    p.status               = o.value(QStringLiteral("status")).toString();
    p.statusLabel          = o.value(QStringLiteral("status_label")).toString();
    p.supplierId           = o.value(QStringLiteral("supplier_id")).toInt();
    p.supplierName         = o.value(QStringLiteral("supplier_name")).toString();
    p.purchaseDate         = o.value(QStringLiteral("purchase_date")).toString();
    p.expectedDeliveryDate = o.value(QStringLiteral("expected_delivery_date")).toString();
    p.notes                = o.value(QStringLiteral("notes")).toString();
    p.subtotal             = o.value(QStringLiteral("subtotal")).toDouble();
    p.total                = o.value(QStringLiteral("total")).toDouble();
    p.itemsCount           = o.value(QStringLiteral("items_count")).toInt();
    const QJsonArray items = o.value(QStringLiteral("items")).toArray();
    for (const QJsonValue &v : items) {
        p.items.append(PurchaseOrderItem::fromJson(v.toObject()));
    }
    return p;
}

SaleListItem SaleListItem::fromJson(const QJsonObject &o)
{
    SaleListItem s;
    s.id            = o.value(QStringLiteral("id")).toInt();
    s.saleNumber    = o.value(QStringLiteral("sale_number")).toString();
    s.status        = o.value(QStringLiteral("status")).toString();
    s.paymentMethod = o.value(QStringLiteral("payment_method")).toString();
    s.total         = o.value(QStringLiteral("total")).toDouble();
    s.soldAt        = o.value(QStringLiteral("sold_at")).toString();
    return s;
}

double SaleLineItem::returnableQty() const
{
    return std::max(0.0, quantity - returnedQuantity);
}

SaleLineItem SaleLineItem::fromJson(const QJsonObject &o)
{
    SaleLineItem i;
    i.id               = o.value(QStringLiteral("id")).toInt();
    i.productName      = o.value(QStringLiteral("product_name")).toString();
    i.sku              = o.value(QStringLiteral("sku")).toString();
    i.quantity         = o.value(QStringLiteral("quantity")).toDouble();
    i.unitSellPrice    = o.value(QStringLiteral("unit_sell_price")).toDouble();
    i.lineTotal        = o.value(QStringLiteral("line_total")).toDouble();
    i.returnedQuantity = o.value(QStringLiteral("returned_quantity")).toDouble();
    return i;
}

bool SaleDetail::isVoid() const
{
    return status == QLatin1String("void");
}

SaleDetail SaleDetail::fromJson(const QJsonObject &o)
{
    SaleDetail d;
    d.id            = o.value(QStringLiteral("id")).toInt();
    d.saleNumber    = o.value(QStringLiteral("sale_number")).toString();
    d.status        = o.value(QStringLiteral("status")).toString();
    d.subtotal      = o.value(QStringLiteral("subtotal")).toDouble();
    d.total         = o.value(QStringLiteral("total")).toDouble();
    d.paymentMethod = o.value(QStringLiteral("payment_method")).toString();
    d.soldAt        = o.value(QStringLiteral("sold_at")).toString();
    const QJsonArray items = o.value(QStringLiteral("items")).toArray();
    for (const QJsonValue &v : items) {
        d.items.append(SaleLineItem::fromJson(v.toObject()));
    }
    return d;
}

QString CartLine::cartKey() const
{
    if (layerId > 0) {
        return QStringLiteral("%1:%2").arg(productId).arg(layerId);
    }
    return QString::number(productId);
}

} // namespace pos
