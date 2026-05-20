#include "ui/ProductCardWidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QNetworkReply>
#include <QPixmap>
#include <QUrl>
#include <QVBoxLayout>

namespace pos {

ProductCardWidget::ProductCardWidget(const ProductCard &product, const QString &currency,
                                     QNetworkAccessManager *nam, QWidget *parent)
    : QFrame(parent)
    , m_product(product)
    , m_nam(nam)
{
    setObjectName(QStringLiteral("productCard"));
    setCursor(product.inStock() ? Qt::PointingHandCursor : Qt::ForbiddenCursor);
    setEnabled(product.inStock());
    setMinimumSize(148, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(200);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(10, 10, 10, 10);
    lay->setSpacing(6);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setObjectName(QStringLiteral("productImage"));
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setMinimumHeight(88);
    m_imageLabel->setMaximumHeight(88);
    m_imageLabel->setScaledContents(false);
    lay->addWidget(m_imageLabel);

    if (!product.imageUrl.isEmpty() && m_nam != nullptr) {
        loadImage(QUrl(product.imageUrl));
    } else {
        m_imageLabel->setText(QStringLiteral("📦"));
        m_imageLabel->setObjectName(QStringLiteral("productPlaceholder"));
    }

    auto *name = new QLabel(product.name, this);
    name->setObjectName(QStringLiteral("productName"));
    name->setWordWrap(true);
    name->setMinimumHeight(34);
    name->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lay->addWidget(name);

    QString meta;
    if (!product.sku.isEmpty()) {
        meta = product.sku + QStringLiteral(" · ");
    }
    meta += QObject::tr("%1 in stock").arg(product.stockQuantity, 0, 'g', 3);
    if (product.layerCount > 1) {
        meta += QObject::tr(" · %1 batches").arg(product.layerCount);
    }
    auto *metaLbl = new QLabel(meta, this);
    metaLbl->setObjectName(QStringLiteral("productMeta"));
    metaLbl->setWordWrap(true);
    lay->addWidget(metaLbl);

    auto *price = new QLabel(product.priceLabel(currency), this);
    price->setObjectName(QStringLiteral("productPrice"));
    price->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(price);

    lay->addStretch(0);
}

void ProductCardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        emit activated(m_product);
        event->accept();
        return;
    }
    QFrame::mousePressEvent(event);
}

void ProductCardWidget::loadImage(const QUrl &url)
{
    if (!url.isValid() || m_nam == nullptr) {
        return;
    }

    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            m_imageLabel->setText(QStringLiteral("📦"));
            m_imageLabel->setObjectName(QStringLiteral("productPlaceholder"));
            return;
        }
        QPixmap pix;
        if (!pix.loadFromData(reply->readAll())) {
            m_imageLabel->setText(QStringLiteral("📦"));
            m_imageLabel->setObjectName(QStringLiteral("productPlaceholder"));
            return;
        }
        const QPixmap scaled = pix.scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_imageLabel->setPixmap(scaled);
        m_imageLabel->setObjectName(QStringLiteral("productImage"));
    });
}

} // namespace pos
