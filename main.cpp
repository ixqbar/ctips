#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "../httpserver/httplistener.h"
#include "httphandler.h"
#include "gamesplash.h"

int main(int argc, char *argv[])
{
    // http://doc.qt.io/qt-5/qtglobal.html#qSetMessagePattern
    // http://doc.qt.io/qt-5/qdatetime.html#toString
    qSetMessagePattern("[%{time yyyy-MM-dd hh:mm:ss.zzz}]%{file}(%{line}): %{message}");

    QApplication::setOrganizationDomain("ixqbar.com");
    QApplication::setOrganizationName("ixqbar");
    QApplication::setApplicationName("ctips");

    QSettings *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    QVariant urlValue = settings->value("url/default");
    if (urlValue.isNull()
            || urlValue.toString().length() == 0
            || (urlValue.toString().startsWith("ws://") == false && urlValue.toString().startsWith("wss://") == false)) {
        settings->setValue("common/url", "default");
        settings->setValue("common/enabled", false);
        settings->setValue("url/default", "ws://127.0.0.1:8899/sock?proxy=0");
        settings->setValue("http/port", 7878);
        settings->setValue("http/minThreads", 1);
    }

    qDebug() << settings->fileName();
    qDebug() << settings->allKeys();

    QApplication app(argc, argv);
    MainWindow win;

    GameSplash splash;
    splash.show();

    QTimer::singleShot(1000, [&](){
        win.start();
        splash.finish(&win);
    });

    //http server
    QSettings *listenerSettings=new QSettings(settings->fileName(), QSettings::IniFormat, &app);
    listenerSettings->beginGroup("http");
    HttpHandler *httpHandler = new HttpHandler(&app);
    httpHandler->setMainWindow(&win);

    new HttpListener(listenerSettings, httpHandler, &app);

    return app.exec();
}
