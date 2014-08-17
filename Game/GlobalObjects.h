#ifndef RES_REMAKE_GLOBAL_OBJECTS_H
#define RES_REMAKE_GLOBAL_OBJECTS_H

#include "RaptorEngine.h"
#include "Player.h"
#include "Scene.h"
#include <string>
RaptorEngine * gEngine;
long lastTick=0;
long lastFunctionClick=0;
std::string objective;

Scene * mainScene;

PacMan player1;

PhysicalBody* box1;

//free resource
int pauseTimer;
bool isPause;
int lasttick;

#endif