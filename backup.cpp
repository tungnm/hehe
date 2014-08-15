

#include "glew.h"
#include<iostream>
#include <vector>
#include <map>
#include "GL/freeglut.h"
#include "cml\cml.h"
#include "Shader.h"
#include <stdlib.h>
#include <time.h>	
#include "Texture.h"
#include "Mesh.h"
#include "RenderableObject.h"
#include "FBO.h"
#include "TextureViewer.h"
#include <sstream>
#include "ParticleSystem.h"

const int W_WIDTH=800;
const int W_HEIGHT=600;

const float Z_NEAR=1.0;
const float Z_FAR=60.0;
//global constant:
//shader controller variables:
GLfloat parallaxWeight=0.0f;

long lastTick=0;

GLfloat g_scale=0.8f;
GLfloat g_intensity=0.8f;
GLfloat g_bias=0.45f;
GLfloat g_sampleRad=0.2;
GLfloat sigN=1.0;

bool ssaoOn=false;

cml::vector4f lightDir(0.5f,0.5f,-1.0f,0.0f); //hard code light directioni as well
cml::vector4f lightPos(2.0f,6.5f,-9.0f,1.0f);
enum QuadPosition{full, lowerLeft, lowerRight};

const float CAMERA_MOVEMENT_SPEED=0.02f;
const float CAMERA_TURN_SPEED=0.005f;

bool keysPressed[255];

ShaderPass buildGBufferPass,colorDotShader,deferredNormalMap,buildShadowMap,goraudPass,skyBoxPass,normalMapPass,POMPass,quadShader,reflectionShader,refractionShader,verticalBlurPass,horizontalBlurPass,brightExtractionPass,diffuseTexturePass,additivePass;
ShaderPass outputToGPass,deferredSecondPass;

//scene manipulation:
int currentObjectSelection=0;
int currentAxisSelection=0;
const GLfloat MOVING_SPEED=0.01;

enum ShaderMode{goraud, reflection,bloom, refraction,diffusE,deferred, COUNT=6 };
cml::matrix44f_c _lightProj;
cml::matrix44f_c _lightView;
cml::matrix44f_c _lightModel;

GLfloat textureOffset=1.0f;

ParticleSystem *fire1;

ParticleSystem *smoke1;
void GetRandomKernel()
{
	
	ofstream myOfile;
	myOfile.open ("randomKernel.txt");
	// Normalize the weights and print result to text file:
	//for( int i = 0; i < 10; i++ ) {
	//	//snprintf(uniName, 20, "Weight[%d]", i);

	//	weights[i]=weights[i] / sum;
	//	myOfile << 	weights[i]<< ",";
	//}

	//print the kernel to a text file:

	
	srand ( time(NULL) );

	cml::vector3f pSphere[16];
	for (unsigned i = 0; i < 16; ++i) 
	{
		float ranX=float(rand() % 20000 -10000);
		float ranY=float(rand() % 20000 -10000);
		float ranZ=float(rand() % 10000);

		pSphere[i] = cml::vector3f(ranX/10000.0,ranY/10000.0,ranZ/10000.0);

		pSphere[i].normalize();
		float scale = float(i) /16.0;
		scale = cml::lerp(0.1f, 1.0f, scale * scale);
		pSphere[i] *= scale;
		myOfile <<"vec3("<<pSphere[i][0]<<","<<pSphere[i][1]<<","<<pSphere[i][2]<<"),"<<endl;
	}
	myOfile.close();

}

vector<RenderableObject*> _normalMapList,_solidColorList,_parallaxList,_reflectionList;
RenderableObject* skyBox;

std::map<string,Texture*> _textureList;
std::map<string,Mesh*> _meshList;
std::map<string,TextureViewer*> _textureViewList;

Texture cubeMapTex,diffuseIrrTex,specularIrrTex, noiseTex,fireTex;

int currentMode=0;//set currentMode to the first mode in the enum list

vector<SingleTexFBO*> _fboList;
GBuffer gbuffer1;
ShadowFBO shadowFBO1;


GLfloat x=0.0;
GLfloat y=0.0;
GLfloat _left=-1.0,_right=1.0,_top=1.0,_bottom=-1.0,_near=1.0,_far=90.0;

const cml::vector3f OY(0.0,1.0,0.0);
const cml::vector3f OX(1.0,0.0,0.0);
const cml::vector3f OZ(0.0,0.0,1.0);

GLuint _renderTexture;

cml::matrix44f_c _cameraSetup;
cml::matrix44f_c _projection;
cml::matrix33f_c _normalTransform;

cml::matrix44f_c _modelViewProj;
cml::vector3f _cameraPosition;
cml::vector3f _cameraDirection;
cml::vector3f _cameraTagent;

GLfloat gauss(float x, float sigma2 )
{
	double coeff = 1.0 / (2.0 * 3.14159265 * sigma2);
	double expon = -(x*x) / (2.0 * sigma2);
	return (float) (coeff*exp(expon));
}
void SetupWeight(int kernelSize, GLfloat sigma2 )
{
	GLfloat *weights;
	weights=new GLfloat[kernelSize];
	GLfloat sum;//for gaussian blur:
	// Compute and sum the weights
	weights[0] = gauss(0,sigma2); // The 1-D Gaussian function
	sum = weights[0];
	for( int i = 1; i < 10; i++ ) {
		weights[i] = gauss(i, sigma2);
		sum += 2 * weights[i];
	}

	ofstream myOfile;
	myOfile.open ("blur.txt");
	// Normalize the weights and print result to text file:
	for( int i = 0; i < 10; i++ ) {
		//snprintf(uniName, 20, "Weight[%d]", i);

		weights[i]=weights[i] / sum;
		myOfile << 	weights[i]<< ",";
	}

	//print the kernel to a text file:

	myOfile.close();

	delete [] weights;
	weights=0;
}

