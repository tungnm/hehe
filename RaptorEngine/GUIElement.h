#ifndef GUI_ELEMENT1
#define GUI_ELEMENT1
#include <string>
#include "cml\cml.h"
#include "glew.h"
#include <unordered_map>
using namespace std;


//this is a single line of string on screen, with the position of the origin
struct GUIElement
{
	string Text;
	cml::vector2f OnScreenPos;
	bool Visibility;
	GLfloat Scale;
	GLfloat condensedValue;
};

#endif