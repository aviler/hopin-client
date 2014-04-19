#ifndef MINER_H
#define MINER_H

#include <QObject>
#include <QProcess>


class Miner : public QObject
{
    Q_OBJECT
public:
    explicit Miner(QObject *parent = 0);
    ~Miner();
    void start(QString parameters);
    void stop();
    bool checkMinerIntegrity();

private:
    QProcess *_mProcess;
    bool _lastWordAccepted;
    uint _lastLogTime;
    void parseLog(QByteArray log, uint &time, QString &word);

signals:
    void mineTime(uint amount);  // sended for each new piece of time of mining
    void minerWasClosed();

public slots:
    void readyReadStandardOutput();
    void readyReadStandardError();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // MINER_H