//render functino is outside of class renderable object
void RenderOBJ(RenderableObject* a)
{
	//	glBindFramebuffer(GL_FRAMEBUFFER, 1);//hard code 1, which is the custom framebuffer 
	//glViewport(0,0,600,600);
	if (a->IsFlipped())
	{
		glFrontFace(GL_CW);
		//draw the mesh:
		//glBindVertexArray(_mesh->GetVAOHandle());
		//	glDrawArrays(GL_TRIANGLES, 0,_mesh->GetNFaces() *3);

		// bind VAO
		glBindVertexArray(_meshList[a->GetMeshKey()]->GetVAOHandle());
		// draw
		glDrawElements(GL_TRIANGLES,_meshList[a->GetMeshKey()]->_nFaces*3,GL_UNSIGNED_INT,0);

		glFrontFace(GL_CCW);
	}
	else
	{

		//draw the mesh:
		glBindVertexArray(_meshList[a->GetMeshKey()]->GetVAOHandle());
		// draw
		glDrawElements(GL_TRIANGLES,_meshList[a->GetMeshKey()]->_nFaces*3,GL_UNSIGNED_INT,0);
	}
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Draw a specify RenderableObject by composing the MV and MVP from CameraSetup, CameraMovement and model Matrix from
void SendTransToViewSpaceUni(RenderableObject *obj1,ShaderPass* shader)
{	
	cml::matrix44f_c MV;
	MV.identity();
	MV=_cameraSetup*obj1->GetModelMatrix();
	cml::matrix_linear_transform(_normalTransform,MV);//transform normal to view space
	//equal to inversing and transposing the to View space matrix
	_normalTransform.inverse();
	_normalTransform.transpose();
	_modelViewProj=_projection*MV;

	//**NOTE: This is bad practice, this should be encapsulated into ShaderPass Class
	//sending modelView and projection matrix as uniform to vertex shader:
	shader->SendMat4ToUni("MV",&MV);//pass in model matrix to transform to
	//world space
	shader->SendMat4ToUni("MVP",&_modelViewProj);
	shader->SendMat3ToUni("normalTransform",&_normalTransform);//transformation of normal to 


	//shader->SendMat4ToUni("V",&_cameraSetup);//pass in model matrix to transform to

}
void SendTransToViewSpaceUni1(RenderableObject *obj1,ShaderPass* shader)
{	
	cml::matrix44f_c MV;
	MV.identity();
	MV=_cameraSetup*obj1->GetModelMatrix();
	cml::matrix_linear_transform(_normalTransform,MV);//transform normal to view space
	//equal to inversing and transposing the to View space matrix
	_normalTransform.inverse();
	_normalTransform.transpose();
	_modelViewProj=_projection*MV;

	//**NOTE: This is bad practice, this should be encapsulated into ShaderPass Class
	//sending modelView and projection matrix as uniform to vertex shader:
	shader->SendMat4ToUni("MV",&MV);//pass in model matrix to transform to
	//world space
	//shader->SendMat4ToUni("MVP",&_modelViewProj);
	shader->SendMat3ToUni("normalTransformView",&_normalTransform);//transformation of normal to 


	//shader->SendMat4ToUni("V",&_cameraSetup);//pass in model matrix to transform to

}
void SendTransToWorldSpaceUni(RenderableObject *obj1,ShaderPass* shader)
{
	//obj1->rotate();	
	cml::matrix44f_c MV;
	MV=_cameraSetup*obj1->GetModelMatrix();
	//build transformation matrix for normal: which is inverted tranposed of model view( tranpose will be implemented in the function,
	//so only do inverse
	cml::matrix_linear_transform(_normalTransform,obj1->GetModelMatrix());//this function extract the top left 3-3 matrix from _cameraSetup to _normalTransform
	_normalTransform.inverse();
	_normalTransform.transpose();
	_modelViewProj=_projection*MV;

	//**NOTE: This is bad practice, this should be encapsulated into ShaderPass Class
	//sending modelView and projection matrix as uniform to vertex shader:
	shader->SendMat4ToUni("M",&obj1->GetModelMatrix());//pass in model matrix to transform to
	//world space
	shader->SendMat4ToUni("MVP",&_modelViewProj);
	shader->SendMat3ToUni("normalTransform",&_normalTransform);//transformation of normal to 
	//world space:
	//camera position for calculting view vector V:

	//shader->SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);

}
void DeferredRender()
{

	//first pass: draw to G Buffer
	gbuffer1.SetActive();//draw o 3 texture of g buffer: pos, norm, diff
	// 	glViewport (0, 0, W_WIDTH, W_HEIGHT);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	outputToGPass.Activate();
	//render scene:
	for(int i=0;i<_normalMapList.size();i++)//testing -1 here is to exclude the last box
	{	
		//SetReflectionUniforms(_normalMapList[i],&outputToGPass);
		//before used setReflection to transform to world space, now use this one to 
		//transform to view space:
		SendTransToViewSpaceUni(_normalMapList[i],&outputToGPass);
		RenderOBJ(_normalMapList[i]);
		//diffuse object need texture so we did everything we did for reflective object 
		//and set the texture:
		//	diffuseIrrShader.SendIntToUni("objectTex",_normalMapList[i]->GetTextureHandle());
	}


	//draw ssao layer to fbo 1
	_fboList[0]->SetActive();
	// 	glViewport (0, 0, W_WIDTH, W_HEIGHT);
	// 	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//dont know why but still need to clear
	deferredSecondPass.Activate();
	// uniform sampler2D gPosTex;
	//uniform sampler2D gNormTex;
	//uniform sampler2D gDiffTex;
	//set input uniform for shader:
	//SENDING UNIFORM HAS BEEN MOVED TO AFTER INIT
	//NOTE: normally for get input from texture, we use quadVert.glsl
	//render full screen:

	_textureViewList["fullScreen"]->Render();//use the quad data to get full screen texture



	//blur ssao layer and apply to the diffuse layer:
	//first blur of ssao draw to fboList[1]
	_fboList[1]->SetActive();
	// 	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	// 	glViewport (0, 0, W_WIDTH/2, W_HEIGHT/2);
	verticalBlurPass.Activate();//normal texture shader, will draw texture as origin
	//use the texture of the second FBO to use in shader
	verticalBlurPass.SendIntToUni("tex1",_fboList[0]->GetTextureHandle());
	//set positino of the quad on 2d screen:
	verticalBlurPass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());
	//render the full screen quad
	_textureViewList["fullScreen"]->Render();

	//second blur of ssao Drawn to fboList[2]
	_fboList[2]->SetActive();
	// 	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	// 	glViewport (0, 0, W_WIDTH/2, W_HEIGHT/2);
	horizontalBlurPass.Activate();//normal texture shader, will draw texture as origin
	//send in the blurred texture
	horizontalBlurPass.SendIntToUni("tex1",_fboList[1]->GetTextureHandle());//input to fbo2 is texture of fbo1
	horizontalBlurPass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());
	//render with textureViewer
	_textureViewList["fullScreen"]->Render();


	//LAST PASS: ADD BLURRED SSAO AND DIFFUSE TOGETHER AND DRAW TO SCREEN:
	//use quad shader vert to draw full screen quad
	//quad shader is similar to extract, second pass, first pass in bloom but it doesn't
	//change the texture, just output the original 
	FBO::SetDefault(W_WIDTH,W_HEIGHT);
	// 	glViewport (0, 0, W_WIDTH, W_HEIGHT);
	// 	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//dont know why but still need to clear
	//depth here

	/*
	//draw the lower left normal viewer
	quadShader.Activate();
	//quadShader.SendIntToUni("tex1",GBuffer.GetGNormTexHandle());//visualize the normal:
	quadShader.SendIntToUni("tex1",GBuffer.GetGNormTexHandle());//visualize the normal:

	quadShader.SendMat4ToUni("MV",_textureViewList[0]->GetModelMatrix());
	_textureViewList[0]->Render();//use the quad data to get full screen texture
	//draw the lower right Lineardepth normal viewer
	quadShader.Activate();
	quadShader.SendIntToUni("tex1",GBuffer.GetLinearDepthTexHandle());//visualize the normal:
	quadShader.SendMat4ToUni("MV",_textureViewList[2]->GetModelMatrix());
	_textureViewList[2]->Render();//use the quad data to get full screen texture
	*/
	//now draw the combined ssao and diffuse with full screen quad using the addition shader:
	additivePass.Activate();

	//additivePass.SendIntToUni("tex1",_fboList[0]->GetTextureHandle());
	//additivePass.SendIntToUni("tex2",gbuffer1.GetGDiffTexHandle());


	//quadShader.SendIntToUni("tex1",GBuffer.GetGNormTexHandle());//visualize the normal:
	//quadShader.SendIntToUni("tex1",_fboList[2]->GetTextureHandle());//visualize the normal:
	additivePass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());//textureViewerList[1] is the full screen one
	_textureViewList["fullScreen"]->Render();//use the quad data to get full screen texture



}

void SendTransToLightSpaceUni(RenderableObject *obj1,ShaderPass* shader)
{
	cml::matrix44f_c MVPL;
	MVPL.identity();
	MVPL=_lightProj*_lightView*obj1->GetModelMatrix();
	shader->SendMat4ToUni("MVPLight",&MVPL);//pass in model matrix to transform to
}
void ReflectionRender()
{
	//glViewport (0, 0, 600, 600);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyBoxPass.Activate();
	//draw the cube map first( assume that it is the first RenderableObject in _normalMapList.
	SendTransToWorldSpaceUni(skyBox,&skyBoxPass);
	skyBoxPass.SendIntToUni("isSkybox",1);
	skyBoxPass.SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);
	RenderOBJ(skyBox);
	skyBoxPass.SendIntToUni("isSkybox",0);

	reflectionShader.Activate();
	cml::vector4f lightInView=_cameraSetup*lightPos;
	reflectionShader.SendVec3ToUni("lightPosViewSpace",lightInView[0],lightInView[1],lightInView[2]);




	for(int i=0;i<_reflectionList.size();i++)//testing -1 here is to exclude the last box
	{	
		if(_reflectionList[i]->isWater)
					{
						//vertex transform
			
						int time=glutGet(GLUT_ELAPSED_TIME);
						reflectionShader.SendFloatToUni("time",time );
						reflectionShader.SendFloatToUni("textureScale",_reflectionList[i]->textureScale );
						reflectionShader.SendFloatToUni("isWater",true);
					}
					else
					{
							reflectionShader.SendFloatToUni("isWater",false);
							reflectionShader.SendFloatToUni("textureScale",1.0);
					}
		SendTransToWorldSpaceUni(_reflectionList[i],&reflectionShader);
		SendTransToLightSpaceUni(_reflectionList[i],&reflectionShader);
		SendTransToViewSpaceUni1(_reflectionList[i],&reflectionShader);
	//send camera position:
	reflectionShader.SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);
	//	_reflectionList[i]->rotate();
	RenderOBJ(_reflectionList[i]);

	}

}

void RefractionRender()
{
	//glViewport (0, 0, 600, 600);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyBoxPass.Activate();
	//draw the cube map first( assume that it is the first RenderableObject in _normalMapList.
	SendTransToWorldSpaceUni(skyBox,&skyBoxPass);
	skyBoxPass.SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);

	RenderOBJ(skyBox);

	refractionShader.Activate();

	for(int i=0;i<_normalMapList.size();i++)//testing -1 here is to exclude the last box
	{	SendTransToWorldSpaceUni(_normalMapList[i],&refractionShader);
	//send camera position:
	refractionShader.SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);
	//	_normalMapList[i]->rotate();
	RenderOBJ(_normalMapList[i]);
	}

}


//void RenderTexture(

void DiffuseTextRender()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//diffuseTexturePass.Activate();

	//for(int i=0;i<_normalMapList.size();i++)//testing -1 here is to exclude the last box
	//{	SendTransToWorldSpaceUni(_normalMapList[i],&diffuseTexturePass);
	//diffuseTexturePass.SendIntToUni("objectTex",_normalMapList[i]->GetTextureHandle());//get texture from pass1
	//diffuseTexturePass.SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);
	//_normalMapList[i]->Render();
	////diffuse object need texture so we did everything we did for reflective object 
	////and set the texture:
	//}

}


