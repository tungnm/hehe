#ifndef CONSTANT_FIL
#define CONSTANT_FIL
#include "cml/cml.h"
namespace Raptor
{
//Rendering constant:
const int W_WIDTH=800;
const int W_HEIGHT=600;
const float Z_NEAR=1.0;
const float Z_FAR=60.0;
const float CAMERA_MOVEMENT_SPEED=0.0014;

const cml::vector3f OY(0.0,1.0,0.0);
const cml::vector3f OX(1.0,0.0,0.0);
const cml::vector3f OZ(0.0,0.0,1.0);
}
#endif