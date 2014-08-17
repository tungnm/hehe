
#include "cml\cml.h"
//forward declaration
class Mesh;
class Texture;

struct MaterialInfo
{
	cml::vector3f color;
	std::string diffuseMap;
	std::string normalMap;
	std::string heightMap;
	std::string specularMap;
};

const MaterialInfo DEFAULT_MATERIAL={cml::vector3f(0.4,0.3,0.3),"none","none","none","none"};

struct Transformation
{
	cml::vector3f _position;
	cml::vector3f _orientation;
	cml::vector3f _scale;
};

class PhysicalBodyImp
{
private:
	//std::string _name;
	cml::matrix44f_c _modelMatrix;
	//bool _flipSide;
	MaterialInfo _material;
	//testing optimization
	//use space to save calculation, only applied when both camera and object not moving
	cml::matrix44f_c MV;
	cml::matrix44f_c MVP;
	cml::matrix33f_c normalTrans;
	bool isNonMoving;
	
public:
	
	void SetNonMoving(cml::matrix44f_c proj, cml::matrix44f_c Camera);
	void UpdateModelMatrix();
	Transformation transform;
	std::string meshKey;
	PhysicalBodyImp()//default constructor
	{
		meshKey="non";
		transform._position.set(0.0f,0.0f,0.0f);
		transform._orientation=cml::vector3f(0.0f,0.0f,0.0f);
		transform._scale=cml::vector3f(1.0,1.0,1.0);
		UpdateModelMatrix();	
	}
	void SetTexture(MapType type, std::string textureKey);
	void SetColor(GLfloat r, GLfloat g, GLfloat b){_material.color=cml::vector3f(r,g,b);}

	std::string GetDiffuseMap(){return _material.diffuseMap;}
	std::string GetNormalMap(){return _material.normalMap;}
	std::string GetHeightMap(){return _material.heightMap;}
	std::string GetSpecularMap(){return _material.specularMap;}
	cml::vector3f GetColor(){return _material.color;}

	PhysicalBodyImp(std::string meshKey);
	void Translate(GLfloat x,GLfloat y,GLfloat z);
	void Translate(cml::vector3f offset);
	void SetScale(GLfloat scaleX,GLfloat scaleY,GLfloat scaleZ);
	cml::matrix44f_c GetModelMatrix();
	void Rotate(float angle);
	void setMesh(std::string meshKey);

	void RotateX(float angle);
	void RotateY( float angle ) ;
	void RotateZ( float angle );




};