void SetUpLightMatrix()
{
	_lightProj.identity();
	_lightView.identity();
	_lightModel.identity();

	cml::matrix_perspective_yfov_RH(_lightProj, cml::rad(45.0f), (GLfloat)(W_WIDTH)/(GLfloat)(W_HEIGHT), Z_NEAR,Z_FAR, cml::ZClip::z_clip_zero);
	//cml::matrix_orthographic_RH(_lightProj,800.0f,600.0f,0.1f, 60.0f, cml::ZClip::z_clip_zero);
	cml::matrix_look_at_RH(_lightView,lightPos[0],lightPos[1],lightPos[2],0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	cml::matrix_set_translation(_lightModel,lightPos[0],lightPos[1],lightPos[2]);
}

void SendTransToLightSpaceUni1(RenderableObject *obj1,ShaderPass* shader)
{
	cml::matrix44f_c MVPL;
	MVPL.identity();
	MVPL=_lightProj*_lightView*obj1->GetModelMatrix();
	cml::matrix44f_c MV;
	MV.identity();
	MV=_lightView*obj1->GetModelMatrix();
	cml::matrix_linear_transform(_normalTransform,MV);//transform normal to view space
	//equal to inversing and transposing the to View space matrix
	_normalTransform.inverse();
	_normalTransform.transpose();
	shader->SendMat4ToUni("MV",&MV);
	shader->SendMat3ToUni("normalTransform",&_normalTransform);
	shader->SendMat4ToUni("MVPLight",&MVPL);//pass in model matrix to transform to
	shader->SendIntToUni("tex1",_textureList[obj1->GetDiffuseMap()]->GetID());
}
void RenderTextureToQuad(int textureHandle,QuadPosition pos)
{
	quadShader.Activate();
	quadShader.SendIntToUni("tex1",textureHandle);//visualize the normal:

	//create a temp texture viewer base on the function parameters just for this function, 
	TextureViewer temp;
	if (pos==QuadPosition::full)
		temp.SetNewDimension(0.0f,0.0f,1.0f);
	else if (pos==QuadPosition::lowerLeft)
		temp.SetNewDimension(-0.5f,-0.5f,0.45f);
	else if (pos==QuadPosition::lowerRight)
		temp.SetNewDimension(0.5f,-0.5f,0.45f);

	//quadShader.SendIntToUni("tex1",GBuffer.GetLinearDepthTexHandle());//visualize the normal:
	//quadShader.SendMat4ToUni("MV",_textureViewList[2]->GetModelMatrix());
	quadShader.SendIntToUni("tex1",textureHandle);//visualize the normal:
	quadShader.SendMat4ToUni("MV",temp.GetModelMatrix());
	temp.Render();
}


void RenderAllObjectIn(vector<RenderableObject*>*objList, void (*TransferToSpace)( RenderableObject*,ShaderPass*), ShaderPass* shaderPass, bool shadow)
{
	for(int i=0;i<_normalMapList.size();i++)//testing -1 here is to exclude the last box
	{	
		TransferToSpace((*objList)[i],shaderPass);
		/*if (shadow)
			SendTransToLightSpaceUni((*objList)[i],shaderPass);
			*/
		RenderOBJ(_normalMapList[i]);
	}
}
void RenderAllObjectForShadowMap(vector<RenderableObject*>*objList, void (*TransferToSpace)( RenderableObject*,ShaderPass*), ShaderPass* shaderPass, bool shadow)
{
	for(int i=0;i<_normalMapList.size();i++)//testing -1 here is to exclude the last box
	{	
		TransferToSpace((*objList)[i],shaderPass);
		if (shadow)
			SendTransToLightSpaceUni((*objList)[i],shaderPass);

		RenderOBJ(_normalMapList[i]);
	}
}
void GoraudRender()
{
	goraudPass.Activate();
	//send MVPLight to shader:

	cml::vector4f lightInView=_cameraSetup*lightPos;
	goraudPass.SendVec3ToUni("lightPosViewSpace",lightInView[0],lightInView[1],lightInView[2]);

	for(int i=0;i<_solidColorList.size();i++)//testing -1 here is to exclude the last box
	{	//send common matrix: MV, MVP...
		SendTransToViewSpaceUni(_solidColorList[i],&goraudPass);
		//Send MVPLight
		SendTransToLightSpaceUni(_solidColorList[i],&goraudPass);
		cml::vector3f myColor=_solidColorList[i]->GetColor();
		goraudPass.SendVec3ToUni("color",myColor[0],myColor[1],myColor[2]);

		RenderOBJ(_solidColorList[i]);
	}
}


//render pass to build a shadow map
void BuildShadowMap(ShadowFBO* shadowFbo)
{
	//update light matrix:
	cml::matrix_look_at_RH(_lightView,lightPos[0],lightPos[1],lightPos[2],0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	cml::matrix_set_translation(_lightModel,lightPos[0],lightPos[1],lightPos[2]);


	shadowFbo->SetActive();
	buildShadowMap.Activate(); 
	/*
	int time=glutGet(GLUT_ELAPSED_TIME);
	buildShadowMap.SendFloatToUni("time",time );*/
	
	//glPolygonOffset(-2,-4);
	glCullFace(GL_FRONT);

	RenderAllObjectIn(&_normalMapList,&SendTransToLightSpaceUni1,&buildShadowMap,false);

	glCullFace(GL_BACK);
	//glPolygonOffset(2.1,3.0);
	//	FBO::SetDefault(W_WIDTH,W_HEIGHT);
	//RenderTextureToQuad(shadowFbo->GetTextureHandle(),QuadPosition::lowerLeft);

	//now render the scene
}
void NormalMapRender()
{
	//hardcode light position in viewspace here, need to encapsulate to lightclass later


	//	POMPass.SendFloatToUni("shadowWeight",parallaxWeight); //for parallax mapping only


	normalMapPass.Activate();

	cml::vector4f lightInView=_cameraSetup*lightPos;
	normalMapPass.SendVec3ToUni("lightPosViewSpace",lightInView[0],lightInView[1],lightInView[2]);
	//the the light strength, now hard coded from fire1 T_T
	normalMapPass.SendFloatToUni("lightStrength",fire1->GetLightStrength() );

	//RenderAllObjectIn(&_normalMapList,&SendTransToViewSpaceUni,&normalMapPass,true);
	for(int i=0;i<_normalMapList.size();i++)//testing -1 here is to exclude the last box
	{	//send common matrix: MV, MVP...
		if(_normalMapList[i]->isWater)
					{
						//vertex transform
			
						int time=glutGet(GLUT_ELAPSED_TIME);
						normalMapPass.SendFloatToUni("time",time );
						normalMapPass.SendFloatToUni("textureScale",_normalMapList[i]->textureScale );
						normalMapPass.SendFloatToUni("isWater",true);
					}
					else
					{
							normalMapPass.SendFloatToUni("isWater",false);
							normalMapPass.SendFloatToUni("textureScale",1.0);
					}
		
		
		
		SendTransToViewSpaceUni(_normalMapList[i],&normalMapPass);
		//Send MVPLight
		SendTransToLightSpaceUni(_normalMapList[i],&normalMapPass);
		normalMapPass.SendIntToUni("tex1",_textureList[_normalMapList[i]->GetDiffuseMap()]->GetID());
		normalMapPass.SendIntToUni("normalMap",_textureList[_normalMapList[i]->GetNormalMap()]->GetID());

		RenderOBJ(_normalMapList[i]);
	}
}

void DeferredNormapMapRender()
{

	deferredNormalMap.Activate();
	//create a temp texture viewer base on the function parameters just for this function, 
	TextureViewer temp;
	temp.SetNewDimension(0.0f,0.0f,1.0f);

	//quadShader.SendIntToUni("tex1",GBuffer.GetLinearDepthTexHandle());//visualize the normal:
	//quadShader.SendMat4ToUni("MV",_textureViewList[2]->GetModelMatrix());
	deferredNormalMap.SendMat4ToUni("MV",temp.GetModelMatrix());

	cml::vector4f lightInView=_cameraSetup*lightPos;
	deferredNormalMap.SendVec3ToUni("lightPosViewSpace",lightInView[0],lightInView[1],lightInView[2]);

	//bake and send the matrix to transform to light space:
	cml::matrix44f_c MVPL;
	MVPL.identity();
	//need to reverse the effect of object view matrix already store in there

	cml::matrix44f_c VInverse=_cameraSetup;
	VInverse.inverse();
	MVPL=_lightProj*_lightView*VInverse; //this one will transform MV in shader, to MVPLight
	deferredNormalMap.SendMat4ToUni("MVPLight",&MVPL);//pass in model matrix to transform to



	temp.Render();




}
void ParallaxRender()
{

	POMPass.Activate();

	cml::vector4f lightInView=_cameraSetup*lightPos;
	POMPass.SendFloatToUni("shadowWeight",parallaxWeight); //for parallax mapping only
	POMPass.SendVec3ToUni("lightPosViewSpace",lightInView[0],lightInView[1],lightInView[2]);


	//RenderAllObjectIn(&_normalMapList,&SendTransToViewSpaceUni,&normalMapPass,true);
	for(int i=0;i<_parallaxList.size();i++)//testing -1 here is to exclude the last box
	{	//send common matrix: MV, MVP...
		SendTransToViewSpaceUni(_parallaxList[i],&POMPass);
		//Send MVPLight
		SendTransToLightSpaceUni(_parallaxList[i],&POMPass);
		POMPass.SendIntToUni("tex1",_textureList[_parallaxList[i]->GetDiffuseMap()]->GetID());
		POMPass.SendIntToUni("normalMap",_textureList[_parallaxList[i]->GetNormalMap()]->GetID());
		POMPass.SendIntToUni("heightMap",_textureList[_parallaxList[i]->GetHeightMap()]->GetID());
		RenderOBJ(_parallaxList[i]);
	}
}

void BloomRender()
{
	//This is the bloom render. It need 3 frameBuffers of single texture in _fboList[0],
	//_fboList[1],_fboList[2]

	//first pass: render the scene
	_fboList[0]->SetActive();//draw to first FBO texture
	// 	glViewport (0, 0, W_WIDTH, W_HEIGHT);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 	

	ReflectionRender();
	//DiffuseTextRender();

	//next extract the highlight that is above a lumthresh and store in next FBO
	_fboList[1]->SetActive();//draw to second FBO texture
	// 	glViewport (0, 0, W_WIDTH/2, W_HEIGHT/2);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	brightExtractionPass.Activate();
	brightExtractionPass.SendIntToUni("tex1",_fboList[0]->GetTextureHandle());//get texture from pass1
	brightExtractionPass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());
	_textureViewList["fullScreen"]->Render();//use the quad data to get full screen texture
	//third pass:
	//blur the spot and draw to third FBO
	_fboList[2]->SetActive();
	// 	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	// 	glViewport (0, 0, W_WIDTH/2, W_HEIGHT/2);
	verticalBlurPass.Activate();//normal texture shader, will draw texture as origin
	//use the texture of the second FBO to use in shader
	verticalBlurPass.SendIntToUni("tex1",_fboList[1]->GetTextureHandle());
	//set positino of the quad on 2d screen:
	verticalBlurPass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());
	//render with textureViewer
	_textureViewList["fullScreen"]->Render();

	FBO::SetDefault(W_WIDTH,W_HEIGHT);
	// 	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	// 	glViewport (0, 0,  W_WIDTH, W_HEIGHT);
	horizontalBlurPass.Activate();//normal texture shader, will draw texture as origin
	//send in the blurred texture
	horizontalBlurPass.SendIntToUni("tex1",_fboList[2]->GetTextureHandle());
	//send in the original diffuse texture to blend with the blurred one:
	horizontalBlurPass.SendIntToUni("texOrigin",_fboList[0]->GetTextureHandle());
	//set positino of the quad on 2d screen:
	horizontalBlurPass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());
	//render with textureViewer
	_textureViewList["fullScreen"]->Render();




}
//shader vert: worldCordWithTexVert.glsl","diffuseIrrFrag.glsl
//uniform sampler2D objectTex; 
//uniform vec3 camPos; 
//uniform vec3 lightPos;
void display(void)
{	
//
//	 Forward render:

	BuildShadowMap(&shadowFBO1);
	FBO::SetDefault(W_WIDTH,W_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_BLEND);
	//ReflectionRender();
	//ParallaxRender();
	//GoraudRender();
		
	//NormalMapRender();
		glEnable(GL_BLEND);
	 glDisable(GL_DEPTH_TEST);
	
	
	//test render particle system:
	colorDotShader.Activate();
	//	for(int i=0;i<particleSytemList.size();i++)
		//{
			//if (!particleSytemList[i]->isActive) continue;
			for(int j=0;j<fire1/*particleSytemList[i]*/->nParticles;j++)
			{
				Particle* current=&fire1/*particleSytemList[i]*/->particleList[j];
				cml::matrix44f_c MV;
	MV=_cameraSetup*(current->GetModelMatrix());
	_modelViewProj=_projection*MV;
	colorDotShader.SendMat4ToUni("MVP",&_modelViewProj);
	colorDotShader.SendMat4ToUni("M",&current->GetModelMatrix());
	//particleOrigin
	colorDotShader.SendVec3ToUni("particleOrigin",current->position);

	colorDotShader.SendFloatToUni("textureYOffset",textureOffset);
	colorDotShader.SendFloatToUni("textureXOffset",current->randomXOffset);

	//colorDotShader.SendFloatToUni("textureXOffset",textureOffset);

	//transparency fade with life decrease:
	colorDotShader.SendFloatToUni("transparency",current->lifetime/current->maxLife);
			colorDotShader.SendVec3ToUni("color",current->GetColor());
		//	glBindVertexArray(_meshList["particle.obj"]->GetVAOHandle());
		//	glDrawArrays(GL_TRIANGLES, 0,_meshList["particle.obj"]->GetNFaces() *3);
			
			glBindVertexArray(_meshList["particle.obj"]->GetVAOHandle());
			// draw
			glDrawElements(GL_TRIANGLES,_meshList["particle.obj"]->_nFaces*3,GL_UNSIGNED_INT,0);

						
			}


//smoke1
			//for(int j=0;j<smoke1/*particleSytemList[i]*/->nParticles;j++)
			//{
			//	Particle* current=&smoke1/*particleSytemList[i]*/->particleList[j];
			//	cml::matrix44f_c MV;
			//	MV=_cameraSetup*(current->GetModelMatrix());
			//	_modelViewProj=_projection*MV;
			//	colorDotShader.SendMat4ToUni("MVP",&_modelViewProj);
			//	//transparency fade with life decrease:
			//	colorDotShader.SendFloatToUni("transparency",current->lifetime/current->maxLife);
			//	colorDotShader.SendVec3ToUni("color",current->GetColor());
			//	//	glBindVertexArray(_meshList["particle.obj"]->GetVAOHandle());
			//	//	glDrawArrays(GL_TRIANGLES, 0,_meshList["particle.obj"]->GetNFaces() *3);

			//	glBindVertexArray(_meshList["particle.obj"]->GetVAOHandle());
			//	// draw
			//	glDrawElements(GL_TRIANGLES,_meshList["particle.obj"]->_nFaces*3,GL_UNSIGNED_INT,0);


			//}


	//	}

//	
//BuildShadowMap(&shadowFBO1);
//	/*FBO::SetDefault(W_WIDTH,W_HEIGHT);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//ReflectionRender();*/
//	//deffered render:
//	
//	gbuffer1.SetActive();
//	buildGBufferPass.Activate();
//
//	
//
//
//	//RenderAllObjectIn(&_normalMapList,&SendTransToViewSpaceUni,&buildGBufferPass,false);
//	buildGBufferPass.SendIntToUni("hasParralax",0);
//	
//
//	for(int i=0;i<_normalMapList.size();i++)//testing -1 here is to exclude the last box
//	{	//send common matrix: MV, MVP...
//		if(_normalMapList[i]->isWater)
//		{
//			//vertex transform
//
//			int time=glutGet(GLUT_ELAPSED_TIME);
//			buildGBufferPass.SendFloatToUni("time",time );
//			buildGBufferPass.SendFloatToUni("textureScale",_normalMapList[i]->textureScale );
//			buildGBufferPass.SendFloatToUni("isWater",true);
//		}
//		else
//		{
//				buildGBufferPass.SendFloatToUni("isWater",false);
//				buildGBufferPass.SendFloatToUni("textureScale",1.0);
//		}
//		SendTransToViewSpaceUni(_normalMapList[i],&buildGBufferPass);
//		//Send MVPLight
//		SendTransToLightSpaceUni(_normalMapList[i],&buildGBufferPass);
//		buildGBufferPass.SendIntToUni("tex1",_textureList[_normalMapList[i]->GetDiffuseMap()]->GetID());
//		buildGBufferPass.SendIntToUni("normalMap",_textureList[_normalMapList[i]->GetNormalMap()]->GetID());
//		
//		RenderOBJ(_normalMapList[i]);
//	}
//	//parrallax
//	if (_parallaxList.size()>0)
//	{
//		cml::vector4f lightInView=_cameraSetup*lightPos;
//		buildGBufferPass.SendVec3ToUni("lightPosViewSpace",lightInView[0],lightInView[1],lightInView[2]);
//		buildGBufferPass.SendIntToUni("hasParralax",1);
//
//	for(int i=0;i<_parallaxList.size();i++)//testing -1 here is to exclude the last box
//	{	//send common matrix: MV, MVP...
//		SendTransToViewSpaceUni(_parallaxList[i],&buildGBufferPass);
//		//Send MVPLight
//		SendTransToLightSpaceUni(_parallaxList[i],&buildGBufferPass);
//		buildGBufferPass.SendIntToUni("tex1",_textureList[_parallaxList[i]->GetDiffuseMap()]->GetID());
//		buildGBufferPass.SendIntToUni("heightMap",_textureList[_parallaxList[i]->GetHeightMap()]->GetID());
//		buildGBufferPass.SendIntToUni("normalMap",_textureList[_parallaxList[i]->GetNormalMap()]->GetID());
//		
//		RenderOBJ(_parallaxList[i]);
//	}
//
//	}
//
//	FBO::SetDefault(W_WIDTH,W_HEIGHT);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//skyBoxPass.Activate();
//	////draw the cube map first( assume that it is the first RenderableObject in _normalMapList.
//	//SendTransToWorldSpaceUni(skyBox,&skyBoxPass);
//	//skyBoxPass.SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);
//
//	//RenderOBJ(skyBox);
////	ReflectionRender();
//	//RenderTextureToQuad(shadowFBO1.GetTextureHandle(),QuadPosition::lowerLeft);
//	/*RenderTextureToQuad(shadowFBO1.GetNormalHandle(),QuadPosition::lowerLeft);
//	RenderTextureToQuad(shadowFBO1.GetAlbedoandle(),QuadPosition::lowerRight);*/
//	
//	DeferredNormapMapRender();
//
	glutSwapBuffers(); 
}

