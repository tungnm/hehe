#ifndef RES_REMAKE_GLOBAL_OBJECTS_H
#define RES_REMAKE_GLOBAL_OBJECTS_H

#include "Renderer.h"
#include "Pacman\Player.h"
#include "Scene.h"

Renderer * renderer1;
long lastTick=0;
long lastFunctionClick=0;
string objective;

Scene * mainScene;

PacMan player1;

PhysicalBody* box1;
PhysicalBody* box2;

//free resource
int pauseTimer;
bool isPause;
int lasttick;

#endif