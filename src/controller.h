#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

class MainWindow;
class Network;
class Miner;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

private:
    MainWindow *view;
    Network *network;
    Miner *miner;
    uint _minedTime;
    QString _minerParameters;
    int _numThreadsMax;
    int _numThreads;
    void loginResponse(QVariantMap response);
    bool parseJSON(QByteArray response, QVariantMap &json);

signals:

public slots:    
    void login(const QString &email, const QString &password);
    void start(int numThreads);
    void stop();
    void addMiningTime(uint time);
    void getConfigSUCCESS(QByteArray response);
    void getConfigFAIL();
    void putAliveSUCCESS(QByteArray response);
    void putAliveFAIL();
    void minerWasClosed();
};

#endif // CONTROLLER_H
