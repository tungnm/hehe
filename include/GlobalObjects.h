#ifndef RES_REMAKE_GLOBAL_OBJECTS_H
#define RES_REMAKE_GLOBAL_OBJECTS_H

#include "Renderer.h"
#include "Pacman\Player.h"

Renderer * renderer1;
long lastTick=0;
long lastFunctionClick=0;
string objective;
PacMan player1;
Appearance* box1;
Appearance* box2;
//free resource
int pauseTimer;
bool isPause;
int lasttick;




#endif