#pragma once

#include "core/Models.h"

#include <QDialog>

class QListWidget;

namespace pos {

class LayerPickerDialog : public QDialog {
    Q_OBJECT
public:
    explicit LayerPickerDialog(const ProductCard &product, const QString &currency, QWidget *parent = nullptr);

    StockLayer selectedLayer() const;

private:
    ProductCard m_product;
    StockLayer m_selected;
    QListWidget *m_list = nullptr;
};

} // namespace pos
