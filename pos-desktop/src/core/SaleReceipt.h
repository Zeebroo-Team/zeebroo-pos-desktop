#pragma once

#include <QJsonObject>
#include <QString>
#include <QVector>

namespace pos {

struct SaleReceiptLine {
    QString name;
    QString sku;
    double quantity = 0.0;
    double unitSellPrice = 0.0;
    double lineTotal = 0.0;
};

struct SaleReceipt {
    QString businessName;
    QString currency;
    QString saleNumber;
    QString soldAtDisplay;
    QString paymentLabel;
    QString channelLabel;
    QString cashierName;
    QString notes;
    double subtotal = 0.0;
    double discountPercent = 0.0;
    double discountAmount = 0.0;
    double total = 0.0;
    double amountPaid = 0.0;
    double amountTendered = 0.0;
    double changeAmount = 0.0;
    bool isCash = false;
    QVector<SaleReceiptLine> items;

    static SaleReceipt fromApiSale(const QJsonObject &sale, const QString &businessName,
                                   const QString &currency);

    QString toPlainText(int paperChars = 42) const;
    QString toHtml() const;
};

} // namespace pos
