#include "core/ApiClient.h"

#include "core/Config.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

namespace pos {

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
{
}

void ApiClient::setAccessToken(const QString &token)
{
    m_token = token.trimmed();
}

void ApiClient::setBusinessId(int id)
{
    m_businessId = id;
}

void ApiClient::setBranchId(int id)
{
    m_branchId = id;
}

QNetworkRequest ApiClient::buildRequest(const QString &path, const QUrlQuery *query) const
{
    QString base = Config::instance().apiBaseUrl();
    if (!base.endsWith(QLatin1String("/api/v1/pos")) && !base.contains(QStringLiteral("/api/v1/pos"))) {
        // allow host-only config
    }
    QUrl url(base + path);
    if (query && !query->isEmpty()) {
        url.setQuery(*query);
    }
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    req.setRawHeader("Accept", "application/json");
    if (!m_token.isEmpty()) {
        req.setRawHeader("Authorization", QByteArray("Bearer ") + m_token.toUtf8());
    }
    if (m_businessId > 0) {
        req.setRawHeader("X-Business-Id", QByteArray::number(m_businessId));
    }
    return req;
}

void ApiClient::handleReply(QNetworkReply *reply, SuccessFn onOk, ErrorFn onErr)
{
    reply->deleteLater();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();

    if (status == 401) {
        emit unauthorized();
        if (onErr) {
            onErr(QStringLiteral("Session expired. Please sign in again."), status);
        }
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
    QJsonObject root = doc.isObject() ? doc.object() : QJsonObject();

    if (reply->error() != QNetworkReply::NoError || status >= 400) {
        QString msg = root.value(QStringLiteral("message")).toString();
        if (msg.isEmpty()) {
            msg = reply->errorString();
        }
        if (root.contains(QStringLiteral("errors"))) {
            const QJsonObject errors = root.value(QStringLiteral("errors")).toObject();
            QStringList parts;
            for (auto it = errors.begin(); it != errors.end(); ++it) {
                const QJsonArray arr = it.value().toArray();
                for (const QJsonValue &v : arr) {
                    parts << v.toString();
                }
            }
            if (!parts.isEmpty()) {
                msg = parts.join(QStringLiteral("\n"));
            }
        }
        if (onErr) {
            onErr(msg.isEmpty() ? QStringLiteral("Request failed.") : msg, status);
        }
        return;
    }

    if (onOk) {
        onOk(root);
    }
}

void ApiClient::sendGet(const QNetworkRequest &req, SuccessFn onOk, ErrorFn onErr)
{
    QNetworkReply *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, onOk, onErr]() {
        handleReply(reply, onOk, onErr);
    });
}

void ApiClient::sendJson(const QNetworkRequest &req, const QByteArray &body, const QString &verb,
                         SuccessFn onOk, ErrorFn onErr)
{
    QNetworkReply *reply = nullptr;
    if (verb == QStringLiteral("POST")) {
        reply = m_nam.post(req, body);
    } else if (verb == QStringLiteral("PUT")) {
        reply = m_nam.put(req, body);
    } else {
        reply = m_nam.sendCustomRequest(req, verb.toUtf8(), body);
    }
    connect(reply, &QNetworkReply::finished, this, [this, reply, onOk, onErr]() {
        handleReply(reply, onOk, onErr);
    });
}

