#ifndef GAMESPLASH_H
#define GAMESPLASH_H

#include <QSplashScreen>

class GameSplash : public QSplashScreen
{
public:
    GameSplash();
    ~GameSplash();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // GAMESPLASH_H
