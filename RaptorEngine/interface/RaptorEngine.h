#ifndef RAPTOR_ENGINE_H
#define RAPTOR_ENGINE_H
#include <string>
#include "PhysicalBody.h"
namespace Raptor
{
class RaptorEngineImp;
/**
 * Singleton class as a main access to the game engine                                                            
 */
class RaptorEngine
{
public:
	RaptorEngine();
	~RaptorEngine();

	static RaptorEngine* RaptorEngine::GetInstance()
	{
		static RaptorEngine singleton;
		return &singleton;
	}
	/**
	 * Initialize the engine, this should be called before any engine call.
	 */
	bool startUp();
	/**
	 * Deinitialize the engine.
	 */
	bool shutDown();
	/**
	 * Set the the camera position and target in absolute world position.
	 */
	void setUpCameraAbsolute(cml::vector3f camPos,cml::vector3f camTarget);
	
	void setLightTarget(GLfloat x,GLfloat y,GLfloat z);

	void setLightPos(GLfloat x, GLfloat y, GLfloat z);
	/**
	 * Load a texture file into GPU memory.
	 * @param filePath Path to the texture file. This is relative to the
	 *	location of the executable.
	 */
	bool loadTexture(std::string filePath);
	
	/**
	 * Load an .obj mesh file into GPU memory.
	 * @param filePath Path to the mesh file. This is relative to the
	 *	location of the executable.
	 */
	bool loadObjMesh(std::string filePath);
	
	/**
	 * Call before any call to render object.
	 */
	void beginRendering();
	
	/**
	 * Each object that want to render its PhysicalBody should call this function after
	 * BeginRendering and before EndRendering.
	 */
	void renderPhysicalBody(PhysicalBody* physicalBody);
	
	/**
	 * Should be called after all the scene objects have rendered themselves.
	 */
	void endRendering();

private:
	RaptorEngineImp* mEngineImp;
};
}

#endif