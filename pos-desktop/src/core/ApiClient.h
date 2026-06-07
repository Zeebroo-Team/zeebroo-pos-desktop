#pragma once

#include "core/Models.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <functional>

namespace pos {

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    QString accessToken() const { return m_token; }
    int businessId() const { return m_businessId; }
    void setAccessToken(const QString &token);
    void setBusinessId(int id);

    using SuccessFn = std::function<void(const QJsonObject &)>;
    using ErrorFn = std::function<void(const QString &, int status)>;

    void login(const QString &email, const QString &password, SuccessFn onOk, ErrorFn onErr);
    void fetchBusinesses(SuccessFn onOk, ErrorFn onErr);
    void bootstrap(const QString &search, int categoryId, int page, SuccessFn onOk, ErrorFn onErr);
    void productBySku(const QString &sku, SuccessFn onOk, ErrorFn onErr);
    void checkout(const QJsonObject &body, SuccessFn onOk, ErrorFn onErr);
    void createProduct(const QJsonObject &body, SuccessFn onOk, ErrorFn onErr);
    void fetchSales(const QString &query, SuccessFn onOk, ErrorFn onErr);
    void fetchSale(int saleId, SuccessFn onOk, ErrorFn onErr);
    void processReturn(int saleId, const QJsonObject &body, SuccessFn onOk, ErrorFn onErr);

    void fetchSuppliers(SuccessFn onOk, ErrorFn onErr);
    void fetchPurchaseOrders(const QString &status, SuccessFn onOk, ErrorFn onErr);
    void fetchPurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr);
    void createPurchaseOrder(const QJsonObject &body, SuccessFn onOk, ErrorFn onErr);
    void placePurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr);
    void receivePurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr);
    void cancelPurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr);

signals:
    void unauthorized();

private:
    QNetworkRequest buildRequest(const QString &path, const QUrlQuery *query = nullptr) const;
    void sendJson(const QNetworkRequest &req, const QByteArray &body, const QString &verb,
                  SuccessFn onOk, ErrorFn onErr);
    void sendGet(const QNetworkRequest &req, SuccessFn onOk, ErrorFn onErr);
    void handleReply(QNetworkReply *reply, SuccessFn onOk, ErrorFn onErr);

    QNetworkAccessManager m_nam;
    QString m_token;
    int m_businessId = 0;
};

} // namespace pos
