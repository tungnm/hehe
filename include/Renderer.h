#ifndef RENDERER_H
#define RENDERER_H
#include "Shader.h"
#include <iostream>
#include <string>
#include "cml\cml.h"
#include "Shader.h"
#include <stdlib.h>
#include <time.h>	
#include "Texture.h"
#include "Mesh.h"
#include <vector>
#include "FBO.h"
#include "Appearance.h"
#include <unordered_map>
#include "TextureViewer.h"
#include "ParticleSystem.h"
#include "GUIElement.h"
#include "Constant.h"
using namespace std;

class Renderer
{
private:
	//optimization:
	GLuint lastVaoHandle;
//test fire
ParticleSystem *fire1;
GLfloat textureOffset;//shader uniform to scroll the texture upward
int lastUpdateTick;

//matrices to be updated and sent to shader uniforms depending on the current object
cml::matrix44f_c _MV;
cml::matrix44f_c _projection;
cml::matrix33f_c _normalTransform;
cml::matrix44f_c _modelViewProj;

//camera matrix and vectors, used to build view matrice
cml::matrix44f_c _cameraSetup;
cml::vector3f _cameraPosition;
cml::vector3f _cameraDirection;
cml::vector3f _cameraTagent;

//shadows:
//matricices to transform to light point of view
cml::matrix44f_c _lightProj;
cml::matrix44f_c _lightView;
cml::matrix44f_c _lightModel;

//FBOs:
//texture viewer are a on screen quad, can be full screen, or a small quad with 2d position on screen
std::unordered_map<string,TextureViewer*> _textureViewList;
//this _fboList is used for Bloom.
std::unordered_map<string, SingleTexFBO*> _fboList;
//G-buffer FBO stored the floating-point-number textures, which are vertex data used for lighting calculation
// this is a deferred rendering pipeline so after G-buffer is buit, there's no distinction among objects and we only know
//a list of 2D- buffers with data
GBuffer gbuffer1;
ShadowFBO shadowFBO1;

//shader options via bool uniforms:
int isSsao;
//post process pass options:
int isBloom;

//cubemap: this one works for forward rendering only
bool hasSkybox;
Appearance skyBox;

//water
Appearance waterModel; 



//list of appearance to be rendered using different shader program
vector<Appearance*> _normalMapList,_solidColorList;

//singleton class to store the mapping of resource file names and GPU handle in hash tables
TextureManager *textureMan;


//shader passes:
//right now all client code for shader-related activity( setting up static uniforms,sending dynamic uniforms)
//is hard coded. This means the writing of these client code requires the knowledge about the shader and render
//algorithm itself. 
ShaderPass BuildGBufferNormalMap,//build G-Buffer for normal map object
			BuildGBufferFire,//build G-buffer for fire particle
			ProcessGBufferPass,buildShadowMap,goraudPass,BuildGBufferSkyBox,normalMapPass,POMPass,quadShader,reflectionShader,refractionShader,verticalBlurPass,horizontalBlurPass,brightExtractionPass,diffuseTexturePass,additivePass;
ShaderPass outputToGPass,deferredSecondPass,BuildGBufferWater, TextShader;

ShaderPass instancedVert;
//private methods
//update modelView, modelViewProjection matrices using the provided model Matrix
void UpdateMatrices(cml::matrix44f_c currengModelMatrix);


//for GUI:
unordered_map<string, GUIElement*> textList;
void LoadLetterMesh();
void RenderGUITexts();

void InitShader();
void InitCameraAndMatrix();
void InitLightMatrix();
//send all uniforms that need to be set only once, to shaders( for example the fire.jpg texture handle used for fire shader
void InitStaticUniforms();
void InitFBOs();
cml::vector3f lightTarget;
//render an object with its VAO handle and number of face, assuming that the matrices already been sent to the shader uniforms
void RenderObj(Appearance *a);

//read from G-buffer and render a full screen quad with all of the lighting calculation, ssao effect...
void ProcessGBuffer();
//Render a quad, assuming that the position and scale of the quad has already been sent to shader uniform
void Renderer::RenderOnScreenQuad();

//use to visualize a texture on screen
void RenderTextureToQuad(int textureHandle);

void Renderer::BuildShadowMap(ShadowFBO* shadowFbo);
void RenderSkyBox();
void UnloadShaders();
void UpdateLightMatrix();
//load the mesh, texture requried such as box.obj for cubemap, random.png for SSAO..
void LoadEngineResource();
MeshManager *meshMan;
	//light: right now renderer only support 1 single point light
cml::vector4f lightPos;
	void RemoveObject(int index)
	{
	//	_normalMapList[index]->visible=false;

	//	delete _normalMapList[index];
//		_normalMapList.erase(_normalMapList.begin()+index);
	}
	//cai' nay dang broken, move vao private
	void AddSolidColorOBJ(string meshKey,cml::vector3f color,cml::vector3f pos, cml::vector3f scale, GLfloat rot);

	//for removing
	int GetNObjects()
	{
		return _normalMapList.size();
	}
public:
	cml::matrix44f_c GetCameraSetup(){return _cameraSetup;}
	cml::matrix44f_c GetProjection(){return _projection;}
	Renderer(){};
	~Renderer(){};
	static Renderer* GetInstance()
	{
		static Renderer singleton;
		return &singleton;

	}
	void SetLightTarget(GLfloat x,GLfloat y,GLfloat z);

	void setLightPos(GLfloat x, GLfloat y, GLfloat z);

	void StartUp();
	void ShutDown();

	void LoadTexture(string textureFileName);
	void LoadMesh(string meshKey);

	/**
	 * This function will load the mesh, textures and create and return the appearance object
	 * @param meshKey fileName of the obj file, located inside textures/
	 * @param diffTex diffuse texture file, located inside textures/
	 * @param normTex normal map texture file
	 * @param pos position vector
	 * @param scale scale vector
	 * @param rot rotation in degree(I think ,maybe radian, deo' hieu?)
	 */
	Appearance* GetAppeance(string meshKey, string diffTex, string normTex, cml::vector3f pos, cml::vector3f scale, GLfloat rot);

	/**
	 * this need to becall before any call to render normalMapObject get called
	 */
	void BeginRendering();
	/**
	 * Each object that want to render its Appearance should call this function after
	 * BeginRendering and before EndRendering
	 */
	void RenderNormalMapObject(Appearance* appearance);
	/**
	 * Should be called after all the scene objects have rendered themsefve
	 */
	void EndRendering();

	void AddText(string id, string text, cml::vector2f position, GLfloat scale, GLfloat condense);
	void ChangeTex(string id, string newText);
	void LoadCubemap(string forward,string backward,string top, string bottom, string right, string left);

	//Camera control interfaces


	void ShowText(string key);
	void HideText(string key);

	void StrafeCamRight(GLfloat amount);
	void StrafeCamLeft(GLfloat amount);
	void MoveCamForward(GLfloat amount);
	void MoveCamBackward(GLfloat amount);
	void UpdateCamera();
	void OrientCameraWithMousePos(int xDisplacement, int yDisplacement);
	void ToggleSSAO();
	void ToggleBloom();
	void GoraudRender();
	void Update(int elapsed);
	void SetupWeight(int kernelSize, GLfloat sigma2 );
	cml::vector3f GetCameraPos(){return _cameraPosition;}
	void SetCameraPosition(cml::vector3f camPos,cml::vector3f camTarget);
};



#endif
