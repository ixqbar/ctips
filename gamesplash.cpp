#include "gamesplash.h"

#include <QPixmap>
#include <QMouseEvent>

GameSplash::GameSplash()
{
    QPixmap pixmap(":/Resources/splash.png");
    this->setPixmap(pixmap);
}

GameSplash::~GameSplash()
{

}

void GameSplash::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}
