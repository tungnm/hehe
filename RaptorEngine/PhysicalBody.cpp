#include "interface\PhysicalBody.h"
#include "PhysicalBodyImp.h"
#include "Mesh.h"
#include "Texture.h"
namespace Raptor
{
void PhysicalBodyImp::UpdateModelMatrix()
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

	void PhysicalBodyImp::Translate(GLfloat x,GLfloat y,GLfloat z)
	{
	
	transform._position=cml::vector3f(x,y,z);//absolute setting
	UpdateModelMatrix();
	}

	void PhysicalBodyImp::SetNonMoving(cml::matrix44f_c proj, cml::matrix44f_c Camera)
	{
	MV=Camera*GetModelMatrix();
	cml::matrix_linear_transform(normalTrans,MV);//transform normal to view space
	//equal to inversing and transposing the to View space matrix
	normalTrans.inverse();
	normalTrans.transpose();

	MVP=proj*MV;
	isNonMoving=true;
	}

	void PhysicalBodyImp::Translate( cml::vector3f offset )
	{
			transform._position+=offset;//accumulate setting
		UpdateModelMatrix();	

	}

	void PhysicalBodyImp::SetScale(GLfloat scaleX,GLfloat scaleY,GLfloat scaleZ)
	{
	transform._scale=cml::vector3f(scaleX,scaleY,scaleZ);
		UpdateModelMatrix();	
	}

	//void PhysicalBodyImp::SetFliped(bool isFlip)
	//{
	//	_flipSide=isFlip;
	//}

void PhysicalBodyImp::SetTexture(MapType type, std::string textureKey)
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

void PhysicalBodyImp::setMesh(std::string mymeshKey)
{

	meshKey=mymeshKey;
	transform._position.set(0.0f,0.0f,0.0f);
	transform._orientation.zero();
	transform._scale=cml::vector3f(1.0,1.0,1.0);
	isNonMoving=false;
	UpdateModelMatrix();	

}




cml::matrix44f_c PhysicalBodyImp::GetModelMatrix()
{
	return _modelMatrix;
}

void PhysicalBodyImp::Rotate(float angle)
{
	transform._orientation[1]=angle;
		UpdateModelMatrix();	
}

void PhysicalBodyImp::RotateX(float angle)
{
	transform._orientation[0]=angle;
	UpdateModelMatrix();	
}

void PhysicalBodyImp::RotateY( float angle ) 
{
	transform._orientation[1]=angle;
	UpdateModelMatrix();	
}

void PhysicalBodyImp::RotateZ( float angle ) 
{
	transform._orientation[2]=angle;
	UpdateModelMatrix();	
}


///////////////////////////////////
//PhysiscalBody Interface Delegates
///////////////////////////////////

PhysicalBody::PhysicalBody() : mPhysicalBodyImp(new PhysicalBodyImp())
{}

PhysicalBody::~PhysicalBody()
{
	delete mPhysicalBodyImp;
}

void PhysicalBody::setTexture(MapType type, std::string textureKey)
{
	mPhysicalBodyImp->SetTexture(type, textureKey);
}

void PhysicalBody::translateAbsolute(GLfloat x,GLfloat y,GLfloat z)
{
	mPhysicalBodyImp->Translate(x, y, z);
}

void PhysicalBody::translateRelative(cml::vector3f offset)
{
	mPhysicalBodyImp->Translate(offset);
}

void PhysicalBody::setScale(GLfloat scaleX,GLfloat scaleY,GLfloat scaleZ)
{
	mPhysicalBodyImp->SetScale(scaleX, scaleY, scaleZ);
}

void PhysicalBody::rotate(float angle)
{
	mPhysicalBodyImp->Rotate(angle);
}

void PhysicalBody::setMesh(std::string meshKey)
{
	mPhysicalBodyImp->setMesh(meshKey);
}

void PhysicalBody::rotateX( float angle )
{
	mPhysicalBodyImp->RotateX(angle);
}

void PhysicalBody::rotateY( float angle )
{
	mPhysicalBodyImp->RotateY(angle);
}

}