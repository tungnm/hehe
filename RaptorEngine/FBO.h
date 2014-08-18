#ifndef FBO_H
#define FBO_H
#include "glew.h"
#include "Texture.h"
namespace Raptor
{
class FBO
{
//a frame buffer object class hold a fboHandle for setting itself as current FBO using function SetAtive()
//the handle to the texture associate with it is for rendering this texture using a textureViewer


public:
	struct FBOInfo
{
	bool hasDepth;
	GLuint width;
	GLuint height;

};
	GLuint _fboHandle; 
	int _width;
	int _height;
	//note: a FBO will create a texture for itself, so we dont need to remember which texture 
	//bind to which FBO
	
	static void SetDefault(int W_WIDTH, int W_HEIGHT);
	
	void SetActive();
	//void SetUpSingleTexFBO(bool hasDepth,int width, int height);
	virtual void SetUp(FBOInfo info, TextureManager* textureman)=0;
	FBO();
	~FBO();
	virtual void  Dellocate()=0;
};


class SingleTexFBO:public FBO
{
private:
	GLuint _textureHandle;
	
public:
	
	GLuint GetTextureHandle();
	void SetUp(FBOInfo info,TextureManager* textureman); //override
	//delete the texture and FBO itself
	void Dellocate()
	{
		glDeleteTextures(1, &_textureHandle);
		glDeleteFramebuffers(1,&_fboHandle);
	}
};

class GBuffer:public FBO
{
private:
	//these attributes below is for g-buffer only:
	GLuint _gPosTexHandle;
	GLuint _gNormTexHandle;
	GLuint _gDiffuseHandle;
	GLuint _gNormalMapTexHandle;
	GLuint _gTangentHandle;


	//GLuint _gDiffTexHandle;
	GLuint _gDepthHandle;
	
public:
	void SetUp(FBOInfo info,TextureManager* textureman); //override
	GLuint GetGNormTexHandle();
	GLuint GetGPosTexHandle();
	GLuint GetGDiffuseTexHandle();
	GLuint GetGNormalMapTexHandle();
	GLuint GetGTanTexHandle();


	//GLuint GetLinearDepthTexHandle();
//	GLuint GetGDiffTexHandle();
	void Dellocate()
	{

	glDeleteTextures(1, &_gDiffuseHandle);
	glDeleteTextures(1, &_gNormalMapTexHandle);
	glDeleteTextures(1, &_gNormTexHandle);
	glDeleteTextures(1, &_gPosTexHandle);
	glDeleteTextures(1, &_gTangentHandle);
	glDeleteFramebuffers(1,&_fboHandle);
	}
};

class ShadowFBO:public FBO
{
private:
	GLuint _textureHandle; //depth
	//for indirect lighting
	/*GLuint _normalTexHanle;
	GLuint _positionTexHandle;
	GLuint _albedoTexHandle;*/
	
public:
	
	GLuint GetTextureHandle();
	/*GLuint GetNormalHandle();
	GLuint GetPosHandle();
	GLuint GetAlbedoandle();*/
	void SetUp(FBOInfo info,TextureManager* textureman); //override
	void Dellocate()
	{
	glDeleteTextures(1, &_textureHandle);
	glDeleteFramebuffers(1,&_fboHandle);
	}
};
}

#endif