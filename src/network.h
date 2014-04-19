#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QSslError;

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    ~Network();
    void setCredentials(QString email, QString password);
    void getConfig();
    void putAlive(uint time);
    QString getUserEmail();

private:
    QNetworkAccessManager *_netManager;
    QString _userEmail;
    QString _userPassword;
    void sslSetup();

signals:
    void getConfigSUCCESS(QByteArray response);
    void getConfigFAIL();
    void putAliveSUCCESS(QByteArray response);
    void putAliveFAIL();
    void httpResponse(QByteArray response); //if wanna receive the news U got to SIGN UP!!! ;)

public slots:
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void configRequestSUCCESS(QByteArray response);
    void configRequestFAIL();
    void aliveRequestSUCCESS(QByteArray response);
    void aliveRequestFAIL();
};

#endif // NETWORK_H
