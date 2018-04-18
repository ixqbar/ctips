#include "messageevent.h"

const QEvent::Type MessageEvent::MessageEventType = static_cast<QEvent::Type>(QEvent::registerEventType(QEvent::Type::User + 100));
