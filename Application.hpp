#include <QObject>
#include <QList>
#include <QSocketNotifier>
#include <dqtx/QAppIndicator.hpp>
#include <dqtx/QIconTheme.hpp>
#include <ctime>

class Application : public QObject
{
    Q_OBJECT

   public:
    static const int8_t PROVIDER_REDIS = 0;
    static const int8_t PROVIDER_YAHOO = 1;

   protected:
    dqtx::QIconTheme m_iconTheme;
    dqtx::QAppIndicator m_appIndicator;
    dqtx::QAppIndicatorMenu m_appIndicatorMenu;
    dqtx::QAppIndicatorMenuItem m_quitMenuItem;

    time_t m_lastTimestamp;
    QList< QPair< double, time_t > > m_obs;
    QList< QPair< time_t, QColor > > m_divisions;
    QString m_symbol;
    QString m_displaySymbol;

   public:
    Application();
    ~Application();
    bool Initialize(int _argc, char *_argv[]);

   protected:
    void readData();
    void addObservation(double _price, time_t _timestamp);

   public slots:
    void onTimeout();
    void onQuit();
};
