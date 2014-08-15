#include "UtilityFunctions.h"
#include <sstream>
bool Utility::CheckFileExist(string fileName)
{
	ifstream file(fileName);
	if(file.good())
	return true;
	else 
	{
		cout<<"File not found: "<<fileName<<"\n";
		return false;
	
	}
}

string Utility::inToString(int a)
{
	stringstream ss;
	ss<<a;
	string result;
	ss>>result;
	return result;
}

int Utility::ToInt(string a)
{
	stringstream ss;
	ss<<a;
	int result;
	ss>>result;
	return result;
}
GLfloat Utility::ToFloat(string a)
{
	stringstream ss;
	ss<<a;
	GLfloat result;
	ss>>result;
	return result;
}
bool Utility::ToBool(char a)
{
	if (a=='0') return false;
	else return true;
}
int Utility::ToInt(char a)
{
	stringstream ss;
	ss<<a;
	int result;
	ss>>result;
	return result;
}