#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include "mainwindow.h"
#include "../httpserver/httprequesthandler.h"

using namespace stefanfrings;

class HttpHandler : public HttpRequestHandler
{
public:
    HttpHandler(QObject* parent=0);
    void service(HttpRequest& request, HttpResponse& response);
    void setMainWindow(MainWindow *win);
private:
    MainWindow *win;
};

#endif // HTTPHANDLER_H
