#include "interface\RaptorEngine.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Texture.h"
class RaptorEngineImp
{
public:
	RaptorEngineImp() :
		mRenderer(NULL),
		mTextureManager(NULL),
		mMeshManager(NULL)
	{

	}
	~RaptorEngineImp()
	{

	}

	/**
	 * Initialize the engine, this should be called before any engine call.
	 */
	bool startUp()
	{
		//initialize all manager singletons
		mTextureManager = TextureManager::GetInstance();
		mTextureManager->StartUp();
	
		mMeshManager=MeshManager::GetInstance();
		mMeshManager->StartUp();

		mRenderer = Renderer::GetInstance();
		mRenderer->StartUp(mTextureManager, mMeshManager);
		return true;
	}
	/**
	 * Deinitialize the engine.
	 */
	bool shutDown()
	{
		mTextureManager->ShutDown();
		mMeshManager->ShutDown();
		mRenderer->ShutDown();
		return true;
	}
	/**
	 * Set the the camera position and target in absolute world position.
	 */
	void setUpCameraAbsolute(cml::vector3f camPos,cml::vector3f camTarget)
	{
		mRenderer->SetCameraPosition(camPos, camTarget);
	}

	/**
	 * Load a texture file into GPU memory.
	 * @param filePath Path to the texture file. This is relative to the
	 *	location of the executable.
	 */
	bool loadTexture(std::string filePath)
	{
		string fileName=filePath;
		if( !mTextureManager->ExistTexture(fileName))//only load if not loaded before
		{

			char* a=new char[fileName.size()+1];
			int i;
			for(i=0;i<fileName.size();i++)
			{
				a[i]=fileName[i];
			}
			a[i]='\0';
			mTextureManager->LoadSingleImage(a);

		}
		return true;
	}
	
	/**
	 * Load an .obj mesh file into GPU memory.
	 * @param filePath Path to the mesh file. This is relative to the
	 *	location of the executable.
	 */
	bool loadObjMesh(std::string filePath)
	{
		if(!mMeshManager->IsMeshExist(filePath))
		{
			mMeshManager->LoadObjFile(filePath);

		}
		return true;
	}
	
	/**
	 * Call before any call to render object.
	 */
	void beginRendering()
	{
		mRenderer->BeginRendering();
	}
	
	/**
	 * Each object that want to render its PhysicalBody should call this function after
	 * BeginRendering and before EndRendering.
	 */
	void renderPhysicalBody(PhysicalBody* physicalBody)
	{
		mRenderer->RenderNormalMapObject(physicalBody);
	}
	
	/**
	 * Should be called after all the scene objects have rendered themselves.
	 */
	void endRendering()
	{
		mRenderer->EndRendering();
	}

	void setLightTarget(GLfloat x,GLfloat y,GLfloat z)
	{
		mRenderer->SetLightTarget(x, y, z);
	}

	void setLightPos(GLfloat x, GLfloat y, GLfloat z)
	{
		mRenderer->setLightPos(x, y, z);
	}
private:
	
	Renderer* mRenderer;
	MeshManager* mMeshManager;
	TextureManager* mTextureManager;
};


////////////////////////////Delegation to pimp/////////////////////////////////
RaptorEngine::RaptorEngine() : mEngineImp(new RaptorEngineImp())
{};
RaptorEngine::~RaptorEngine()
{
	delete mEngineImp;
}

/**
* Initialize the engine, this should be called before any engine call.
*/
bool RaptorEngine::startUp()
{
	return mEngineImp->startUp();
}
/**
* Deinitialize the engine.
*/
bool RaptorEngine::shutDown()
{
	return mEngineImp->shutDown();
}
/**
* Set the the camera position and target in absolute world position.
*/
void RaptorEngine::setUpCameraAbsolute(cml::vector3f camPos,cml::vector3f camTarget)
{
	mEngineImp->setUpCameraAbsolute(camPos, camTarget);
}

void RaptorEngine::setLightTarget(GLfloat x,GLfloat y,GLfloat z)
{
	mEngineImp->setLightTarget(x, y,z);
}

void RaptorEngine::setLightPos(GLfloat x, GLfloat y, GLfloat z)
{
	mEngineImp->setLightPos(x, y, z);
}
/**
* Load a texture file into GPU memory.
* @param filePath Path to the texture file. This is relative to the
*	location of the executable.
*/
bool RaptorEngine::loadTexture(std::string filePath)
{
	return mEngineImp->loadTexture(filePath);
}

/**
* Load an .obj mesh file into GPU memory.
* @param filePath Path to the mesh file. This is relative to the
*	location of the executable.
*/
bool RaptorEngine::loadObjMesh(std::string filePath)
{
	return mEngineImp->loadObjMesh(filePath);
}

/**
* Call before any call to render object.
*/
void RaptorEngine::beginRendering()
{
	mEngineImp->beginRendering();
}

/**
* Each object that want to render its PhysicalBody should call this function after
* BeginRendering and before EndRendering.
*/
void RaptorEngine::renderPhysicalBody(PhysicalBody* physicalBody)
{
	mEngineImp->renderPhysicalBody(physicalBody);
}

/**
* Should be called after all the scene objects have rendered themselves.
*/
void RaptorEngine::endRendering()
{
	mEngineImp->endRendering();
}
///////////////////////////////////////////////////////////////////////////////
