#include "Application.hpp"
#include <limits>
#include <dqtx/QSparkLineIconFactory.hpp>
#include <QTimer>
#include <QIcon>
#include <cstdint>
#include <QApplication>
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <tclap/CmdLine.h>
#include <sys/socket.h>

Application::Application()
    : m_appIndicator(
          "MarketIndicator", "", "", m_iconTheme.dir().absolutePath())
    , m_quitMenuItem("Quit")
    , m_symbol("SPY")
    , m_displaySymbol("SPY")
{
}

Application::~Application() {}

bool Application::Initialize(int _argc, char *_argv[])
{
    TCLAP::CmdLine cmdLine("Display a chart as a Unity AppIndicator.", ' ', "");
    TCLAP::ValueArg< std::string > displaySymbolArg(
        "d", "display-symbol",
        "Symbol to display as a label for the AppIndicator.", false, "",
        "STRING");
    TCLAP::UnlabeledValueArg< std::string > symbolArg(
        "symbol", "The symbol to chart", false, "SPY", "SYMBOL");

    cmdLine.add(displaySymbolArg);
    cmdLine.add(symbolArg);

    cmdLine.parse(_argc, _argv);

    m_symbol = symbolArg.getValue().c_str();
    m_displaySymbol = m_symbol;

    if (displaySymbolArg.getValue() != "")
    {
        m_displaySymbol = displaySymbolArg.getValue().c_str();
    }

    QString tmp = m_symbol.toLower();

    QTimer *timer = new QTimer(this);
    timer->setInterval(60000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();

    QObject::connect(&m_quitMenuItem, SIGNAL(menuItemActivated()), this,
                     SLOT(onQuit()));
    m_appIndicatorMenu.addMenuItem(&m_quitMenuItem);
    m_appIndicator.setMenu(&m_appIndicatorMenu);
    m_appIndicator.setLabel(m_displaySymbol);
    m_quitMenuItem.show();

    m_lastTimestamp = time(NULL);
    time_t now = m_lastTimestamp;

    struct tm tmNow;
    localtime_r(&now, &tmNow);
    tmNow.tm_hour = 0;
    tmNow.tm_min = 0;
    tmNow.tm_sec = 0;
    now = mktime(&tmNow);

    m_divisions.push_back(QPair< time_t, QColor >(now, QColor(Qt::white)));

    m_lastTimestamp -= 86400 * 2;

    readData();

    m_appIndicator.show();
    return true;
}

void Application::readData()
{
    QEventLoop eventLoop;
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), &eventLoop,
                     SLOT(quit()));
    QString address = QString(
                          "http://chartapi.finance.yahoo.com/instrument/1.0/%1/"
                          "chartdata;type=quote;range=2d/csv").arg(m_symbol);
    QUrl url(address);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QString data(reply->readAll());

        QStringList lines = data.split("\n");

        QStringList::iterator i = lines.begin();
        QStringList::iterator iend = lines.end();

        for (; i != iend; ++i)
        {
            QStringList elems = (*i).split(",");

            if (elems.size() == 6)
            {
                time_t timestamp = elems[0].toInt();
                double price = elems[1].toDouble();

                if (timestamp - m_lastTimestamp >= 3600)
                {
                    addObservation(price, timestamp);
                }
            }
        }
    }
}

void Application::onTimeout() { readData(); }

void Application::onQuit() { QApplication::quit(); }

void Application::addObservation(double _price, time_t _timestamp)
{
    static int32_t count = 0;

    if (_timestamp > m_lastTimestamp)
    {
        if (m_obs.size() == 48)
        {
            m_obs.pop_front();
        }

        m_obs.push_back(QPair< double, time_t >(_price, _timestamp));

        QIcon icon = dqtx::QSparkLineIconFactory::create(
            m_obs, QColor(Qt::gray), m_divisions, QColor(Qt::transparent),
            std::numeric_limits< double >::max(), 0, 48, 48, 0, 0, 10, 10);

        QString name = "MarketIndicator-1-" + m_symbol;

        if (count++ % 2)
        {
            name = "MarketIndicator-2-" + m_symbol;
        }

        m_iconTheme.addIcon(icon, name);
        m_appIndicator.setIconName(name);

        m_lastTimestamp = _timestamp;
    }
}
