#include "TextureViewer.h"

//Mesh TextureViewer::_quad;
namespace Raptor
{
TextureViewer::TextureViewer( GLfloat x,GLfloat y, GLfloat scale )
{
	_modelMatrix.identity();
	cml::matrix_set_translation(_modelMatrix,x,y,0.0f);
	//scale:
	_modelMatrix(0,0)=scale;_modelMatrix(1,1)=scale;
}

cml::matrix44f_c* TextureViewer::GetModelMatrix()
{
	return &_modelMatrix;
}
void TextureViewer::SetNewDimension(GLfloat x,GLfloat y, GLfloat scale)
{
	_modelMatrix.identity();
	cml::matrix_set_translation(_modelMatrix,x,y,0.0f);
	//scale:
	_modelMatrix(0,0)=scale;_modelMatrix(1,1)=scale;
}
}