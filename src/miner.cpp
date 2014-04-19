#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QCryptographicHash>
#include <QByteArray>

#include "miner.h"

Miner::Miner(QObject *parent) :
    QObject(parent)
{
    _mProcess = new QProcess(this);

    // Reset this current path to execute on mac, minerd that's alongside
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    connect(_mProcess, &QProcess::readyReadStandardOutput,
            this, &Miner::readyReadStandardOutput);
    connect(_mProcess, &QProcess::readyReadStandardError,
            this, &Miner::readyReadStandardError);

    // https://bugreports.qt-project.org/browse/QTBUG-30926?attachmentOrder=desc
    // look like we have to force an overload of finished, otherwise BUG
    connect(_mProcess, (void (QProcess::*)(int,QProcess::ExitStatus))&QProcess::finished,
            this, &Miner::finished);
}

Miner::~Miner()
{

    _mProcess->close();
}

void Miner::start(QString parameters)
{
    //QString arguments = "-a scrypt -o stratum+tcp://stratum.hashfaster.com:3333 -u hopin.worker -p 123mudar";
//    qDebug() << "(MINER) Starting with config:" << parameters;
    _mProcess->start("./bin/minerd " + parameters);
}

void Miner::stop()
{
//    qDebug() << "(MINER) Stoping...";
    _mProcess->close();
}

// TODO Still need to make sure that nothing comes out of this output channel
void Miner::readyReadStandardOutput(){
//    qDebug() << "OUTPUT - " << _mProcess->readAllStandardOutput();
}

bool Miner::checkMinerIntegrity()
{
    // So here based on the platform and also on the architecture
    // We verify if the minerd binaries were corrupted or not

#ifdef Q_OS_DARWIN

    QFile file("bin/minerd");

    if (file.open(QIODevice::ReadOnly)) {

        QByteArray minerd = file.readAll();

        QByteArray fileHash = QCryptographicHash::hash(minerd, QCryptographicHash::Sha512);

//        qDebug() << "HASH do file: " << QString(fileHash.toHex());

        // hash hardcoded
        QByteArray hashOSX64 = QByteArray("a0f5c279d59382d4052abecc2fc266a377bf9cff4f573a05d05acfa74895bfc0cb5315ca0f7cab2d594369a24a90e24dcf4b14a515bde03887362e57938a60b7");

        if( fileHash.toHex() == hashOSX64) {
//            qDebug() << "fileHash ORIGINAL";
            return true;
        }else{
//            qDebug() << "fileHash PIRATA";
        }

    }else{
//        qDebug() << "nao conseguiu abrir arquivo minerd";
    }

    file.close();

#else
    // UNIX code here
    // nothing to do here
#endif

    return false;

}

void Miner::readyReadStandardError()
{

    // parse log
    uint time;
    QString word;
    parseLog(_mProcess->readAllStandardError(), time, word);

    //se for palavra aceita (accepted / Stratum / thread)
    if( word == "thread" || word == "accepted:" || word == "Stratum") {

        // se flag lastWordAccepted = TRUE;
        if(_lastWordAccepted) {

            // qDebug() << _lastLogTime - time;
            // so this is a valid amount of mining and the user MUST receive HOPIN for that!
            emit mineTime( time - _lastLogTime );

        }

        _lastWordAccepted = true;

    } else {
        //se NÃO for palavra aceita (accepted / Stratum / thread)
        _lastWordAccepted = false;
    }

    // atualiza o lastTime, que registra o tempo do ultimo log
    _lastLogTime = time;
}

void Miner::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    // Warning controller that somehow the miner was closed
//    qDebug() << "Finalizou minerd - exitCode: " << exitCode << " | exitStatus: " << exitStatus;
    emit minerWasClosed();
}

// parse the 'log' from miner output and write values in 'time' and 'word'
// so the caller can use later
void Miner::parseLog(QByteArray log, uint &time, QString &word)
{
    // split the log by whitespaces
    QString line(log);
//    qDebug() << log;
    QString delimiterPattern(" ");
    QStringList list = line.split(delimiterPattern);

    // save the word
    word = list.at(2);

    // convert time in string to unix timestamp
    QString _time(list.at(1));
    _time.remove(8,1);  // removes ] char
    QTime t = QTime::fromString(_time, "hh:mm:ss");
    QDateTime fullDate(QDate::currentDate(),t);
    time = fullDate.toTime_t();
}
