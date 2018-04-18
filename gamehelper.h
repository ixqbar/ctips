#ifndef GAMEHELPER_H
#define GAMEHELPER_H

#include <QString>

class QString;

class GameHelper
{
public:
    static GameHelper& getInstance();
    QString getMacAddress();

private:
    GameHelper(){}
    GameHelper(const GameHelper&);
    GameHelper& operator=(const GameHelper&);
};

#endif // GAMEHELPER_H
