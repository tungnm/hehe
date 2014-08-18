#ifndef RES_REMAKE_GLOBAL_OBJECTS_H
#define RES_REMAKE_GLOBAL_OBJECTS_H

#include "RaptorEngine.h"
#include "Player.h"
#include "Scene.h"
#include <string>

Raptor::RaptorEngine * gEngine;
long lastTick=0;
long lastFunctionClick=0;
std::string objective;

Scene * mainScene;

Player player;
Monster enemy;

Raptor::PhysicalBody* xAxis;
Raptor::PhysicalBody* yAxis;
Raptor::PhysicalBody* zAxis;
//free resource
int pauseTimer;
bool isPause;
int lasttick;

#endif