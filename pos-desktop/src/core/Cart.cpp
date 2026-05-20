#include "core/Cart.h"

namespace pos {

Cart::Cart(QObject *parent)
    : QObject(parent)
{
}

void Cart::clear()
{
    if (m_lines.isEmpty()) {
        return;
    }
    m_lines.clear();
    emit changed();
}

void Cart::addOrIncrement(const CartLine &line, double step)
{
    const QString key = line.cartKey();
    if (m_lines.contains(key)) {
        m_lines[key].quantity += step;
    } else {
        m_lines.insert(key, line);
    }
    emit changed();
}

void Cart::setQuantity(const QString &key, double qty)
{
    if (!m_lines.contains(key)) {
        return;
    }
    if (qty <= 0.0) {
        m_lines.remove(key);
    } else {
        m_lines[key].quantity = qty;
    }
    emit changed();
}

void Cart::removeLine(const QString &key)
{
    if (m_lines.remove(key)) {
        emit changed();
    }
}

double Cart::subtotal() const
{
    double sum = 0.0;
    for (const CartLine &line : m_lines) {
        sum += line.lineTotal();
    }
    return sum;
}

double Cart::discountAmount(double percent) const
{
    if (percent <= 0.0) {
        return 0.0;
    }
    return subtotal() * (percent / 100.0);
}

double Cart::total(double discountPercent) const
{
    return subtotal() - discountAmount(discountPercent);
}

} // namespace pos
