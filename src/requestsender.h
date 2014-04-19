#ifndef REQUESTSENDER_H
#define REQUESTSENDER_H

#include <QObject>

class QNetworkAccessManager;

namespace RequestType
{
    enum Type
    {
        CONFIG = 1,
        ALIVE = 2
    };
}

class RequestSender : public QObject
{
    Q_OBJECT
public:
    explicit RequestSender(QObject *parent = 0, QNetworkAccessManager *_nam = 0);
    ~RequestSender();

    void createConfigRequest(QString email, QString password);
    void createAliveRequest(QString email, QString password, uint time);

private:
    QNetworkAccessManager *_netManager;
    RequestType::Type _self;

signals:
    void configRequestSUCCESS(QByteArray response);
    void configRequestFAIL();
    void aliveRequestSUCCESS(QByteArray response);
    void aliveRequestFAIL();

public slots:
    void finished(QNetworkReply* reply);
};

#endif // REQUESTSENDER_H
