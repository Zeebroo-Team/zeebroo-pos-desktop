#include "ui/ReceiptDialog.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>

namespace pos {

ReceiptDialog::ReceiptDialog(const SaleReceipt &receipt, QWidget *parent)
    : QDialog(parent)
    , m_receipt(receipt)
{
    setWindowTitle(tr("Receipt — %1").arg(receipt.saleNumber));
    setModal(true);
    resize(420, 640);
    setMinimumSize(360, 480);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    auto *heading = new QLabel(tr("Sale completed"), this);
    heading->setObjectName(QStringLiteral("receiptDialogTitle"));
    auto *sub = new QLabel(tr("Review the bill below, then print to your thermal printer."), this);
    sub->setObjectName(QStringLiteral("receiptDialogSub"));
    sub->setWordWrap(true);
    root->addWidget(heading);
    root->addWidget(sub);

    m_preview = new QTextEdit(this);
    m_preview->setObjectName(QStringLiteral("receiptPreview"));
    m_preview->setReadOnly(true);
    m_preview->setLineWrapMode(QTextEdit::WidgetWidth);
    m_preview->setFont(QFont(QStringLiteral("Courier New"), 10));
    m_preview->setPlainText(m_receipt.toPlainText(42));
    root->addWidget(m_preview, 1);

    auto *hint = new QLabel(tr("Tip: choose an 80mm thermal printer in the print dialog."), this);
    hint->setObjectName(QStringLiteral("receiptDialogHint"));
    hint->setWordWrap(true);
    root->addWidget(hint);

    auto *buttons = new QHBoxLayout();
    auto *printBtn = new QPushButton(tr("Print receipt"), this);
    printBtn->setObjectName(QStringLiteral("receiptPrintBtn"));
    printBtn->setDefault(true);
    printBtn->setCursor(Qt::PointingHandCursor);
    connect(printBtn, &QPushButton::clicked, this, &ReceiptDialog::onPrint);

    auto *closeBtn = new QPushButton(tr("Close"), this);
    closeBtn->setObjectName(QStringLiteral("receiptCloseBtn"));
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    buttons->addWidget(printBtn);
    buttons->addStretch();
    buttons->addWidget(closeBtn);
    root->addLayout(buttons);
}

void ReceiptDialog::configureThermalPrinter(QPrinter &printer) const
{
    printer.setResolution(203);
    printer.setFullPage(true);
    printer.setPageSize(QPageSize(QSizeF(80.0, 297.0), QPageSize::Millimeter, QStringLiteral("Receipt80mm")));
    printer.setPageMargins(QMarginsF(2, 2, 2, 2), QPageLayout::Millimeter);
    printer.setPageOrientation(QPageLayout::Portrait);
}

void ReceiptDialog::onPrint()
{
    QPrinter printer(QPrinter::HighResolution);
    configureThermalPrinter(printer);

    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print receipt — thermal printer"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QTextDocument doc;
    doc.setDefaultFont(QFont(QStringLiteral("Courier New"), 9));
    doc.setHtml(m_receipt.toHtml());

    const QRectF pageRect = printer.pageRect(QPrinter::Point);
    doc.setTextWidth(pageRect.width());

    doc.print(&printer);
}

} // namespace pos
