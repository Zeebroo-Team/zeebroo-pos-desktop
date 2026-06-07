#pragma once

#include "core/ApiClient.h"
#include "core/Models.h"

#include <QDialog>

class QLineEdit;
class QPushButton;

namespace pos {

class AddSupplierDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(AddSupplierDialog)
public:
    explicit AddSupplierDialog(ApiClient *api, QWidget *parent = nullptr);

    Supplier createdSupplier() const { return m_created; }

private slots:
    void onSave();

private:
    void buildUi();

    ApiClient  *m_api = nullptr;
    Supplier    m_created;

    QLineEdit  *m_nameEdit        = nullptr;
    QLineEdit  *m_contactEdit     = nullptr;
    QLineEdit  *m_emailEdit       = nullptr;
    QLineEdit  *m_phoneEdit       = nullptr;
    QPushButton *m_saveBtn        = nullptr;
};

} // namespace pos
