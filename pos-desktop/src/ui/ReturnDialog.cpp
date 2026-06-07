#include "ui/ReturnDialog.h"
#include "core/ApiClient.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTextEdit>
#include <QVBoxLayout>

namespace pos {

ReturnDialog::ReturnDialog(ApiClient *api, const QString &currency, QWidget *parent)
    : QDialog(parent)
    , m_api(api)
    , m_currency(currency)
{
    setWindowTitle(tr("Process Return"));
    setFixedSize(740, 580);
    setObjectName(QStringLiteral("returnDialog"));
    setWindowModality(Qt::ApplicationModal);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    // ── Search row ────────────────────────────────────────────────────────────
    auto *searchRow = new QWidget(this);
    searchRow->setObjectName(QStringLiteral("returnSearchRow"));
    auto *searchLay = new QHBoxLayout(searchRow);
    searchLay->setContentsMargins(12, 8, 12, 8);
    searchLay->setSpacing(8);

    auto *searchLabel = new QLabel(tr("SALE NUMBER"), searchRow);
    searchLabel->setObjectName(QStringLiteral("returnFieldLabel"));

    m_saleSearchEdit = new QLineEdit(searchRow);
    m_saleSearchEdit->setPlaceholderText(tr("e.g. S-0001"));
    m_saleSearchEdit->setObjectName(QStringLiteral("returnSearchInput"));
    m_saleSearchEdit->setFixedHeight(36);
    connect(m_saleSearchEdit, &QLineEdit::returnPressed, this, &ReturnDialog::onFindSale);

    m_findBtn = new QPushButton(tr("Find Sale"), searchRow);
    m_findBtn->setObjectName(QStringLiteral("returnFindBtn"));
    m_findBtn->setFixedHeight(36);
    m_findBtn->setCursor(Qt::PointingHandCursor);
    connect(m_findBtn, &QPushButton::clicked, this, &ReturnDialog::onFindSale);

    searchLay->addWidget(searchLabel);
    searchLay->addWidget(m_saleSearchEdit, 1);
    searchLay->addWidget(m_findBtn);
    root->addWidget(searchRow);

    // ── Error / sale info labels ──────────────────────────────────────────────
    m_errorLbl = new QLabel(this);
    m_errorLbl->setObjectName(QStringLiteral("returnErrorLbl"));
    m_errorLbl->setWordWrap(true);
    m_errorLbl->setVisible(false);
    root->addWidget(m_errorLbl);

    m_saleInfoLbl = new QLabel(tr("Enter a sale number above and click Find Sale."), this);
    m_saleInfoLbl->setObjectName(QStringLiteral("returnSaleInfo"));
    root->addWidget(m_saleInfoLbl);

    // ── Two-column content area ───────────────────────────────────────────────
    auto *contentRow = new QHBoxLayout();
    contentRow->setSpacing(12);

    // Left panel — items
    auto *leftPanel = new QFrame(this);
    leftPanel->setObjectName(QStringLiteral("returnPanel"));
    leftPanel->setMinimumWidth(340);
    leftPanel->setMaximumWidth(380);
    auto *leftLay = new QVBoxLayout(leftPanel);
    leftLay->setContentsMargins(0, 0, 0, 0);
    leftLay->setSpacing(0);

    auto *itemsHeader = new QWidget(leftPanel);
    itemsHeader->setObjectName(QStringLiteral("returnPanelHeader"));
    auto *itemsHeadLay = new QHBoxLayout(itemsHeader);
    itemsHeadLay->setContentsMargins(14, 10, 14, 10);
    auto *itemsTitle = new QLabel(tr("ITEMS TO RETURN"), itemsHeader);
    itemsTitle->setObjectName(QStringLiteral("returnPanelTitle"));
    itemsHeadLay->addWidget(itemsTitle);
    leftLay->addWidget(itemsHeader);

    auto *itemsScroll = new QScrollArea(leftPanel);
    itemsScroll->setObjectName(QStringLiteral("returnItemsScroll"));
    itemsScroll->setWidgetResizable(true);
    itemsScroll->setFrameShape(QFrame::NoFrame);
    itemsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_itemsContainer = new QWidget(itemsScroll);
    m_itemsContainer->setObjectName(QStringLiteral("returnItemsContainer"));
    m_itemsLayout = new QVBoxLayout(m_itemsContainer);
    m_itemsLayout->setContentsMargins(8, 8, 8, 8);
    m_itemsLayout->setSpacing(2);
    m_itemsLayout->setAlignment(Qt::AlignTop);

    auto *placeholder = new QLabel(tr("No sale loaded."), m_itemsContainer);
    placeholder->setObjectName(QStringLiteral("returnPlaceholder"));
    placeholder->setAlignment(Qt::AlignCenter);
    m_itemsLayout->addWidget(placeholder);

    itemsScroll->setWidget(m_itemsContainer);
    leftLay->addWidget(itemsScroll, 1);
    contentRow->addWidget(leftPanel, 0);

    // Right panel — refund details
    auto *rightPanel = new QFrame(this);
    rightPanel->setObjectName(QStringLiteral("returnPanel"));
    auto *rightLay = new QVBoxLayout(rightPanel);
    rightLay->setContentsMargins(14, 14, 14, 14);
    rightLay->setSpacing(10);

    auto *methodTitle = new QLabel(tr("REFUND METHOD"), rightPanel);
    methodTitle->setObjectName(QStringLiteral("returnFieldLabel"));
    rightLay->addWidget(methodTitle);

    auto *methodRow = new QHBoxLayout();
    methodRow->setSpacing(8);
    m_methodGroup = new QButtonGroup(this);
    m_methodGroup->setExclusive(true);

    const QStringList methodLabels = {tr("Cash"), tr("Card"), tr("Credit")};
    for (int i = 0; i < methodLabels.size(); ++i) {
        auto *btn = new QPushButton(methodLabels[i], rightPanel);
        btn->setObjectName(QStringLiteral("returnMethodBtn"));
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(36);
        m_methodGroup->addButton(btn, i);
        methodRow->addWidget(btn, 1);
    }
    m_methodGroup->button(0)->setChecked(true);
    rightLay->addLayout(methodRow);

    auto *reasonTitle = new QLabel(tr("REASON"), rightPanel);
    reasonTitle->setObjectName(QStringLiteral("returnFieldLabel"));
    rightLay->addWidget(reasonTitle);

    m_reasonEdit = new QLineEdit(rightPanel);
    m_reasonEdit->setObjectName(QStringLiteral("returnTextInput"));
    m_reasonEdit->setPlaceholderText(tr("e.g. Defective, Wrong item…"));
    m_reasonEdit->setFixedHeight(36);
    rightLay->addWidget(m_reasonEdit);

    auto *notesTitle = new QLabel(tr("NOTES"), rightPanel);
    notesTitle->setObjectName(QStringLiteral("returnFieldLabel"));
    rightLay->addWidget(notesTitle);

    m_notesEdit = new QTextEdit(rightPanel);
    m_notesEdit->setObjectName(QStringLiteral("returnTextInput"));
    m_notesEdit->setFixedHeight(80);
    m_notesEdit->setPlaceholderText(tr("Optional notes…"));
    rightLay->addWidget(m_notesEdit);

    rightLay->addStretch(1);

    auto *totalFrame = new QFrame(rightPanel);
    totalFrame->setObjectName(QStringLiteral("returnTotalFrame"));
    auto *totalLay = new QVBoxLayout(totalFrame);
    totalLay->setContentsMargins(12, 10, 12, 10);
    totalLay->setSpacing(4);

    auto *totalCaption = new QLabel(tr("REFUND TOTAL"), totalFrame);
    totalCaption->setObjectName(QStringLiteral("returnTotalCaption"));
    m_refundTotalLbl = new QLabel(money(0.0), totalFrame);
    m_refundTotalLbl->setObjectName(QStringLiteral("returnTotalValue"));
    totalLay->addWidget(totalCaption);
    totalLay->addWidget(m_refundTotalLbl);
    rightLay->addWidget(totalFrame);

    contentRow->addWidget(rightPanel, 1);
    root->addLayout(contentRow, 1);

    // ── Footer ────────────────────────────────────────────────────────────────
    auto *sep = new QFrame(this);
    sep->setObjectName(QStringLiteral("returnFooterSep"));
    sep->setFrameShape(QFrame::HLine);
    root->addWidget(sep);

    auto *footerRow = new QHBoxLayout();
    footerRow->setSpacing(10);

    auto *cancelBtn = new QPushButton(tr("Cancel"), this);
    cancelBtn->setObjectName(QStringLiteral("returnCancelBtn"));
    cancelBtn->setFixedHeight(42);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_processBtn = new QPushButton(tr("Process Return  ▶"), this);
    m_processBtn->setObjectName(QStringLiteral("returnProcessBtn"));
    m_processBtn->setFixedHeight(42);
    m_processBtn->setEnabled(false);
    m_processBtn->setDefault(true);
    m_processBtn->setCursor(Qt::PointingHandCursor);
    connect(m_processBtn, &QPushButton::clicked, this, &ReturnDialog::onProcessReturn);

    footerRow->addWidget(cancelBtn);
    footerRow->addStretch(1);
    footerRow->addWidget(m_processBtn);
    root->addLayout(footerRow);
}

// ── Private helpers ───────────────────────────────────────────────────────────

QString ReturnDialog::money(double v) const
{
    return QString::number(v, 'f', 2)
           + (m_currency.isEmpty() ? QString() : QStringLiteral(" ") + m_currency);
}

void ReturnDialog::clearItems()
{
    m_sale = SaleDetail{};
    m_rows.clear();

    while (QLayoutItem *item = m_itemsLayout->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }

    auto *placeholder = new QLabel(tr("No sale loaded."), m_itemsContainer);
    placeholder->setObjectName(QStringLiteral("returnPlaceholder"));
    placeholder->setAlignment(Qt::AlignCenter);
    m_itemsLayout->addWidget(placeholder);

    m_processBtn->setEnabled(false);
    updateRefundTotal();
}

void ReturnDialog::showSale(const SaleDetail &sale)
{
    m_sale = sale;
    m_rows.clear();

    // Clear the placeholder / previous items
    while (QLayoutItem *item = m_itemsLayout->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }

    // Update sale info bar
    QString dateStr = sale.soldAt.left(10);
    m_saleInfoLbl->setText(tr("Sale %1  ·  %2  ·  %3")
        .arg(sale.saleNumber)
        .arg(dateStr)
        .arg(money(sale.total)));

    bool anyReturnable = false;

    for (const SaleLineItem &item : sale.items) {
        const double returnable = item.returnableQty();

        auto *rowWidget = new QWidget(m_itemsContainer);
        rowWidget->setObjectName(QStringLiteral("returnItemRow"));
        auto *lay = new QHBoxLayout(rowWidget);
        lay->setContentsMargins(8, 5, 8, 5);
        lay->setSpacing(6);

        ReturnRow row;
        row.saleItemId    = item.id;
        row.unitPrice     = item.unitSellPrice;
        row.returnableQty = returnable;

        row.check = new QCheckBox(rowWidget);
        row.check->setChecked(returnable > 0.001);
        row.check->setEnabled(returnable > 0.001);
        lay->addWidget(row.check);

        // Name + returned sub-label
        auto *nameCol = new QVBoxLayout();
        nameCol->setSpacing(1);
        auto *nameLbl = new QLabel(item.productName, rowWidget);
        nameLbl->setObjectName(QStringLiteral("returnItemName"));
        nameCol->addWidget(nameLbl);
        if (item.returnedQuantity > 0.001) {
            auto *retLbl = new QLabel(tr("Already returned: %1")
                .arg(QString::number(item.returnedQuantity, 'f', 2)), rowWidget);
            retLbl->setObjectName(QStringLiteral("returnItemSub"));
            nameCol->addWidget(retLbl);
        }
        lay->addLayout(nameCol, 1);

        // "×N" available qty label
        auto *availLbl = new QLabel(
            QChar(0xD7) + QString::number(returnable, 'f', returnable == (int)returnable ? 0 : 2),
            rowWidget);
        availLbl->setObjectName(QStringLiteral("returnItemQtyLbl"));
        availLbl->setFixedWidth(38);
        availLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lay->addWidget(availLbl);

        // Return quantity spinbox
        row.qtySpin = new QDoubleSpinBox(rowWidget);
        row.qtySpin->setRange(0.0, returnable);
        row.qtySpin->setValue(returnable > 0.001 ? returnable : 0.0);
        row.qtySpin->setSingleStep(1.0);
        row.qtySpin->setDecimals(2);
        row.qtySpin->setFixedWidth(72);
        row.qtySpin->setEnabled(returnable > 0.001);
        lay->addWidget(row.qtySpin);

        // Line total
        row.totalLbl = new QLabel(money(returnable > 0.001 ? returnable * item.unitSellPrice : 0.0), rowWidget);
        row.totalLbl->setObjectName(QStringLiteral("returnItemTotal"));
        row.totalLbl->setFixedWidth(72);
        row.totalLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lay->addWidget(row.totalLbl);

        m_itemsLayout->addWidget(rowWidget);
        m_rows.append(row);
        const int rowIdx = m_rows.size() - 1;

        connect(m_rows[rowIdx].check, &QCheckBox::toggled, this, [this, rowIdx](bool checked) {
            m_rows[rowIdx].qtySpin->setEnabled(checked);
            m_rows[rowIdx].qtySpin->setValue(checked ? m_rows[rowIdx].returnableQty : 0.0);
            onReturnItemChanged();
        });
        connect(m_rows[rowIdx].qtySpin,
                QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &ReturnDialog::onReturnItemChanged);

        if (returnable > 0.001) {
            anyReturnable = true;
        }
    }

    if (!anyReturnable) {
        auto *noRet = new QLabel(tr("All items have already been fully returned."), m_itemsContainer);
        noRet->setObjectName(QStringLiteral("returnPlaceholder"));
        noRet->setAlignment(Qt::AlignCenter);
        noRet->setWordWrap(true);
        m_itemsLayout->addWidget(noRet);
    }

    m_itemsLayout->addStretch(1);
    updateRefundTotal();
}

void ReturnDialog::updateRefundTotal()
{
    double total = 0.0;
    bool anySelected = false;

    for (const ReturnRow &row : m_rows) {
        if (row.check && row.check->isChecked() && row.qtySpin && row.qtySpin->value() > 0.001) {
            total += row.qtySpin->value() * row.unitPrice;
            anySelected = true;
        }
    }

    m_refundTotalLbl->setText(money(total));
    m_processBtn->setEnabled(anySelected && m_sale.id > 0);
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void ReturnDialog::onFindSale()
{
    const QString q = m_saleSearchEdit->text().trimmed();
    if (q.isEmpty()) {
        m_errorLbl->setText(tr("Please enter a sale number."));
        m_errorLbl->setVisible(true);
        return;
    }

    m_findBtn->setEnabled(false);
    m_findBtn->setText(tr("Searching…"));
    m_errorLbl->setVisible(false);
    clearItems();
    m_saleInfoLbl->setText(tr("Searching…"));

    m_api->fetchSales(q,
        [this](const QJsonObject &root) {
            const QJsonArray data = root.value(QStringLiteral("data")).toArray();

            if (data.isEmpty()) {
                m_findBtn->setEnabled(true);
                m_findBtn->setText(tr("Find Sale"));
                m_saleInfoLbl->setText(tr("No sale found."));
                m_errorLbl->setText(tr("No sale found for \"%1\".").arg(m_saleSearchEdit->text().trimmed()));
                m_errorLbl->setVisible(true);
                return;
            }

            const int saleId = data.first().toObject().value(QStringLiteral("id")).toInt();

            m_api->fetchSale(saleId,
                [this](const QJsonObject &detailRoot) {
                    m_findBtn->setEnabled(true);
                    m_findBtn->setText(tr("Find Sale"));

                    const SaleDetail sale = SaleDetail::fromJson(
                        detailRoot.value(QStringLiteral("data")).toObject());

                    if (sale.isVoid()) {
                        m_saleInfoLbl->setText(tr("—"));
                        m_errorLbl->setText(tr("This sale has been voided and cannot be returned."));
                        m_errorLbl->setVisible(true);
                        return;
                    }

                    m_errorLbl->setVisible(false);
                    showSale(sale);
                },
                [this](const QString &msg, int) {
                    m_findBtn->setEnabled(true);
                    m_findBtn->setText(tr("Find Sale"));
                    m_saleInfoLbl->setText(tr("—"));
                    m_errorLbl->setText(msg);
                    m_errorLbl->setVisible(true);
                }
            );
        },
        [this](const QString &msg, int) {
            m_findBtn->setEnabled(true);
            m_findBtn->setText(tr("Find Sale"));
            m_saleInfoLbl->setText(tr("—"));
            m_errorLbl->setText(msg);
            m_errorLbl->setVisible(true);
        }
    );
}

void ReturnDialog::onReturnItemChanged()
{
    for (ReturnRow &row : m_rows) {
        if (row.totalLbl) {
            const double qty = (row.check && row.check->isChecked() && row.qtySpin)
                               ? row.qtySpin->value()
                               : 0.0;
            row.totalLbl->setText(money(qty * row.unitPrice));
        }
    }
    updateRefundTotal();
}

void ReturnDialog::onProcessReturn()
{
    QJsonArray items;
    for (const ReturnRow &row : m_rows) {
        if (!row.check || !row.check->isChecked()) continue;
        if (!row.qtySpin || row.qtySpin->value() < 0.001) continue;
        QJsonObject itemObj;
        itemObj.insert(QStringLiteral("sale_item_id"), row.saleItemId);
        itemObj.insert(QStringLiteral("quantity"), row.qtySpin->value());
        items.append(itemObj);
    }

    if (items.isEmpty()) {
        m_errorLbl->setText(tr("Select at least one item with a quantity greater than zero."));
        m_errorLbl->setVisible(true);
        return;
    }

    const QString refundMethod = [this]() -> QString {
        switch (m_methodGroup->checkedId()) {
        case 1:  return QStringLiteral("card");
        case 2:  return QStringLiteral("credit");
        default: return QStringLiteral("cash");
        }
    }();

    QJsonObject body;
    body.insert(QStringLiteral("items"), items);
    body.insert(QStringLiteral("refund_method"), refundMethod);
    const QString reason = m_reasonEdit->text().trimmed();
    if (!reason.isEmpty()) {
        body.insert(QStringLiteral("refund_reason"), reason);
    }
    const QString notes = m_notesEdit->toPlainText().trimmed();
    if (!notes.isEmpty()) {
        body.insert(QStringLiteral("notes"), notes);
    }

    m_processBtn->setEnabled(false);
    m_findBtn->setEnabled(false);
    m_errorLbl->setVisible(false);

    m_api->processReturn(m_sale.id, body,
        [this](const QJsonObject &resp) {
            const QJsonObject data = resp.value(QStringLiteral("data")).toObject();
            const QString returnNum = data.value(QStringLiteral("return_number")).toString();
            const double refundTotal = data.value(QStringLiteral("total")).toDouble();

            QMessageBox::information(this,
                tr("Return Processed"),
                tr("Return %1 processed.\nRefund: %2")
                    .arg(returnNum.isEmpty() ? QString() : QStringLiteral("#") + returnNum)
                    .arg(money(refundTotal)));
            accept();
        },
        [this](const QString &msg, int) {
            m_errorLbl->setText(msg);
            m_errorLbl->setVisible(true);
            m_processBtn->setEnabled(true);
            m_findBtn->setEnabled(true);
        }
    );
}

} // namespace pos
