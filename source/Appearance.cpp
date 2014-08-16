#include "Appearance.h"
#include "Mesh.h"
#include "Texture.h"

void Appearance::UpdateModelMatrix()
{
	_modelMatrix.identity();
	//scale:
	_modelMatrix(0,0)=transform._scale[0];_modelMatrix(1,1)=transform._scale[1];_modelMatrix(2,2)=transform._scale[2];

	cml::matrix_set_translation(_modelMatrix,transform._position[0],transform._position[1],transform._position[2]);
	
	if(transform._orientation[0]!=0.0f)
	cml::matrix_rotate_about_local_x(_modelMatrix,transform._orientation[0]);
	else if(transform._orientation[1]!=0.0f)
	cml::matrix_rotate_about_local_y(_modelMatrix,transform._orientation[1]);
	else if(transform._orientation[2]!=0.0f)
	cml::matrix_rotate_about_local_z(_modelMatrix,transform._orientation[2]);

}

	void Appearance::Translate(GLfloat x,GLfloat y,GLfloat z)
	{
	
	transform._position=cml::vector3f(x,y,z);//absolute setting
	UpdateModelMatrix();
	}

	void Appearance::SetNonMoving(cml::matrix44f_c proj, cml::matrix44f_c Camera)
	{
	MV=Camera*GetModelMatrix();
	cml::matrix_linear_transform(normalTrans,MV);//transform normal to view space
	//equal to inversing and transposing the to View space matrix
	normalTrans.inverse();
	normalTrans.transpose();

	MVP=proj*MV;
	isNonMoving=true;
	}

	void Appearance::Translate( cml::vector3f offset )
	{
			transform._position+=offset;//accumulate setting
		UpdateModelMatrix();	

	}

	void Appearance::SetScale(GLfloat scaleX,GLfloat scaleY,GLfloat scaleZ)
	{
	transform._scale=cml::vector3f(scaleX,scaleY,scaleZ);
		UpdateModelMatrix();	
	}

	//void Appearance::SetFliped(bool isFlip)
	//{
	//	_flipSide=isFlip;
	//}

void Appearance::SetTexture(MapType type, std::string textureKey)
{
	if (type==MapType::diffuse)
		_material.diffuseMap=textureKey;
	else if (type==MapType::normal)
		_material.normalMap=textureKey;
	else if (type==MapType::height)
		_material.heightMap=textureKey;
	else if (type==MapType::specular)
		_material.specularMap=textureKey;
}

Appearance::Appearance(std::string mymeshKey)
{
	/*_name="noName";
	_flipSide=false;
	*/
	meshKey=mymeshKey;
	transform._position.set(0.0f,0.0f,0.0f);
	transform._orientation.zero();
	transform._scale=cml::vector3f(1.0,1.0,1.0);
	isNonMoving=false;
	UpdateModelMatrix();	
	/*isWater=false;
	textureScale=1;*/
}



cml::matrix44f_c Appearance::GetModelMatrix()
{
	return _modelMatrix;
}

void Appearance::Rotate(float angle)
{
	transform._orientation[1]=angle;
		UpdateModelMatrix();	
}

//void Appearance::SetName( string name )
//{
//	_name=name;
//}

//std::string Appearance::GetName()
//{
//	return _name;
//}

//cml::vector3f Appearance::GetPosition()
//{
//	return _position;
//}
//
//cml::vector3f Appearance::GetScale()
//{
//	return _scale;
//}
//
//GLfloat Appearance::GetOrientation()
//{
//	return _orientation;
//}