void UpdateCamera()
{
	//room
	cml::matrix_look_at_RH(_cameraSetup,_cameraPosition[0],_cameraPosition[1],_cameraPosition[2],_cameraPosition[0]+_cameraDirection[0],_cameraPosition[1]+_cameraDirection[1],_cameraPosition[2]+_cameraDirection[2],0.0f,1.0f,0.0f);
	//rex and bunny
	//cml::matrix_look_at_RH(_cameraSetup,_cameraPosition[0],_cameraPosition[1],_cameraPosition[2],0.0f,_cameraPosition[1]+_cameraDirection[1],0.0f,0.0f,1.0f,0.0f);

}



void SetUpCamera()
{
	
	_projection.identity();

	cml::matrix_perspective_yfov_RH(_projection, cml::rad(45.0f), (GLfloat)(W_WIDTH)/(GLfloat)(W_HEIGHT),Z_NEAR, Z_FAR, cml::ZClip::z_clip_zero);

	_cameraPosition.set(0.0f,0.8f,-5.0f);

	//Initialize camera direction and camera tagent need to be perpendicular to each other
	_cameraDirection.set(0.0f,0.0f,1.0f);
	_cameraTagent.set(1.0f,0.0f,0.0f);

	cml::matrix_look_at_RH(_cameraSetup,_cameraPosition[0],_cameraPosition[1],_cameraPosition[2],0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);

	//UpdateCamera();
	//cml::matrix_rotate_about_world_y(rot,cml::rad(45.0f));
	//_cameraSetup=rot*_cameraSetup;
	//initliaze light:
	SetUpLightMatrix();
}

