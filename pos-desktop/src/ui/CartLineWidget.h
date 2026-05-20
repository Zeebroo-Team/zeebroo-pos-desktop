#pragma once

#include "core/Models.h"

#include <QFrame>

class QDoubleSpinBox;
class QLabel;

namespace pos {

class CartLineWidget : public QFrame {
    Q_OBJECT
public:
    CartLineWidget(const CartLine &line, const QString &unitPriceText, const QString &lineTotalText,
                   QWidget *parent = nullptr);

signals:
    void quantityChanged(double quantity);
    void removeRequested();

private:
    QDoubleSpinBox *m_qty = nullptr;
};

} // namespace pos
