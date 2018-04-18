#ifndef MESSAGEEVENT_H
#define MESSAGEEVENT_H

#include <QEvent>
#include <QString>

class MessageEvent : public QEvent
{

public:
    static const QEvent::Type MessageEventType;
    MessageEvent(): QEvent(MessageEventType) {}
    void setMessage(QString message) {this->message = message;}
    QString getMessage() const {return message;}

private:
    QString message;
};

#endif // MESSAGEEVENT_H