void LoadResources1() 
{
	//load mesh:
	//	_meshList["box"]=new Mesh();
	//	_meshList["box"]->Load("models\\utahStandard.obj");//TODO: need to load box.obj for skybox
	_meshList["rex"]=new Mesh();
	_meshList["rex"]->Load("models\\stego.obj");
	_meshList["box"]=new Mesh();
	_meshList["box"]->Load("models\\box.obj");

	//load environment map, diffuse irradiance and specular irradiance map textures:
		//diffuseIrrTex.SetUpCubeMap("stormy\\forwardD.png","stormy\\backwardD.png","stormy\\upD.png","stormy\\downD.png","stormy\\rightD.png","stormy\\leftD.png");
	//specularIrrTex.SetUpCubeMap("stormy\\forwardS.png","stormy\\backwardS.png","stormy\\upS.png","stormy\\downS.png","stormy\\rightS.png","stormy\\leftS.png");

	_textureList["stego"]=new Texture();
	_textureList["stego"]->SetUpWithImage("textures\\stego.jpg");

	_textureList["stegoNormal"]=new Texture();
	_textureList["stegoNormal"]->SetUpWithImage("textures\\stego_normal.jpg");

	_textureList["stegoHeight"]=new Texture();
	_textureList["stegoHeight"]->SetUpWithImage("textures\\rockBump.png");




	//	_textureList["noise"]=new Texture();
	//	_textureList["noise"]->SetUpWithImage("textures\\noise.png");
}


//check if mesh already loaded, if not then load-> prevent incorrectMesh key
void SetUpFBOs()
{
	//---------------------------------Set Up FBO for bloom effect:
	//

	// These setting is for bloom
	FBO::FBOInfo info;
	info.width=W_WIDTH;info.height=W_HEIGHT;info.hasDepth=true;

	_fboList.push_back(new SingleTexFBO());//store origin render
	_fboList[0]->SetUp(info);
	_fboList.push_back(new SingleTexFBO());//store extracted highlighted 
	//the rest doesn't need depth because of post process:
	info.hasDepth=false;info.width/=2;info.height/=2;

	_fboList[1]->SetUp(info);
	_fboList.push_back(new SingleTexFBO());//second blur
	_fboList[2]->SetUp(info);
	//--------------------------------------------------------------

	/*
	//set up FBO for ssao:-----------------------------------------
	//test test make first fbo small for ssao:
	_fboList.push_back(new FBO());//store origin render
	_fboList[0]->SetUpSingleTexFBO(true, W_WIDTH, W_HEIGHT);
	_fboList.push_back(new FBO());//store extracted highlighted 
	_fboList[1]->SetUpSingleTexFBO(false, W_WIDTH/2, W_HEIGHT/2);
	_fboList.push_back(new FBO());//second blur
	_fboList[2]->SetUpSingleTexFBO(false, W_WIDTH/2, W_HEIGHT/2);
	//--------------------------------------------------------------

	*/


	info.width=W_WIDTH*2;
	info.height=W_HEIGHT*2;
	shadowFBO1.SetUp(info);


	info.width=W_WIDTH;
	info.height=W_HEIGHT;
	gbuffer1.SetUp( info);
	//add 2 textureViewers:
	_textureViewList["lowerLeft"]=new TextureViewer(-0.5f,-0.5f,0.45f);//lower left viewer
	_textureViewList["fullScreen"]=new TextureViewer(0.0f,0.0f,1.0f);//full screen
	_textureViewList["lowerRight"]=new TextureViewer(0.5f,-0.5f,0.45f);//lower right viewer

}

void LoadTexture(string textureKey)
{
	if(_textureList[textureKey]==NULL)
	{
		_textureList[textureKey]=new Texture();
		string fileName="textures\\"+textureKey;
		char* a=new char[fileName.size()+1];
		int i;
		for(i=0;i<fileName.size();i++)
		{
			a[i]=fileName[i];
		}
		a[i]='\0';

		_textureList[textureKey]->SetUpWithImage(a);
	}
}
//send to shaders uniforms which are not frequenly changed( so only need to call this function once) 
void SetUpStaticUniforms()
{
	//--NOTE: need to activate shader before sending data to the shader
	cubeMapTex.SetUpCubeMap("textures\\stormy\\tforward.png","textures\\stormy\\tbackward.png","textures\\stormy\\tup.png","textures\\stormy\\tdown.png","textures\\stormy\\tright.png","textures\\stormy\\tleft.png");
	fireTex.SetUpWithImage("textures\\fire2.jpg");

	//set fire texture 
	colorDotShader.Activate();
	colorDotShader.SendIntToUni("fireTexture",fireTex.GetID());

	skyBoxPass.Activate();
	skyBoxPass.SendIntToUni("CubeMapTex",cubeMapTex.GetID());//set texture for skybox and reflective objs once

	reflectionShader.Activate();
	reflectionShader.SendIntToUni("CubeMapTex",cubeMapTex.GetID());
	reflectionShader.SendIntToUni("shadowMap",shadowFBO1.GetTextureHandle());

	refractionShader.Activate();
	refractionShader.SendIntToUni("CubeMapTex",cubeMapTex.GetID());

	POMPass.Activate();
	//POMPass.SendIntToUni("tex1",_textureList["stego"]->GetID());
	//POMPass.SendIntToUni("normalMap",_textureList["stegoNormal"]->GetID());
	//POMPass.SendIntToUni("heightMap",_textureList["stegoHeight"]->GetID());
	POMPass.SendIntToUni("shadowMap",shadowFBO1.GetTextureHandle());

	normalMapPass.Activate();
	normalMapPass.SendIntToUni("shadowMap",shadowFBO1.GetTextureHandle());

	goraudPass.Activate();
	//shadow sampler for goraud:
	goraudPass.SendIntToUni("shadowMap",shadowFBO1.GetTextureHandle());

	diffuseTexturePass.Activate();
	//diffuseTexturePass.SendIntToUni("diffuseIrrTex",diffuseIrrTex.GetID());
	//lightPos dont change so could set it here at the beginning
	//rex and bunny
	//diffuseTexturePass.SendVec3ToUni("lightPos",-4.0,3.0,1.0);
	//room scene
	diffuseTexturePass.SendVec3ToUni("lightPos",3.0,6.0,3.0);

	//bind G buffer to texture for deferred render:
	deferredNormalMap.Activate(); 
	deferredNormalMap.SendIntToUni("gPosTex",gbuffer1.GetGPosTexHandle());
	deferredNormalMap.SendIntToUni("gNormTex",gbuffer1.GetGNormTexHandle());
	deferredNormalMap.SendIntToUni("gDiffuseTex",gbuffer1.GetGDiffuseTexHandle());
	deferredNormalMap.SendIntToUni("gTanTex",gbuffer1.GetGTanTexHandle());
	deferredNormalMap.SendIntToUni("gNormalMapTex",gbuffer1.GetGNormalMapTexHandle());

	deferredNormalMap.SendMat4ToUni("camProj",&_projection);

	deferredNormalMap.SendIntToUni("shadowMap",shadowFBO1.GetTextureHandle());
	//for RSM
	/*uniform sampler2DShadow shadowMapPosition;
	uniform sampler2DShadow shadowMapNormal;
	uniform sampler2DShadow shadowMapAlbedo;*/
	//deferredNormalMap.SendIntToUni("shadowMapPosition",shadowFBO1.GetPosHandle());
	//deferredNormalMap.SendIntToUni("shadowMapNormal",shadowFBO1.GetNormalHandle());
	//deferredNormalMap.SendIntToUni("shadowMapAlbedo",shadowFBO1.GetAlbedoandle());

	LoadTexture("random_normals.png");
	deferredNormalMap.SendIntToUni("rnm",_textureList["random_normals.png"]->GetID());


	deferredSecondPass.Activate();
	//deferredSecondPass.SendIntToUni("gDiffTex",gbuffer1.GetGDiffTexHandle());
	deferredSecondPass.SendIntToUni("gNormTex",gbuffer1.GetGNormTexHandle());
	//	deferredSecondPass.SendIntToUni("gDepthTex",gbuffer1.GetLinearDepthTexHandle());
	deferredSecondPass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());
	//deferredSecondPass.SendIntToUni("randomTex",_textureList["noise"]->GetID());
	cml::matrix33f_c projI;
	cml::matrix_linear_transform(projI,_projection);//this function extract the top left 3-3 matrix from _cameraSetup to _normalTransform
	projI.inverse();
	deferredSecondPass.SendMat3ToUni("ProjectionMatrixInverse",&projI);
}

