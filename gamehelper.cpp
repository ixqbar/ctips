#include "gamehelper.h"
#include <QDebug>
#include <QNetworkInterface>


GameHelper& GameHelper::getInstance()
{
    static GameHelper instance;

    return instance;
}


QString GameHelper::getMacAddress()
{
    QString address;
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if (!(interface.flags() & QNetworkInterface::IsLoopBack)
                && (interface.flags() & QNetworkInterface::IsRunning)) {
            address.append(interface.hardwareAddress());
            break;
        }
    }

    return address;
}
