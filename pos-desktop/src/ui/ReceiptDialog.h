#pragma once

#include "core/SaleReceipt.h"

#include <QDialog>
#include <QTextEdit>

class QPrinter;

namespace pos {

class ReceiptDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReceiptDialog(const SaleReceipt &receipt, QWidget *parent = nullptr);

private slots:
    void onPrint();

private:
    void configureThermalPrinter(QPrinter &printer) const;

    SaleReceipt m_receipt;
    QTextEdit *m_preview = nullptr;
};

} // namespace pos