void LoadMesh(string meshKey)
{
	//check if mesh 
	if(_meshList[meshKey]==NULL)
	{
		_meshList[meshKey]=new Mesh();
		_meshList[meshKey]->Load("models\\"+meshKey);
	}
}
void AddNormalMapOBJ(string meshKey, string diffTex, string normTex, cml::vector3f pos, cml::vector3f scale, GLfloat rot)
{
	LoadMesh(meshKey);
	LoadTexture(diffTex);
	LoadTexture(normTex);

	_normalMapList.push_back(new RenderableObject(meshKey));

	_normalMapList[_normalMapList.size()-1]->SetTexture(MapType::diffuse,diffTex);
	_normalMapList[_normalMapList.size()-1]->SetTexture(MapType::normal,normTex);

	_normalMapList[_normalMapList.size()-1]->SetScale(scale[0],scale[1],scale[2]);
	_normalMapList[_normalMapList.size()-1]->Translate(pos[0],pos[1],pos[2]);
	_normalMapList[_normalMapList.size()-1]->Rotate(rot);
}

void DeleteNormalMapObj(int index)
{
delete	_normalMapList[index];
_normalMapList[index]=NULL;
_normalMapList.erase(_normalMapList.begin()+index);
}

void AddPOMObj(string meshKey, string diffTex, string normTex,string heightTex, cml::vector3f pos, cml::vector3f scale, GLfloat rot)
{
	LoadMesh(meshKey);
	LoadTexture(diffTex);
	if (normTex!="none")
	LoadTexture(normTex);

	LoadTexture(heightTex);

	_parallaxList.push_back(new RenderableObject(meshKey));

	_parallaxList[_parallaxList.size()-1]->SetTexture(MapType::diffuse,diffTex);
	_parallaxList[_parallaxList.size()-1]->SetTexture(MapType::normal,normTex);
	_parallaxList[_parallaxList.size()-1]->SetTexture(MapType::height,heightTex);

	_parallaxList[_parallaxList.size()-1]->SetScale(scale[0],scale[1],scale[2]);
	_parallaxList[_parallaxList.size()-1]->Translate(pos[0],pos[1],pos[2]);
	_parallaxList[_parallaxList.size()-1]->Rotate(rot);
}
void AddSolidColorOBJ(string meshKey,cml::vector3f color,cml::vector3f pos, cml::vector3f scale, GLfloat rot)
{
	LoadMesh(meshKey);
	_solidColorList.push_back(new RenderableObject(meshKey));
	_solidColorList[_solidColorList.size()-1]->SetColor(color[0],color[1],color[2]);

	_solidColorList[_solidColorList.size()-1]->SetScale(scale[0],scale[1],scale[2]);
	_solidColorList[_solidColorList.size()-1]->Translate(pos[0],pos[1],pos[2]);
	_solidColorList[_solidColorList.size()-1]->Rotate(rot);
}

void SaveScene()
{
	ofstream file;
	file.open("savedScene.txt");
	for(int i=0;i<_normalMapList.size();i++)
	{
		file<<_normalMapList[i]->GetName()<<" "<<_normalMapList[i]->GetMeshKey()<<" "<<_normalMapList[i]->GetNormalMap()<<" "
			<<_normalMapList[i]->GetDiffuseMap()<<" "<<_normalMapList[i]->GetPosition()<<" "<<_normalMapList[i]->GetScale()<<" "<<
			_normalMapList[i]->GetOrientation()<<"\n";
	}

	file.close();
}
void LoadScene(string fileName)
{
	stringstream ss;

	ifstream file;
	file.open(fileName.c_str());
	string line;
	string name, mesh, diff, norm;
	cml::vector3f pos, scale;
	GLfloat x,y,z;
	GLfloat orient;

	while(getline(file,line))
	{
		ss.clear();
		ss.str("");
		
		ss<<line;
		ss>>name;ss>>mesh;ss>>norm;ss>>diff;ss>>x;ss>>y;ss>>z;pos.set(x,y,z);
		ss>>x;ss>>y;ss>>z;scale.set(x,y,z);ss>>orient;
		AddNormalMapOBJ(mesh,diff,norm,pos,scale,orient);
		_normalMapList[_normalMapList.size()-1]->SetName(name);
	}

	file.close();
}

void SetupScene()
{
	//set light for ocean scene:
	//lightPos.set(10.47f,5.96f,8.35f,1.0f);
	
	
	//light for room scene:
	lightPos.set(2.0f,0.0f,-3.0f,1.0f);
	
	//build scene
		LoadMesh("box.obj");
	skyBox=new RenderableObject("box.obj");//,&cubeMapTex,true,0.0f,0.0f,0.0f,0.0f,20.0,20.0,20.0);
	skyBox->SetScale(1.5,1.5,1.5);
	skyBox->SetFliped(true);

//	AddNormalMapOBJ("extrudedBox.obj","red.jpg","noise.jpg",cml::vector3f(0.0,-1.5,0.0),cml::vector3f(0.5,0.5,0.5),0.0f);
	//AddNormalMapOBJ("stego.obj","stego.jpg","stego_normal.jpg",cml::vector3f(0.0,0.0,0.0),cml::vector3f(0.1,0.1,0.1),1.5f);
	
	//ddNormalMapOBJ("box.obj","rocks.png", "rockNorm.png",cml::vector3f(1.0,0.0,0.0),cml::vector3f(0.07,0.07,0.07),0.0f);

	//AddPOMObj("box.obj","rocks.png", "rockNorm.png","rockBump.png",cml::vector3f(-1.0,0.0,0.0),cml::vector3f(0.07,0.07,0.07),0.0f);

	//AddNormalMapOBJ("box.obj","concrete.bmp","boxNormal.jpg",cml::vector3f(0.0,-1.5,0.0),cml::vector3f(0.4,0.03,0.4),1.2f);
	
	
	//box1here
	//AddNormalMapOBJ("box.obj","green.jpg","noise.jpg",cml::vector3f(1.0,0.5,1.0),cml::vector3f(0.05,0.1,0.05),0.0f);
	
	
	//AddNormalMapOBJ("table.obj","bench1.jpg","noise.jpg",cml::vector3f(0.0,-1.5,0.0),cml::vector3f(0.1,0.1,0.1),1.2f);
	//_normalMapList[_normalMapList.size()-1]->SetName("table1");
	//AddNormalMapOBJ("stego.obj","stego.jpg","stego_normal.jpg",cml::vector3f(0.0,-1.5,0.0),cml::vector3f(0.1,0.1,0.1),1.2f);
	//AddSolidColorOBJ("box.obj",cml::vector3f(0.4,0.4,0.2),cml::vector3f(0.0,-1.5,0.0),cml::vector3f(0.4,0.03,0.4),0.0);
	//	_normalMapList.push_back(new RenderableObject(_meshList["box"],_textureList["rex"],false,0.0f,-1.8f,0.0f,0.0f,0.4f,0.03f,0.4f));//second object

	//AddNormalMapOBJ(,"yellow.jpg","noise.jpg",cml::vector3f(0.0,0.0,0.0),cml::vector3f(0.1,0.05,0.1),0.5);
	_reflectionList.push_back(new RenderableObject("water.obj"));
	LoadMesh("water.obj");
	_reflectionList[_reflectionList.size()-1]->SetScale(0.2,0.02,0.2);
		_reflectionList[_reflectionList.size()-1]->Translate(0.0,0.0,0.0);
		_reflectionList[_reflectionList.size()-1]->Rotate(0.0);

	_reflectionList[_reflectionList.size()-1]->SetName("water");
	_reflectionList[_reflectionList.size()-1]->isWater=true;
	_reflectionList[_reflectionList.size()-1]->textureScale=1;
	
	LoadScene("savedScene.txt");
	
	//box2 here
	//AddNormalMapOBJ("box.obj","wood.png","noise.jpg",cml::vector3f(0.0,0.0,0.0),cml::vector3f(0.1,0.02,0.1),1.5);
	//_normalMapList[_normalMapList.size()-1]->textureScale=1;
		
	
	
	//_normalMapList[_normalMapList.size()-1]->isWater=true;
//
//AddNormalMapOBJ("stego.obj","stego.jpg","stego_normal.jpg",cml::vector3f(0.0,0.0,0.0),cml::vector3f(0.1,0.1,0.1),1.5f);	_normalMapList[_normalMapList.size()-1]->isWater=true;
//	_normalMapList[_normalMapList.size()-1]->textureScale=1;
//	_normalMapList[_normalMapList.size()-1]->isWater=true;



		LoadMesh("particle.obj");
		//test particle system
		fire1=new ParticleSystem(0.2,cml::vector3f(0.0,0.0,0.0 /*2.0f,0.0f,-3.0f*/),particleSystemType::fire,0.45f);
		fire1->Start();
	//	smoke1=new ParticleSystem(0.2,cml::vector3f(0.0,0.0,0.0),particleSystemType::smoke);
		//smoke1->Start();
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		 glBlendFunc (GL_ONE, GL_ONE);
		 
}

