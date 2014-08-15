#ifndef SHADER_H
#define SHADER_H
#include "glew.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include "cml\cml.h"

using namespace std;

class ShaderPass
{
public:

	void SendVec2ToUni(string uniformName, GLfloat v1,GLfloat v2);
	
	void SendVec3ToUni(string uniformName, GLfloat v1,GLfloat v2,GLfloat v3);
	void SendMat4ToUni(string uniformName, cml::matrix44f_c* mat);
	void SendMat3ToUni(string uniformName, cml::matrix33f_c* mat);
	void SendVec3ToUni(string uniformName, cml::vector3f v);

	void SendFloatArrayToUni(string uniformArrayName,int arrayCount,GLfloat* floatArray);
	void SendIntToUni(string uniformName, int value);
	void SendFloatToUni(string uniformName, float value);
	void SendBoolToUni(string uniformName, bool value);;

	ShaderPass();
	GLuint GetProgramHandle();
	void Activate();
	void InitShader(const char* vShaderFile, const char* fShaderFile);

	//send matrices from client to shader uniforms, either viewspace or worldspace
	void SendViewSpaceTransformtoUni(cml::matrix44f_c* MV,cml::matrix44f_c* MVP, cml::matrix33f_c* Normal);
	void SendWorldSpaceTransformtoUni(cml::matrix44f_c* M,cml::matrix44f_c* MVP, cml::matrix33f_c* Normal);

	//free memory by deleting the vertex, fragment shader, and deleting the program
	void Unload();
private:

	GLuint pHandle;

	//vertex and fragment shader handle will be used to delete the shader when program ends.
	GLuint vertexHandle,fragmentHandle;


};


//this function create shader object, load shader source and compile a shader for vert and frag shader



#endif