void ApiClient::login(const QString &email, const QString &password, SuccessFn onOk, ErrorFn onErr)
{
    QJsonObject payload;
    payload.insert(QStringLiteral("email"), email.trimmed());
    payload.insert(QStringLiteral("password"), password);
    payload.insert(QStringLiteral("device_name"), Config::instance().deviceName());

    QNetworkRequest req = buildRequest(QStringLiteral("/auth/token"));
    sendJson(req, QJsonDocument(payload).toJson(), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::fetchBusinesses(SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/businesses"));
    sendGet(req, onOk, onErr);
}

void ApiClient::fetchBranches(SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/online/branches"));
    sendGet(req, onOk, onErr);
}

void ApiClient::bootstrap(const QString &search, int categoryId, int page, SuccessFn onOk, ErrorFn onErr)
{
    QUrlQuery query;
    if (!search.trimmed().isEmpty()) {
        query.addQueryItem(QStringLiteral("q"), search.trimmed());
    }
    if (categoryId > 0) {
        query.addQueryItem(QStringLiteral("category"), QString::number(categoryId));
    }
    if (page > 1) {
        query.addQueryItem(QStringLiteral("page"), QString::number(page));
    }
    if (m_branchId > 0) {
        query.addQueryItem(QStringLiteral("branch"), QString::number(m_branchId));
    }
    QNetworkRequest req = buildRequest(QStringLiteral("/online/bootstrap"), query.isEmpty() ? nullptr : &query);
    sendGet(req, onOk, onErr);
}

void ApiClient::productBySku(const QString &sku, SuccessFn onOk, ErrorFn onErr)
{
    const QString encoded = QString::fromUtf8(QUrl::toPercentEncoding(sku.trimmed()));
    QNetworkRequest req = buildRequest(QStringLiteral("/online/products/sku/") + encoded);
    sendGet(req, onOk, onErr);
}

void ApiClient::checkout(const QJsonObject &body, SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/online/checkout"));
    sendJson(req, QJsonDocument(body).toJson(), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::createProduct(const QJsonObject &body, SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/online/products"));
    sendJson(req, QJsonDocument(body).toJson(), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::fetchSales(const QString &query, SuccessFn onOk, ErrorFn onErr)
{
    QUrlQuery q;
    if (!query.trimmed().isEmpty()) {
        q.addQueryItem(QStringLiteral("q"), query.trimmed());
    }
    q.addQueryItem(QStringLiteral("channel"), QStringLiteral("online"));
    QNetworkRequest req = buildRequest(QStringLiteral("/sales"), &q);
    sendGet(req, onOk, onErr);
}

void ApiClient::fetchSale(int saleId, SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/sales/") + QString::number(saleId));
    sendGet(req, onOk, onErr);
}

void ApiClient::processReturn(int saleId, const QJsonObject &body, SuccessFn onOk, ErrorFn onErr)
{
    const QString path = QStringLiteral("/sales/") + QString::number(saleId) + QStringLiteral("/return");
    QNetworkRequest req = buildRequest(path);
    sendJson(req, QJsonDocument(body).toJson(), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::fetchSuppliers(SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/suppliers"));
    sendGet(req, onOk, onErr);
}

void ApiClient::createSupplier(const QJsonObject &body, SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/suppliers"));
    sendJson(req, QJsonDocument(body).toJson(QJsonDocument::Compact), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::fetchPurchaseOrders(const QString &status, SuccessFn onOk, ErrorFn onErr)
{
    QUrlQuery q;
    if (!status.isEmpty() && status != QStringLiteral("all")) {
        q.addQueryItem(QStringLiteral("status"), status);
    }
    QNetworkRequest req = buildRequest(QStringLiteral("/purchase-orders"), q.isEmpty() ? nullptr : &q);
    sendGet(req, onOk, onErr);
}

void ApiClient::fetchPurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/purchase-orders/") + QString::number(id));
    sendGet(req, onOk, onErr);
}

void ApiClient::createPurchaseOrder(const QJsonObject &body, SuccessFn onOk, ErrorFn onErr)
{
    QNetworkRequest req = buildRequest(QStringLiteral("/purchase-orders"));
    sendJson(req, QJsonDocument(body).toJson(), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::placePurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr)
{
    const QString path = QStringLiteral("/purchase-orders/") + QString::number(id) + QStringLiteral("/place");
    QNetworkRequest req = buildRequest(path);
    sendJson(req, QByteArray("{}"), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::receivePurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr)
{
    const QString path = QStringLiteral("/purchase-orders/") + QString::number(id) + QStringLiteral("/receive");
    QNetworkRequest req = buildRequest(path);
    sendJson(req, QByteArray("{}"), QStringLiteral("POST"), onOk, onErr);
}

void ApiClient::cancelPurchaseOrder(int id, SuccessFn onOk, ErrorFn onErr)
{
    const QString path = QStringLiteral("/purchase-orders/") + QString::number(id) + QStringLiteral("/cancel");
    QNetworkRequest req = buildRequest(path);
    sendJson(req, QByteArray("{}"), QStringLiteral("POST"), onOk, onErr);
}

} // namespace pos
