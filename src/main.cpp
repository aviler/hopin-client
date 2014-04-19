#include <QApplication>
#include <QDir>

#include "QsLog.h"
#include "QsLogDest.h"

#include "mainwindow.h"
#include "controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    using namespace QsLogging;

    // 1. init the logging mechanism
    Logger& logger = Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(QDir(a.applicationDirPath()).filePath("log.txt"));

    // 2. add two destinations
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
                                       sLogPath, EnableLogRotation, MaxSizeBytes(56320), MaxOldLogCount(0)));
    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());

    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);

    // OBEY THE CONTROLLER !!!!
    Controller c(0);

    return a.exec();
}
