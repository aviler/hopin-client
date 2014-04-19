#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QThread>

#include "QsLog.h"

#include "controller.h"
#include "mainwindow.h"
#include "network.h"
#include "miner.h"

Controller::Controller(QObject *parent) :
    QObject(parent)
{
    //    view(new MainWindow),
    //    network(new Network),
    //    miner(new Miner)
    view = new MainWindow(0);
    network = new Network(this);
    miner = new Miner(this);

    _minedTime = 0;
    _numThreads = 0;

    // Save this for idletime/hopins(sec) percentage
    _numThreadsMax = QThread::idealThreadCount();

    // setup UI
    view->setIdletimeSldrMax(_numThreadsMax);
    view->hideMainWdg();
    view->show();

    connect(view, &MainWindow::login, this, &Controller::login);
    connect(view, &MainWindow::start, this, &Controller::start);
    connect(view, &MainWindow::stop, this, &Controller::stop);

    //if WE wanna receive the news WE got to SIGN UP!!! ;)
    connect(network, &Network::getConfigSUCCESS, this, &Controller::getConfigSUCCESS);
    connect(network, &Network::getConfigFAIL, this, &Controller::getConfigFAIL);
    connect(network, &Network::putAliveSUCCESS, this, &Controller::putAliveSUCCESS);
    connect(network, &Network::putAliveFAIL, this, &Controller::putAliveFAIL);

    // to keep track of the time were the user REALLY mined
    connect(miner, &Miner::mineTime, this, &Controller::addMiningTime);
    connect(miner, &Miner::minerWasClosed, this, &Controller::minerWasClosed);

    //Testing Logger
    // 3. start logging
//    QLOG_INFO() << "Program started";
//    QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

//    QLOG_TRACE() << "Here's a" << QString::fromUtf8("trace") << "message";
//    QLOG_DEBUG() << "Here's a" << static_cast<int>(QsLogging::DebugLevel) << "message";
//    QLOG_WARN()  << "Uh-oh!";
//    qDebug() << "This message won't be picked up by the logger";
//    QLOG_ERROR() << "An error has occurred";
//    qWarning() << "Neither will this one";
//    QLOG_FATAL() << "Fatal error!";
//    for (int i = 0;i < 10;++i) {
//           QLOG_ERROR() << QString::fromUtf8("3 NOVO TESTE");
//       }
}

Controller::~Controller()
{
//    QLOG_INFO() << "Vai fechar";
    delete view;
    QsLogging::Logger::destroyInstance();
}

// Helper
bool Controller::parseJSON(QByteArray response, QVariantMap &json)
{
    QJsonParseError *err = new QJsonParseError();

    QJsonDocument doc = QJsonDocument::fromJson(response, err);

    if (err->error != 0) {
//        qDebug() << err->errorString();
        return false;
    }

    if (doc.isNull()) {
//        qDebug() << "Invalid JSON document, what TO DO?";


    } else if(doc.isObject()) {

        QJsonObject jObject = doc.object();
        json = jObject.toVariantMap();
        delete err;
        return true;
    }

    return false;
}

void Controller::login(const QString &email, const QString &password)
{
    //should call for login rest resource but for now we do not login just send
    //credentials, so just get config file to start cpuminer
    network->setCredentials(email, password);
    network->getConfig();
}

// When user click to start hopin generation this is called, with the selected
// number of threads
void Controller::start(int numThreads)
{
    // This will tell us the time percent to send to the server
    _numThreads = numThreads;

//    qDebug() << "(CONTROLLER) Start miner with " << numThreads << " threads.";

    // first check binaries integrity
    if( miner->checkMinerIntegrity() ) {
        // Start miner with this parameters
        miner->start(_minerParameters + " -t " + QString::number(numThreads));
        view->setMiningState();

    }else{
        view->setStatusLabel(QString("Os arquivos do hopin foram corrompidos. Reinstale o aplicativo."));
    }
}

void Controller::stop()
{
//    qDebug() << "(CONTROLLER) Stop miner";
    view->setStatusLabel("");
    miner->stop();
}

// This is not just a setter, after setting we check, if user complete 5 min of
// mining we send to the server the value to update user data.
void Controller::addMiningTime(uint time)
{
    _minedTime += time;
    if ( _minedTime > 100) {            //HEY BITCH!!! HARDCODED??? CHANGE THIS!
        uint t = _minedTime;
        _minedTime = 0;

        // Here we determine how much we are really sending to the server, cause
        // if the user is using only 50% of idle time then we also cut-off 50%
        // of his/her hopin time
        //        qDebug() << "time total: " << time;

        int rt = ( _numThreads * time ) / _numThreadsMax;
        //        qDebug() << "time percent: " << rt;

        // post this time to the server
        network->putAlive(rt);
    }
}

void Controller::getConfigSUCCESS(QByteArray response)
{
//    qDebug() << "(Controller) RECEIVED Config Success";

    // parse response
    QVariantMap json;
    if(parseJSON(response, json)){

        _minerParameters = json["parameters"].toString();

        // Update View
        // Ok we just SUPPOSE here that network have this info, CHANGE
        view->setUserEmail(network->getUserEmail());
        view->setHopinsAmount(json["hopins"].toString());
        view->hideLoginWdg();
        view->showMainWdg();

    }else{
//        qDebug() << "Couldn't parse json response";
    }
}

void Controller::getConfigFAIL()
{
//    qDebug() << "(Controller) RECEIVED Config Fail";

    // Warn user on view
    view->setLoginWarning("Falha na conexão. Tente novamente.");
    view->loginBtnSetEnabled(true);
}

void Controller::putAliveSUCCESS(QByteArray response)
{
//    qDebug() << "(Controller) RECEIVED Alive Success";

    // parse response
    QVariantMap json;
    if(parseJSON(response, json)){

        // Update View
        view->setHopinsAmount(json["hopins"].toString());

    }else{
//        qDebug() << "Couldn't parse json response";
    }
}

void Controller::putAliveFAIL()
{
//    qDebug() << "(Controller) RECEIVED Alive Fail";

    // A little "hack" here calling view slot stopBtnClicked we not only send
    // ourself a MainWindow signal to stop the miner, BUT ALSO clean up the main
    // wdg
    view->stopBtnClicked();
    //miner->stop();

    // Then we just say to MainWindow to hide one widget and show the other
    view->hideMainWdg();
    view->showLoginWdg();
}

void Controller::minerWasClosed()
{
    // miner was closed by the user OR the process was killed
    // just need to warn view about it
    view->setStoppedState();
}
