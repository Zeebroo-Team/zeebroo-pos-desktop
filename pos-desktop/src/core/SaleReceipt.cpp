#include "core/SaleReceipt.h"

#include <QDateTime>
#include <QJsonArray>
#include <QLocale>

namespace pos {

namespace {

QString money(double value, const QString &currency)
{
    return QLocale().toString(value, 'f', 2) + (currency.isEmpty() ? QString() : QStringLiteral(" ") + currency);
}

QString padLine(const QString &left, const QString &right, int width)
{
    const int spaces = width - left.length() - right.length();
    if (spaces >= 1) {
        return left + QString(spaces, QLatin1Char(' ')) + right;
    }
    return left + QStringLiteral(" ") + right;
}

QString centerText(const QString &text, int width)
{
    if (text.length() >= width) {
        return text.left(width);
    }
    const int pad = (width - text.length()) / 2;
    return QString(pad, QLatin1Char(' ')) + text;
}

QString repeatChar(QChar ch, int count)
{
    return QString(count, ch);
}

QString wrapText(const QString &text, int width)
{
    QString out;
    QString word;
    int lineLen = 0;
    const auto flushWord = [&]() {
        if (word.isEmpty()) {
            return;
        }
        if (lineLen > 0 && lineLen + 1 + word.length() > width) {
            out += QLatin1Char('\n');
            lineLen = 0;
        } else if (lineLen > 0) {
            out += QLatin1Char(' ');
            ++lineLen;
        }
        out += word;
        lineLen += word.length();
        word.clear();
    };

    for (const QChar c : text) {
        if (c.isSpace()) {
            flushWord();
        } else {
            word += c;
        }
    }
    flushWord();
    return out;
}

} // namespace

SaleReceipt SaleReceipt::fromApiSale(const QJsonObject &sale, const QString &businessName,
                                     const QString &currency)
{
    SaleReceipt r;
    r.businessName = businessName;
    r.currency = currency;
    r.saleNumber = sale.value(QStringLiteral("sale_number")).toString();
    r.paymentLabel = sale.value(QStringLiteral("payment_method_label")).toString();
    if (r.paymentLabel.isEmpty()) {
        r.paymentLabel = sale.value(QStringLiteral("payment_method")).toString();
    }
    r.channelLabel = sale.value(QStringLiteral("channel_label")).toString();
    r.subtotal = sale.value(QStringLiteral("subtotal")).toDouble();
    r.discountAmount = sale.value(QStringLiteral("discount_amount")).toDouble();
    if (!sale.value(QStringLiteral("discount_percent")).isNull()) {
        r.discountPercent = sale.value(QStringLiteral("discount_percent")).toDouble();
    }
    r.total = sale.value(QStringLiteral("total")).toDouble();
    r.amountPaid = sale.value(QStringLiteral("amount_paid")).toDouble();
    r.amountTendered = sale.value(QStringLiteral("amount_tendered")).toDouble();
    r.changeAmount = sale.value(QStringLiteral("change_amount")).toDouble();
    r.notes = sale.value(QStringLiteral("notes")).toString();
    r.isCash = sale.value(QStringLiteral("payment_method")).toString() == QStringLiteral("cash");

    const QJsonObject cashier = sale.value(QStringLiteral("cashier")).toObject();
    r.cashierName = cashier.value(QStringLiteral("name")).toString();

    const QString soldAtIso = sale.value(QStringLiteral("sold_at")).toString();
    const QDateTime soldAt = QDateTime::fromString(soldAtIso, Qt::ISODate);
    r.soldAtDisplay = soldAt.isValid()
        ? soldAt.toLocalTime().toString(QStringLiteral("MMM d, yyyy  h:mm AP"))
        : QString();

    const QJsonArray items = sale.value(QStringLiteral("items")).toArray();
    for (const QJsonValue &v : items) {
        const QJsonObject o = v.toObject();
        SaleReceiptLine line;
        line.name = o.value(QStringLiteral("product_name")).toString();
        line.sku = o.value(QStringLiteral("sku")).toString();
        line.quantity = o.value(QStringLiteral("quantity")).toDouble();
        line.unitSellPrice = o.value(QStringLiteral("unit_sell_price")).toDouble();
        line.lineTotal = o.value(QStringLiteral("line_total")).toDouble();
        r.items.append(line);
    }

    return r;
}

QString SaleReceipt::toPlainText(int paperChars) const
{
    const int w = qMax(32, paperChars);
    QString out;

    out += centerText(businessName.toUpper(), w) + QLatin1Char('\n');
    out += centerText(QStringLiteral("TAX INVOICE / RECEIPT"), w) + QLatin1Char('\n');
    out += repeatChar(QLatin1Char('='), w) + QLatin1Char('\n');
    out += padLine(QStringLiteral("Sale"), saleNumber, w) + QLatin1Char('\n');
    if (!soldAtDisplay.isEmpty()) {
        out += padLine(QStringLiteral("Date"), soldAtDisplay, w) + QLatin1Char('\n');
    }
    if (!cashierName.isEmpty()) {
        out += padLine(QStringLiteral("Cashier"), cashierName, w) + QLatin1Char('\n');
    }
    out += padLine(QStringLiteral("Payment"), paymentLabel, w) + QLatin1Char('\n');
    out += repeatChar(QLatin1Char('-'), w) + QLatin1Char('\n');

    for (const SaleReceiptLine &line : items) {
        const QString qtyStr = QLocale().toString(line.quantity, 'g', 3);
        const QString left = line.name + QStringLiteral(" x") + qtyStr;
        out += wrapText(left, w) + QLatin1Char('\n');
        if (!line.sku.isEmpty()) {
            out += wrapText(QStringLiteral("  ") + line.sku, w) + QLatin1Char('\n');
        }
        out += padLine(QStringLiteral("  @ ") + money(line.unitSellPrice, currency),
                       money(line.lineTotal, currency), w)
            + QLatin1Char('\n');
    }

    out += repeatChar(QLatin1Char('-'), w) + QLatin1Char('\n');
    out += padLine(QStringLiteral("Subtotal"), money(subtotal, currency), w) + QLatin1Char('\n');
    if (discountAmount > 0.0) {
        QString discLabel = QStringLiteral("Discount");
        if (discountPercent > 0.0) {
            discLabel += QStringLiteral(" (") + QLocale().toString(discountPercent, 'f', 2) + QStringLiteral("%)");
        }
        out += padLine(discLabel, money(discountAmount, currency), w) + QLatin1Char('\n');
    }
    out += padLine(QStringLiteral("TOTAL"), money(total, currency), w) + QLatin1Char('\n');

    if (isCash && amountTendered > 0.0) {
        out += padLine(QStringLiteral("Cash received"), money(amountTendered, currency), w) + QLatin1Char('\n');
        out += padLine(QStringLiteral("Change"), money(changeAmount, currency), w) + QLatin1Char('\n');
    } else if (amountPaid > 0.0) {
        out += padLine(QStringLiteral("Amount paid"), money(amountPaid, currency), w) + QLatin1Char('\n');
    }

    if (!notes.isEmpty()) {
        out += repeatChar(QLatin1Char('-'), w) + QLatin1Char('\n');
        out += QStringLiteral("Notes:") + QLatin1Char('\n');
        out += wrapText(notes, w) + QLatin1Char('\n');
    }

    out += repeatChar(QLatin1Char('='), w) + QLatin1Char('\n');
    out += centerText(QStringLiteral("Thank you!"), w) + QLatin1Char('\n');

    return out;
}

QString SaleReceipt::toHtml() const
{
    const auto htmlEsc = [](const QString &s) -> QString {
        return QString(s).toHtmlEscaped();
    };

    QString html;
    html += QStringLiteral(
        "<html><head><style>"
        "body{font-family:'Courier New',monospace;font-size:11pt;margin:0;padding:8px;}"
        "h1{font-size:13pt;text-align:center;margin:0 0 4px;}"
        "h2{font-size:10pt;text-align:center;font-weight:normal;margin:0 0 8px;color:#444;}"
        "table{width:100%;border-collapse:collapse;}"
        "td{padding:2px 0;vertical-align:top;}"
        ".r{text-align:right;}"
        ".sep{border-top:1px dashed #999;margin:6px 0;}"
        ".total td{font-weight:bold;font-size:12pt;}"
        "</style></head><body>");

    html += QStringLiteral("<h1>") + htmlEsc(businessName) + QStringLiteral("</h1>");
    html += QStringLiteral("<h2>") + htmlEsc(QStringLiteral("TAX INVOICE / RECEIPT")) + QStringLiteral("</h2>");
    html += QStringLiteral("<table>");
    html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Sale")) + QStringLiteral("</td><td class='r'>")
        + htmlEsc(saleNumber) + QStringLiteral("</td></tr>");
    if (!soldAtDisplay.isEmpty()) {
        html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Date")) + QStringLiteral("</td><td class='r'>")
            + htmlEsc(soldAtDisplay) + QStringLiteral("</td></tr>");
    }
    if (!cashierName.isEmpty()) {
        html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Cashier")) + QStringLiteral("</td><td class='r'>")
            + htmlEsc(cashierName) + QStringLiteral("</td></tr>");
    }
    html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Payment")) + QStringLiteral("</td><td class='r'>")
        + htmlEsc(paymentLabel) + QStringLiteral("</td></tr>");
    html += QStringLiteral("</table><div class='sep'></div><table>");

    for (const SaleReceiptLine &line : items) {
        const QString qtyStr = QLocale().toString(line.quantity, 'g', 3);
        html += QStringLiteral("<tr><td colspan='2'><b>") + htmlEsc(line.name) + QStringLiteral("</b> x")
            + htmlEsc(qtyStr) + QStringLiteral("</td></tr>");
        if (!line.sku.isEmpty()) {
            html += QStringLiteral("<tr><td colspan='2' style='color:#666;font-size:9pt;'>") + htmlEsc(line.sku)
                + QStringLiteral("</td></tr>");
        }
        html += QStringLiteral("<tr><td>@ ") + htmlEsc(money(line.unitSellPrice, currency))
            + QStringLiteral("</td><td class='r'>") + htmlEsc(money(line.lineTotal, currency))
            + QStringLiteral("</td></tr>");
    }

    html += QStringLiteral("</table><div class='sep'></div><table>");
    html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Subtotal")) + QStringLiteral("</td><td class='r'>")
        + htmlEsc(money(subtotal, currency)) + QStringLiteral("</td></tr>");
    if (discountAmount > 0.0) {
        html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Discount")) + QStringLiteral("</td><td class='r'>")
            + htmlEsc(money(discountAmount, currency)) + QStringLiteral("</td></tr>");
    }
    html += QStringLiteral("<tr class='total'><td>") + htmlEsc(QStringLiteral("TOTAL")) + QStringLiteral("</td><td class='r'>")
        + htmlEsc(money(total, currency)) + QStringLiteral("</td></tr>");

    if (isCash && amountTendered > 0.0) {
        html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Cash received")) + QStringLiteral("</td><td class='r'>")
            + htmlEsc(money(amountTendered, currency)) + QStringLiteral("</td></tr>");
        html += QStringLiteral("<tr><td>") + htmlEsc(QStringLiteral("Change")) + QStringLiteral("</td><td class='r'>")
            + htmlEsc(money(changeAmount, currency)) + QStringLiteral("</td></tr>");
    }

    html += QStringLiteral("</table>");

    if (!notes.isEmpty()) {
        html += QStringLiteral("<div class='sep'></div><div>") + htmlEsc(QStringLiteral("Notes:")) + QStringLiteral("<br>")
            + htmlEsc(notes) + QStringLiteral("</div>");
    }

    html += QStringLiteral("<div class='sep'></div><p style='text-align:center;margin:8px 0 0;'>")
        + htmlEsc(QStringLiteral("Thank you!")) + QStringLiteral("</p></body></html>");

    return html;
}

} // namespace pos