void LoadShaders() 
{

	colorDotShader.InitShader("shaders\\ColorDotVert.glsl","shaders\\GoraudFrag.glsl");


	skyBoxPass.InitShader("shaders\\skyBoxVert.glsl","shaders\\ReflectionFrag.glsl"); //skybox using same vert shader as reflection
	POMPass.InitShader("shaders\\TextureVert.glsl","shaders\\POMFrag.glsl");
	normalMapPass.InitShader("shaders\\TextureVert.glsl","shaders\\normalMapFrag.glsl");

	quadShader.InitShader("shaders\\quadVert.glsl","shaders\\quadFrag.glsl");

	TextureViewer::_quad.Load("models\\quad.obj");

	buildGBufferPass.InitShader("shaders\\Deffered\\BuildGBufferVert.glsl","shaders\\Deffered\\BuildGBufferFrag.glsl");
	deferredNormalMap.InitShader("shaders\\quadVert.glsl","shaders\\Deffered\\defferedNormalSSAO.glsl");

		reflectionShader.InitShader("shaders\\reflectionVert.glsl","shaders\\ReflectionFrag.glsl");
	//	refractionShader.InitShader("shaders\\refractionVert.glsl","shaders\\ReflectionFrag.glsl");

	//	verticalBlurPass.InitShader("shaders\\quadVert.glsl","shaders\\verticalBlurFrag.glsl");
	//	horizontalBlurPass.InitShader("shaders\\quadVert.glsl","shaders\\horizontalBlurFrag.glsl");
	//	brightExtractionPass.InitShader("shaders\\quadVert.glsl","shaders\\extractFrag.glsl");
	//	diffuseTexturePass.InitShader("shaders\\worldCordWithTexVert.glsl","shaders\\diffuseIrrFrag.glsl");
	//	additivePass.InitShader("shaders\\quadVert.glsl","shaders\\additionFrag.glsl");
	goraudPass.InitShader("shaders\\GoraudVert.glsl","shaders\\GoraudFrag.glsl");
	buildShadowMap.InitShader("shaders\\ShadowVert.glsl","shaders\\ShadowFrag.glsl");
}


void init(void)
{	

	GetRandomKernel();

	lightDir=cml::normalize(lightDir);
	//test load model:
	//testShader.InitShader("PhongVert.glsl","PhongFrag.glsl");
	//set up this shader:
	//GLuint MVHandle,MVPHandle,normalMHandle,cameraMovementHandle;//handleID to view, projection, normal matrix
	//	GLuint modeHandle,isCubeMapHandle;//handle to mode uniform
	//add uniform:

	////get location of the uniform offset in vertex shader"
	//	//offsetLocation = glGetUniformLocation(theProgram, "offset");
	//	MVHandle=glGetUniformLocation(pHandle, "MV");
	//	MVPHandle=glGetUniformLocation(pHandle, "MVP");
	//	normalMHandle=glGetUniformLocation(pHandle, "normalTransform");
	//	modeHandle=glGetUniformLocation(pHandle, "mode");
	//	isCubeMapHandle=glGetUniformLocation(pHandle, "isCubeMap");

	//testShader.AddUniform("MV"); //handle=1
	//testShader.AddUniform("MVP");//		=2
	//testShader.AddUniform("normalTransform");//5
	//testShader.AddUniform("mode");//4
	//testShader.AddUniform("isCubeMap");//3
	//note: hard code so dont need to look up eveetyime, ->better performance
	//testShader.Activate();	

	LoadShaders();


	//we can change the coordinate from world coordinate or view coordinate with the
	//loaded vertex shader
	//outputToGPass.InitShader("viewCordNoTexVert.glsl","deferredtoGFrag.glsl");
	//deferredSecondPass.InitShader("quadVert.glsl","ssaoFrag.glsl");
	//GLfloat weights[10];
	//the line below is a tool to write data out a text file(blur.txt).
	//SetupWeight(10,3.0);


	//verticalBlurPass.SendFloatArrayToUni("Weight",10,weights);
	//horizontalBlurPass.SendFloatArrayToUni("Weight",10,weights);
	//	GLuint test1=glGetUniformLocation(verticalBlurPass.GetProgramHandle(),"Weight[1]");

	//SetupWeight();
	//shader2.Activate();

	//testing second shader:
	//GLuint phongIndex =
	//glGetSubroutineIndex(testShader.GetProgramHandle(),
	//             GL_VERTEX_SHADER,           "Phong");
	//glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &phongIndex);

	SetUpCamera();
	//LoadResources();
	SetUpFBOs();

	SetupScene();
	SetUpStaticUniforms();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

}

