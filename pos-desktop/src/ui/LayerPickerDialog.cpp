#include "ui/LayerPickerDialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

namespace pos {

LayerPickerDialog::LayerPickerDialog(const ProductCard &product, const QString &currency, QWidget *parent)
    : QDialog(parent)
    , m_product(product)
{
    setWindowTitle(tr("Choose stock batch"));
    setModal(true);
    resize(420, 360);

    auto *layout = new QVBoxLayout(this);
    auto *title = new QLabel(tr("<b>%1</b><br>Select which batch to sell from.").arg(product.name), this);
    title->setWordWrap(true);
    layout->addWidget(title);

    m_list = new QListWidget(this);
    const QString suffix = currency.isEmpty() ? QString() : QStringLiteral(" ") + currency;
    for (const StockLayer &layer : product.layers) {
        const QString text = tr("%1 — %2 each (%3 left)")
                                 .arg(layer.label)
                                 .arg(layer.unitSellPrice, 0, 'f', 2)
                                 .arg(layer.quantityRemaining, 0, 'g', 3)
                             + suffix;
        auto *item = new QListWidgetItem(text, m_list);
        item->setData(Qt::UserRole, layer.id);
        item->setData(Qt::UserRole + 1, layer.unitSellPrice);
        item->setData(Qt::UserRole + 2, layer.label);
    }
    if (m_list->count() > 0) {
        m_list->setCurrentRow(0);
    }
    layout->addWidget(m_list, 1);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

StockLayer LayerPickerDialog::selectedLayer() const
{
    StockLayer layer;
    const QListWidgetItem *item = m_list->currentItem();
    if (!item) {
        return layer;
    }
    layer.id = item->data(Qt::UserRole).toInt();
    layer.unitSellPrice = item->data(Qt::UserRole + 1).toDouble();
    layer.label = item->data(Qt::UserRole + 2).toString();
    return layer;
}

} // namespace pos
