#include "httphandler.h"
#include <QApplication>
#include "messageevent.h"

HttpHandler::HttpHandler(QObject* parent)
    : HttpRequestHandler(parent)
{

}

void HttpHandler::service(HttpRequest &request, HttpResponse &response)
{
    if (request.getMethod() != "POST") {
        response.write("error[only support post method]", true);
        return;
    }

    qDebug() << "httpServer received message:" << request.getBody();

    MessageEvent *msgEvent = new MessageEvent();
    msgEvent->setMessage(0, request.getBody());
    QApplication::postEvent(win, msgEvent);

    response.write("ok", true);
}

void HttpHandler::setMainWindow(MainWindow *win)
{
    this->win = win;
}
