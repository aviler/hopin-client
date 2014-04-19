#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "requestsender.h"
#include "network.h"

RequestSender::RequestSender(QObject *parent, QNetworkAccessManager *_nam) :
    QObject(parent)
{
    _netManager = _nam;
    connect(_netManager, &QNetworkAccessManager::finished,
            this, &RequestSender::finished);

    connect(this, &RequestSender::configRequestSUCCESS,
            ((Network*)parent), &Network::configRequestSUCCESS);
    connect(this, &RequestSender::configRequestFAIL,
            ((Network*)parent), &Network::configRequestFAIL);
    connect(this, &RequestSender::aliveRequestSUCCESS,
            ((Network*)parent), &Network::aliveRequestSUCCESS);
    connect(this, &RequestSender::aliveRequestFAIL,
            ((Network*)parent), &Network::aliveRequestFAIL);

}

RequestSender::~RequestSender() {
    disconnect(_netManager, &QNetworkAccessManager::finished,
               this, &RequestSender::finished);

    disconnect(this, &RequestSender::configRequestSUCCESS,
               ((Network*)this->parent()), &Network::configRequestSUCCESS);
    disconnect(this, &RequestSender::configRequestFAIL,
               ((Network*)this->parent()), &Network::configRequestFAIL);
    disconnect(this, &RequestSender::aliveRequestSUCCESS,
               ((Network*)this->parent()), &Network::aliveRequestSUCCESS);
    disconnect(this, &RequestSender::aliveRequestFAIL,
               ((Network*)this->parent()), &Network::aliveRequestFAIL);
}

void RequestSender::createConfigRequest(QString email, QString password) {

    // for now long this is a Config Request Sender type
    _self = RequestType::CONFIG;

    // config request stuff
    QNetworkRequest request;

    request.setUrl(QUrl("https://162.243.119.122/config"));
    request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(email).arg(password).toLatin1().toBase64()));
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Content-Type", "application/json");

    // we use this info later to respond ONLY to the request we sended
    request.setOriginatingObject(this);

    _netManager->get(request);
}

void RequestSender::createAliveRequest(QString email, QString password, uint time)
{
    // for now long this is a Alive Request Sender type
    _self = RequestType::ALIVE;

    // alive request stuff
    QNetworkRequest request;

    request.setUrl(QUrl("https://162.243.119.122/alive"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(email).arg(password).toLatin1().toBase64()));
    request.setRawHeader("Accept", "application/json");

    // we use this info later to respond ONLY to the request we sended
    request.setOriginatingObject(this);

    // create data that PUT /alive HTTP1.1 is expecting, and we expect that the
    // is expecting this info from us on this resource endpoint, SEMPER HIGH ;)
    QByteArray putdata;
    QUrl param;
    QString postKey = "time";
    QString postValue = QString::number(time);
    putdata.append(postKey).append("=").append(postValue);

    _netManager->put(request,putdata);
}

void RequestSender::finished(QNetworkReply* reply) {
    if (reply->request().originatingObject() != this) {
        // That's not the request sent by the object -> stop the method here !
        return;
    }

    // First we must go trough http errors
    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCodeV.toInt() == 200) {
        // Hooray!!!!
//        qDebug() << "Http it's OK, Chief!!!";
    }

    // Other stuff that we can check out later!
//    QVariant isSecure = reply->attribute(QNetworkRequest::ConnectionEncryptedAttribute);
//    if(isSecure.toBool()) {
//        //qDebug() << "is secure? " << isSecure.toBool();
//    }

    // No error received?
    if (reply->error() == QNetworkReply::NoError) {

        // Read data from QNetworkReply here
        QByteArray bytes = reply->readAll();

        // I sended this, but what am I ?
        switch (_self)
        {
        case RequestType::CONFIG:
        {
//            qDebug() << "reply for config request SUCCESS";
            emit configRequestSUCCESS(bytes);
            break;
        }
        case RequestType::ALIVE:
        {
//            qDebug() << "reply for alive request SUCCESS";
            emit aliveRequestSUCCESS(bytes);
            break;
        }

        default:
        {
            // is likely to be an error
//            qDebug() << "Self Awareness EXCEPTION!!!!";
        }
        };

    } else {

        // Some http error received
        // Handle errors here
//        qDebug() << "HTTP error: " << reply->error();

        // I sended this, but what am I ?
        switch (_self)
        {
        case RequestType::CONFIG:
        {
//            qDebug() << "reply for config request FAIL";
            emit configRequestFAIL();
            break;
        }
        case RequestType::ALIVE:
        {
//            qDebug() << "reply for alive request FAIL";
            emit aliveRequestFAIL();
            break;
        }

        default:
        {
            // is likely to be an error
//            qDebug() << "Self Awareness EXCEPTION!!!!";
        }
        };
    }

    // We receive ownership of the reply object
    // and therefore need to handle deletion.
    reply->deleteLater();
    this->deleteLater();
}
