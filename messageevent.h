#ifndef MESSAGEEVENT_H
#define MESSAGEEVENT_H

#include <QEvent>
#include <QString>

class MessageEvent : public QEvent
{

public:
    static const QEvent::Type MessageEventType;
    MessageEvent(): QEvent(MessageEventType) {}
    void setMessage(int type, QString message) {
        this->type = type;
        this->message = message;
    }
    QString getMessage() const {return message;}
    int getMessageType() {return type;}

private:
    int type;
    QString message;
};

#endif // MESSAGEEVENT_H
