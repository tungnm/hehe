#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <string>
#include <fstream>
#include "glew.h"
#include <sstream>
using namespace std;

namespace Utility
{
bool CheckFileExist(string fileName);

int ToInt(string a);
GLfloat ToFloat(string a);
bool ToBool(char a);
int ToInt(char a);
string inToString(int a);


}



#endif