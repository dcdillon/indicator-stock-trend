#include "Application.hpp"
#include <QApplication>
#include <iostream>
#include <dqtx/QUnixSignalHandler.hpp>

int main(int _argc, char *_argv[])
{
    QApplication application(_argc, _argv);
    Application app;
    dqtx::QUnixSignalHandler signalHandler;
    QObject::connect(&signalHandler, SIGNAL(signal(int)), &application,
                     SLOT(quit()));
    signalHandler.registerSignal(SIGINT);
    signalHandler.registerSignal(SIGHUP);
    signalHandler.registerSignal(SIGTERM);

    if (app.Initialize(_argc, _argv))
    {
        application.exec();
    }

    return 0;
}
