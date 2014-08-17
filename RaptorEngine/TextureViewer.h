#ifndef TEXTURE_VIEWER_H
#define TEXTURE_VIEWER_H
#include "glew.h"
#include "cml\cml.h"
#include "Mesh.h"
class TextureViewer
{//a texture viewer is a sub screen to see a texture. The size and position of the screen could be set by code
//a texture viewer hold 
private:
	//_textureList or a texture of a  FBO
	cml::matrix44f_c _modelMatrix; //for setting the position of the viewer on 2d screen
	//the quad that is actually used to render the texture
public:
	//static Mesh _quad; 
	TextureViewer(GLfloat x,GLfloat y, GLfloat scale);
	TextureViewer(){};//default constructor, do nothing
	
	cml::matrix44f_c* GetModelMatrix();
	void SetNewDimension(GLfloat x,GLfloat y, GLfloat scale);
	
};

#endif