void UpdateKeyboard()
{
	long current=glutGet(GLUT_ELAPSED_TIME);

	if (keysPressed['a'])
	{
		_cameraPosition[0]+=CAMERA_MOVEMENT_SPEED*_cameraTagent[0];
		_cameraPosition[2]+=CAMERA_MOVEMENT_SPEED*_cameraTagent[2];
	}
	else if (keysPressed['d'])
	{
		_cameraPosition[0]-=CAMERA_MOVEMENT_SPEED*_cameraTagent[0];
		_cameraPosition[2]-=CAMERA_MOVEMENT_SPEED*_cameraTagent[2];
	}
	if (keysPressed['w'])
	{
		_cameraPosition[0]+=CAMERA_MOVEMENT_SPEED*_cameraDirection[0];
		_cameraPosition[2]+=CAMERA_MOVEMENT_SPEED*_cameraDirection[2];
	}
	else if (keysPressed['s'])
	{
		_cameraPosition[0]-=CAMERA_MOVEMENT_SPEED*_cameraDirection[0];
		_cameraPosition[2]-=CAMERA_MOVEMENT_SPEED*_cameraDirection[2];
	}
	else if (keysPressed['o'])//increase parrallax weight
	{
		////rotate the light:
		//cml::vector3f newLightPos;
		//newLightPos[0]=lightPos[0];newLightPos[1]=lightPos[1]-0.1;newLightPos[2]=lightPos[2];
		////newLightPos=cml::rotate_vector(newLightPos,OY,0.005f);

		//lightPos.set(newLightPos[0],newLightPos[1],newLightPos[2],1.0);

		////	lightPos[1]+=0.2f;

		_cameraPosition[1]+=0.01f;


		if (parallaxWeight+0.1<1.0f)
			parallaxWeight+=0.1f;
	}
	else if (keysPressed['p'])//decrease parrallax
	{	//lightPos[1]-=0.2f;
		//rotate the light:
		//cml::vector3f newLightPos;
		//newLightPos[0]=lightPos[0];newLightPos[1]=lightPos[1]+0.1;newLightPos[2]=lightPos[2];
		////newLightPos=cml::rotate_vector(newLightPos,OY,-0.005f);

		//lightPos.set(newLightPos[0],newLightPos[1],newLightPos[2],1.0);

			_cameraPosition[1]-=0.01f;
		if (parallaxWeight-0.1>0.0f)
			parallaxWeight-=0.1f;
	}
	//scene manipulation:
	else if (keysPressed[49]&&current-lastTick>400)    //press 1
	{
		lastTick=current;
		if(currentObjectSelection>0) 
		{
				currentObjectSelection--;
				cout<<"Select: "<<_normalMapList[currentObjectSelection]->GetName()<<"\n";
		}
	}
	else if (keysPressed[127]&&current-lastTick>400)    //press delete 
	{
		lastTick=current;
		if(currentObjectSelection<_normalMapList.size()>0) 
		{
			cout<<"Delete: "<<_normalMapList[currentObjectSelection]->GetName()<<"\n";
			DeleteNormalMapObj(currentObjectSelection);
			currentObjectSelection--;
			if(currentObjectSelection<0) currentObjectSelection=0;
			
		}
	}
	else if (keysPressed[32]&&current-lastTick>400)    //press spae to save 
	{
		lastTick=current;
		if(currentObjectSelection<_normalMapList.size()>0) 
		{
			SaveScene();

			cout<<"Scene saved\n";
		}
	}
	else if (keysPressed[51]&&current-lastTick>400) //press 3
	{
		lastTick=current;
		if(currentObjectSelection<_normalMapList.size()-1) 
		{
			currentObjectSelection++;
			cout<<"Select: "<<_normalMapList[currentObjectSelection]->GetMeshKey()<<"\n";
		}
	}
	else if (keysPressed[55]&&current-lastTick>400) //press 7 to choose x axis
	{
		lastTick=current;
		
			currentAxisSelection=0;
			cout<<"Transform X Axis ";
	}
	else if (keysPressed[56]&&current-lastTick>400) //press 8 to choose y axis
	{
		lastTick=current;

		currentAxisSelection=1;
		cout<<"Transform Y Axis ";
	}
	else if (keysPressed[57]&&current-lastTick>400) //press 9 to choose z axis
	{
		lastTick=current;

		currentAxisSelection=2;
		cout<<"Transform Z Axis ";
	}
	else if (keysPressed[52]) //press 4 to decrease
	{	cml::vector3f offset;
		offset.zero();
		offset[currentAxisSelection]-=MOVING_SPEED;
		_normalMapList[currentObjectSelection]->Translate(offset);
		cout<<"x: "<<_normalMapList[currentObjectSelection]->GetPosition()[0]
		<<" y: "<<_normalMapList[currentObjectSelection]->GetPosition()[1]<<" z: "<<_normalMapList[currentObjectSelection]->GetPosition()[2];
	}
	else if (keysPressed[54]) //press 4 to decrease
	{	cml::vector3f offset;
	offset.zero();
	offset[currentAxisSelection]+=MOVING_SPEED;
	_normalMapList[currentObjectSelection]->Translate(offset);
	cout<<"x: "<<_normalMapList[currentObjectSelection]->GetPosition()[0]
	<<" y: "<<_normalMapList[currentObjectSelection]->GetPosition()[1]<<" z: "<<_normalMapList[currentObjectSelection]->GetPosition()[2];
	}
//	else if (keysPressed['m'])
//	{
//		//increase scale:
//		g_scale+=0.004*sigN;
//		deferredNormalMap.Activate(); 
//		deferredNormalMap.SendFloatToUni("g_scale",g_scale);
//		cout<<"g_scale: "<<g_scale<<endl;
//		/*GLfloat g_scale=0.8f;
//		GLfloat g_intensity=0.8f;
//		GLfloat g_bias=0.45f;
//		GLfloat g_sampleRad=0.2;
//		GLfloat sigN=1.0;
//		
//*/
//	}
//	else if (keysPressed['n'])
//	{
//		//intensity
//		g_intensity+=0.01*sigN;
//		deferredNormalMap.Activate(); 
//		deferredNormalMap.SendFloatToUni("g_intensity",g_intensity);
//		cout<<"g_intensity: "<<g_intensity<<endl;
//
//	}
//	else if (keysPressed['b'])
//	{
//		//bias
//		g_bias+=0.01*sigN;
//		deferredNormalMap.Activate(); 
//		deferredNormalMap.SendFloatToUni("g_bias",g_bias);
//		cout<<"g_bias: "<<g_bias<<endl;
//	}
//	else if (keysPressed['v'])
//	{
//		//g_sampleRad
//		g_sampleRad+=0.01*sigN;
//		deferredNormalMap.Activate(); 
//		deferredNormalMap.SendFloatToUni("g_sampleRad",g_sampleRad);
//		cout<<"g_sampleRad: "<<g_sampleRad<<endl;
//	}
	else if (keysPressed['c']&&current-lastTick>700)
	{
		lastTick=current;
		if(ssaoOn) ssaoOn=false;
		else ssaoOn=true;

		deferredNormalMap.Activate(); 
		deferredNormalMap.SendFloatToUni("ssaoOn",ssaoOn);
		
	}

}
int lastUpdateTick=0;

void Update(int value)
{
//	cout<<"x: "<<_cameraPosition[0]<<" y: "<<_cameraPosition[1]<<" z:"<<_cameraPosition[2]<<endl;
	//rotate the light:
	//cml::vector3f newLightPos;
	//newLightPos[0]=lightPos[0];newLightPos[1]=lightPos[1];newLightPos[2]=lightPos[2];
	//newLightPos=cml::rotate_vector(newLightPos,OY,0.005f);
	textureOffset+=0.01f;
	//slow down the fire
	int currentTick=glutGet(GLUT_ELAPSED_TIME);
	
	//lightPos.set(newLightPos[0],newLightPos[1],newLightPos[2],1.0);
	
	fire1->Update(currentTick,currentTick-lastUpdateTick);
		//smoke1->Update();
		lastUpdateTick=currentTick;
	UpdateKeyboard();
	UpdateCamera();
	glutTimerFunc(15, Update, 0);
	glutPostRedisplay();


}
#pragma region userInput
void processNormalKeysUp(unsigned char key, int x, int y)
{
	if (key>=0&&key<=254)
	{
		keysPressed[key]=false;
	}
	//}
	//if (key == 'd')
	//{	
	//	keysPressed[keys::d]=false;
	//}
	//if (key == 's')
	//{	
	//	keysPressed[keys::s]=false;
	//}
	//if (key == 'w')
	//{		keysPressed[keys::w]=false;
	//}
	//if (key == 'o')
	//{		keysPressed[keys::o]=false;
	//}
	//if (key == 'p')
	//{		keysPressed[keys::p]=false;
	//}

}
void processNormalKeys(unsigned char key, int x, int y) 
{
	if (key>=0&&key<=254 )
	{
		keysPressed[key]=true;

	}
	//if (key == 'd')
	//{	
	//	keysPressed[keys::d]=true;
	//}
	//if (key == 's')
	//{	
	//	keysPressed[keys::s]=true;
	//}
	//if (key == 'w')
	//{		keysPressed[keys::w]=true;
	//}
	//if (key == 'o')
	//{		keysPressed[keys::o]=true;
	//}
	//if (key == 'p')
	//{		keysPressed[keys::p]=true;
	//}


}
void processMouse(int x, int y)
{
	_cameraDirection.set(0.0,0.0,1.0);
	_cameraTagent.set(1.0,0.0,0.0);
	int displacementX=x-W_WIDTH/2.0;
	int displacementY=y-W_HEIGHT/2.0;
	_cameraDirection=cml::rotate_vector(_cameraDirection,OY,(-displacementX)*CAMERA_TURN_SPEED);

	_cameraTagent=cml::rotate_vector(_cameraTagent,OY,(-displacementX)*CAMERA_TURN_SPEED);
	_cameraDirection=cml::rotate_vector(_cameraDirection,_cameraTagent,(displacementY)*CAMERA_TURN_SPEED);
	//_cameraTagent=cml::rotate_vector(_cameraTagent,OZ,(displacementY)*CAMERA_TURN_SPEED);

}
void processSpecialKeys(int key, int x, int y)
{
	//hard code number of modes here
	if (key==GLUT_KEY_LEFT&&currentMode>0)
		currentMode--;
	else if (key==GLUT_KEY_RIGHT&&currentMode<ShaderMode::COUNT-1)
		currentMode++;
}
#pragma endregion userInput

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize( W_WIDTH, W_HEIGHT);
	glViewport(0,0, W_WIDTH, W_HEIGHT);
	glutCreateWindow("Tung Demo");
	glutPassiveMotionFunc(processMouse);
	glutDisplayFunc(display); 
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutSpecialFunc(processSpecialKeys);

	glutTimerFunc(15, Update, 0);
	init();
	glutWarpPointer(W_WIDTH/2.0,W_HEIGHT/2.0);
	glutMainLoop();
	return 0;
}

