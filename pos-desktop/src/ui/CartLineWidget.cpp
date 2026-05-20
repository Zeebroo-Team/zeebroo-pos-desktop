#include "ui/CartLineWidget.h"

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace pos {

CartLineWidget::CartLineWidget(const CartLine &line, const QString &unitPriceText,
                               const QString &lineTotalText, QWidget *parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("cartLine"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    auto *top = new QHBoxLayout();
    top->setSpacing(8);

    auto *name = new QLabel(line.name, this);
    name->setObjectName(QStringLiteral("cartLineName"));
    name->setWordWrap(true);
    QFont nameFont = name->font();
    nameFont.setPointSize(12);
    nameFont.setWeight(QFont::DemiBold);
    name->setFont(nameFont);
    top->addWidget(name, 1);

    auto *lineTotal = new QLabel(lineTotalText, this);
    lineTotal->setObjectName(QStringLiteral("cartLineTotal"));
    lineTotal->setAlignment(Qt::AlignRight | Qt::AlignTop);
    QFont totalFont = lineTotal->font();
    totalFont.setPointSize(13);
    totalFont.setWeight(QFont::Bold);
    lineTotal->setFont(totalFont);
    top->addWidget(lineTotal, 0, Qt::AlignTop);
    root->addLayout(top);

    QString meta;
    if (!line.sku.isEmpty()) {
        meta = line.sku + QStringLiteral("  ·  ");
    }
    meta += unitPriceText;
    auto *metaLbl = new QLabel(meta, this);
    metaLbl->setObjectName(QStringLiteral("cartSub"));
    metaLbl->setWordWrap(true);
    root->addWidget(metaLbl);

    if (!line.layerLabel.isEmpty()) {
        auto *layerChip = new QLabel(line.layerLabel, this);
        layerChip->setObjectName(QStringLiteral("cartLayerChip"));
        layerChip->setWordWrap(true);
        root->addWidget(layerChip);
    }

    auto *bottom = new QHBoxLayout();
    bottom->setSpacing(6);

    auto *minusBtn = new QPushButton(QStringLiteral("−"), this);
    minusBtn->setObjectName(QStringLiteral("cartQtyBtn"));
    minusBtn->setFixedSize(32, 32);
    minusBtn->setCursor(Qt::PointingHandCursor);

    m_qty = new QDoubleSpinBox(this);
    m_qty->setObjectName(QStringLiteral("cartQty"));
    m_qty->setRange(0.001, 99999);
    m_qty->setDecimals(3);
    m_qty->setValue(line.quantity);
    m_qty->setAlignment(Qt::AlignCenter);
    m_qty->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_qty->setFixedWidth(72);

    auto *plusBtn = new QPushButton(QStringLiteral("+"), this);
    plusBtn->setObjectName(QStringLiteral("cartQtyBtn"));
    plusBtn->setFixedSize(32, 32);
    plusBtn->setCursor(Qt::PointingHandCursor);

    connect(minusBtn, &QPushButton::clicked, this, [this]() {
        m_qty->setValue(qMax(0.001, m_qty->value() - 1.0));
    });
    connect(plusBtn, &QPushButton::clicked, this, [this]() {
        m_qty->setValue(m_qty->value() + 1.0);
    });
    connect(m_qty, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &CartLineWidget::quantityChanged);

    bottom->addWidget(minusBtn);
    bottom->addWidget(m_qty);
    bottom->addWidget(plusBtn);
    bottom->addStretch();

    auto *rm = new QPushButton(tr("Remove"), this);
    rm->setObjectName(QStringLiteral("cartRemoveBtn"));
    rm->setCursor(Qt::PointingHandCursor);
    connect(rm, &QPushButton::clicked, this, &CartLineWidget::removeRequested);
    bottom->addWidget(rm);

    root->addLayout(bottom);
}

} // namespace pos
