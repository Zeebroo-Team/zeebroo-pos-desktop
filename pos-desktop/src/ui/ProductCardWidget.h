#pragma once

#include "core/Models.h"

#include <QFrame>
#include <QNetworkAccessManager>

class QLabel;
class QNetworkReply;

namespace pos {

class ProductCardWidget : public QFrame {
    Q_OBJECT
public:
    explicit ProductCardWidget(const ProductCard &product, const QString &currency,
                               QNetworkAccessManager *nam, QWidget *parent = nullptr);

    const ProductCard &product() const { return m_product; }

signals:
    void activated(const ProductCard &product);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void loadImage(const QUrl &url);

    ProductCard m_product;
    QNetworkAccessManager *m_nam = nullptr;
    QLabel *m_imageLabel = nullptr;
};

} // namespace pos
