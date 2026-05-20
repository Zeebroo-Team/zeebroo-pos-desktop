#pragma once

#include "core/Models.h"

#include <QHash>
#include <QObject>

namespace pos {

class Cart : public QObject {
    Q_OBJECT
public:
    explicit Cart(QObject *parent = nullptr);

    const QHash<QString, CartLine> &lines() const { return m_lines; }
    bool isEmpty() const { return m_lines.isEmpty(); }

    void clear();
    void addOrIncrement(const CartLine &line, double step = 1.0);
    void setQuantity(const QString &key, double qty);
    void removeLine(const QString &key);

    double subtotal() const;
    double discountAmount(double percent) const;
    double total(double discountPercent) const;

signals:
    void changed();

private:
    QHash<QString, CartLine> m_lines;
};

} // namespace pos
