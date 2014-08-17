#ifndef PHYSICAL_BODY_H
#define PHYSICAL_BODY_H
#include "glew.h"
#include "cml\cml.h"

class PhysicalBodyImp;
enum MapType{diffuse,normal, height, specular, count};
/**
 * This class represent a physical object that can be rendered by the engine.
 * The object of this class has basic tranformation information(position, orientation,
 * scale) and material information: diffuse map, normal map.
 */
class PhysicalBody
{
public:

	PhysicalBody();
	~PhysicalBody();
	/**
	* Set the textures for the physical body
	* @param type The type of the texture map: diffuse or normal
	* @param textureKey The string key to access the texture
	*/
	void setTexture(MapType type, std::string textureKey);
	/**
	* Set the mesh of the physical body
	* @param meshKey the string key of the mesh object
	*/
	void setMesh(std::string meshKey);
	/**
	* Set the absolute position for the object
	*/
	void translateAbsolute(GLfloat x,GLfloat y,GLfloat z);
	/**
	* Translate object in relative to the current position
	*/
	void translateRelative(cml::vector3f offset);
	/**
	* Set scale for the object
	*/
	void setScale(GLfloat scaleX,GLfloat scaleY,GLfloat scaleZ);
	/**
	* Rotate the object along vertical axis
	*/
	void rotate(float angle);

	/**
	* Rotate the object along X axis
	*/
	void rotateX(float angle);

	/**
	* Rotate the object along Y axis
	*/
	void rotateY(float angle);

	/**
	* Rotate the object along Z axis
	*/
	void rotateZ(float angle);
	
	
	//my real physical body, no one can see this except the lord of the raptors
	PhysicalBodyImp* mPhysicalBodyImp;
};
#endif