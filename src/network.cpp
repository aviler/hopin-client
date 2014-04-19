#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QVariant>

#include "network.h"
#include "requestsender.h"

Network::Network(QObject *parent) :
    QObject(parent),
    _netManager(new QNetworkAccessManager)
{

    sslSetup();

    // We deal only with SSL ERRORS
    connect(_netManager, &QNetworkAccessManager::sslErrors,
            this, &Network::sslErrors);

}

Network::~Network()
{

}

void Network::setCredentials(QString email, QString password)
{
    _userEmail = email;
    _userPassword = password;
}

// Setup ssl tunnel, TODO verify if this applies to all connections of a QNetworkAccessManager
void Network::sslSetup()
{
    QFile file(":/ssl/certificate");
    file.open(QIODevice::ReadOnly);
    const QByteArray bytes = file.readAll();

    // Create a certificate object
    const QSslCertificate certificate(bytes);

    // Add this certificate to all SSL connections
    QSslSocket::addDefaultCaCertificate(certificate);
}

void Network::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    // TODO We need to check as do something about every possible ssl error HERE

//    qDebug() << "SSL Errors: ";
    QSslError sslerror;

    foreach (sslerror, errors) {
//        qDebug() << sslerror.errorString();
    }

    reply->ignoreSslErrors();
}



void Network::getConfig()
{
    RequestSender *rs = new RequestSender(this, _netManager);
    rs->createConfigRequest(_userEmail, _userPassword);
}

void Network::putAlive(uint time)
{
    RequestSender *rs = new RequestSender(this, _netManager);
    rs->createAliveRequest(_userEmail, _userPassword, time);
}

QString Network::getUserEmail()
{
    return _userEmail;
}

void Network::configRequestSUCCESS(QByteArray response)
{
    emit getConfigSUCCESS(response);
}

void Network::configRequestFAIL()
{
    emit getConfigFAIL();
}

void Network::aliveRequestSUCCESS(QByteArray response)
{
    emit putAliveSUCCESS(response);
}

void Network::aliveRequestFAIL()
{
    emit putAliveFAIL();
}
