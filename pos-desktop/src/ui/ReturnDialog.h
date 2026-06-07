#pragma once

#include "core/Models.h"
#include <QDialog>
#include <QVector>

class QButtonGroup;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QScrollArea;
class QTextEdit;
class QPushButton;
class QWidget;
class QVBoxLayout;

namespace pos {

class ApiClient;

class ReturnDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReturnDialog(ApiClient *api, const QString &currency, QWidget *parent = nullptr);

private slots:
    void onFindSale();
    void onReturnItemChanged();
    void onProcessReturn();

private:
    struct ReturnRow {
        int saleItemId = 0;
        double unitPrice = 0.0;
        double returnableQty = 0.0;
        QCheckBox *check = nullptr;
        QDoubleSpinBox *qtySpin = nullptr;
        QLabel *totalLbl = nullptr;
    };

    void showSale(const SaleDetail &sale);
    void clearItems();
    void updateRefundTotal();
    QString money(double v) const;

    ApiClient *m_api = nullptr;
    QString m_currency;
    SaleDetail m_sale;
    QVector<ReturnRow> m_rows;

    QLineEdit *m_saleSearchEdit = nullptr;
    QPushButton *m_findBtn = nullptr;
    QLabel *m_saleInfoLbl = nullptr;
    QLabel *m_errorLbl = nullptr;

    QWidget *m_itemsContainer = nullptr;
    QVBoxLayout *m_itemsLayout = nullptr;

    QButtonGroup *m_methodGroup = nullptr;
    QLineEdit *m_reasonEdit = nullptr;
    QTextEdit *m_notesEdit = nullptr;
    QLabel *m_refundTotalLbl = nullptr;

    QPushButton *m_processBtn = nullptr;
};

} // namespace pos
