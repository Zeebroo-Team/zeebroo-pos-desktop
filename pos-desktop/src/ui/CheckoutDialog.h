#pragma once

#include "core/Models.h"
#include <QDialog>
#include <QKeyEvent>

class QButtonGroup;
class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QWidget;

namespace pos {

class ApiClient;
class Cart;

class CheckoutDialog : public QDialog {
    Q_OBJECT
public:
    explicit CheckoutDialog(ApiClient *api, Cart *cart, const BootstrapData &bootstrap,
                            double discountPercent, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onPaymentMethodChanged(int id);
    void onNumpadKey();
    void onExactDue();
    void onClearTendered();
    void onDiscountChanged();
    void onCompleteSale();

private:
    void updatePaymentUi();
    QString money(double value) const;
    int defaultAccountId() const;

    ApiClient *m_api = nullptr;
    Cart *m_cart = nullptr;
    BootstrapData m_bootstrap;
    double m_discountPercent = 0.0;
    QString m_paymentMethod = QStringLiteral("cash");

    QButtonGroup *m_payGroup = nullptr;
    QLabel *m_summaryTotalLbl = nullptr;
    QWidget *m_cashPanel = nullptr;
    QLineEdit *m_discountEdit = nullptr;
    QLabel *m_discountAmountLbl = nullptr;
    QLabel *m_subtotalLbl = nullptr;
    QLabel *m_discountRowLbl = nullptr;
    QLineEdit *m_tenderedEdit = nullptr;
    QLabel *m_dueLabel = nullptr;
    QLabel *m_changeLabel = nullptr;
    QLabel *m_cashHint = nullptr;
    QTextEdit *m_notesEdit = nullptr;
    QPushButton *m_completeBtn = nullptr;
};

} // namespace pos
