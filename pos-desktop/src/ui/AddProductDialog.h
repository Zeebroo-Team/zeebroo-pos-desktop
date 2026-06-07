#pragma once

#include "core/Models.h"
#include <QDialog>

class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace pos {

class ApiClient;

class AddProductDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddProductDialog(ApiClient *api, const QString &currency, QWidget *parent = nullptr);

    ProductCard createdProduct() const { return m_created; }

private slots:
    void onSubmit();

private:
    ApiClient *m_api;
    QString m_currency;
    ProductCard m_created;

    QLineEdit     *m_nameEdit  = nullptr;
    QLineEdit     *m_skuEdit   = nullptr;
    QDoubleSpinBox *m_priceEdit = nullptr;
    QDoubleSpinBox *m_stockEdit = nullptr;
    QPushButton   *m_submitBtn = nullptr;
    QLabel        *m_errorLbl  = nullptr;
};

} // namespace pos
