#include "FBO.h"
#include "Texture.h"



GLuint ShadowFBO::GetTextureHandle()
{
	return _textureHandle;
}

void ShadowFBO::SetUp(FBO::FBOInfo info,TextureManager* textureman)
{
	_width=info.width;
	_height=info.height;
	
	//The shadow maptexture

	glGenTextures(1,&_textureHandle);
	
//	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());

	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());
	glBindTexture(GL_TEXTURE_2D,_textureHandle);
	
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	GLfloat border[]={1.0f,0.0f,0.0f,0.0f};
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,border);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LESS);

	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32,info.width,info.height,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,NULL);
	
	//Reserve a texture unit for this FBO, FBO only need to know the texture handle, it does not need to create
	//any texture object.
	


	//Create and set up the FBO
	glGenFramebuffers(1,&_fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER,_fboHandle);
	//texture handle here is actually the depth texture
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_textureHandle,0);
	/*glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_normalTexHanle,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_positionTexHandle,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D,_albedoTexHandle,0);*/

	GLenum drawBuffers[]={GL_NONE};/*, GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};*/
	glDrawBuffers(0,drawBuffers);
	// Revert to the default framebuffer for now
	glBindFramebuffer(GL_FRAMEBUFFER,0);




}

GLuint SingleTexFBO::GetTextureHandle()
{
	return _textureHandle;
}

//has depth should be used for normal render pass( render list of Renderable objects), post processing pass dont need depth buffer
void SingleTexFBO::SetUp(FBO::FBOInfo info,TextureManager* textureman)
{
	_width=info.width;
	_height=info.height;
	//create EMPTY texture ojbect:
	GLuint renderTexture;
	glGenTextures(1,&renderTexture);
	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());

	_textureHandle=renderTexture;
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,info.width,info.height,0, GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	//-----------------------------
	GLuint depthHandle,colorHandle[1];
	//create framebuffer object
	glGenFramebuffers(1,&_fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboHandle);

	//bind the texture to the FBO:
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_textureHandle,0);

	//create depth buffer if required:
	if (info.hasDepth)
	{
		glGenRenderbuffers(1,&depthHandle);
		glBindRenderbuffer(GL_RENDERBUFFER,depthHandle);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,info.width,info.height);
		//bind depth buffer to FBO:
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthHandle);
	}
	// Set the target for the fragment shader outputs
	GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBufs);//this function only specify the output framebuffer for the shader,
	//not actually draw anything


	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void FBO::SetDefault(int W_WIDTH, int W_HEIGHT)
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport (0, 0, W_WIDTH, W_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
void FBO::SetActive()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fboHandle);
	glViewport (0, 0,_width , _height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
GLuint GBuffer::GetGNormTexHandle()
{
	return _gNormTexHandle;
}
GLuint GBuffer::GetGPosTexHandle()
{
	return _gPosTexHandle;
}
GLuint  GBuffer::GetGDiffuseTexHandle()
{
	return _gDiffuseHandle;
}
GLuint GBuffer::GetGNormalMapTexHandle()
{
	return _gNormalMapTexHandle;	
}
	GLuint GBuffer::GetGTanTexHandle()
	{
		return _gTangentHandle;
	}

void GBuffer::SetUp(FBO::FBOInfo info,TextureManager* textureman)
{


	//this should match the output of GBufferBuilder shader:
/*
layout (location = 0) out vec4 outputColor;
layout (location = 1) out vec3 posToG;
layout (location = 2) out vec4 normToG;
layout (location = 3) out vec3 diffuseToG;
layout (location = 4) out vec3 tanToG;
layout (location = 5) out vec3 normalMapToG;
*/
	_width=info.width;
	_height=info.height;

	//position:
	glGenTextures(1,&_gPosTexHandle);	
	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());
	glBindTexture(GL_TEXTURE_2D, _gPosTexHandle);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGB32F,info.width,info.height,0, GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	//normal is similar to position, we used GL_RGB32F
	//NOTE THAT, IN HERE WE STORE A LINEAR DEPTH IN ALPHA COMPONENT OF THE NORMAL , SO THE NORMAL WILL BE VEC4 instead of VEC3
	
	//normal
	glGenTextures(1,&_gNormTexHandle);
	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());

	glBindTexture(GL_TEXTURE_2D, _gNormTexHandle);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGB32F,info.width,info.height,0, GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	
	//Diffuse
	//Note that I used diffuse as GL_RGBA32F so it's a vec4. The reason is for optimization. Because skyboxGFrag.glsl and fireGFrag.glsl
	//only need to write out diffuse color data. So if we patch everything to diffuse texture then when writing to G-Buffer we only need
	//something like diffuseToG=vec4(diffuse, -200); -200 here is the trick to classify the type of the fragment(skybox or water or fire
	//or normalMap object..). it's optmized because if we use other texture for storing -200, then we need to write more, redundant data to G-Buffer
	glGenTextures(1,&_gDiffuseHandle);
	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());

	glBindTexture(GL_TEXTURE_2D, _gDiffuseHandle);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA32F,info.width,info.height,0, GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//tangent

	glGenTextures(1,&_gTangentHandle);
	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());
	
	glBindTexture(GL_TEXTURE_2D, _gTangentHandle);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGB32F,info.width,info.height,0, GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//normalMap:
	glGenTextures(1,&_gNormalMapTexHandle);
	glActiveTexture(GL_TEXTURE0+ textureman->ReserveTexture());

	glBindTexture(GL_TEXTURE_2D, _gNormalMapTexHandle);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGB32F,info.width,info.height,0, GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//depth buffer:
	
	glGenRenderbuffers(1,&_gDepthHandle);
	glBindRenderbuffer(GL_RENDERBUFFER,_gDepthHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,info.width,info.height);

	//create and bind the g buffer fBO:

	glGenFramebuffers(1,&_fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboHandle);

	//bind the texture to the FBO:
	//bind depth
	//depth buffer:

	glGenRenderbuffers(1,&_gDepthHandle);
	glBindRenderbuffer(GL_RENDERBUFFER,_gDepthHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,info.width,info.height);
	
	//bind depth:
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,_gDepthHandle);


	//bind other 3 pos, norm and diffuse:
	
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_gPosTexHandle,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_gNormTexHandle,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D,_gDiffuseHandle,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT3,GL_TEXTURE_2D,_gTangentHandle,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT4,GL_TEXTURE_2D,_gNormalMapTexHandle,0);
	

	//rignt now now diffuse yet, bind the linear depth to GL_COLOR_ATTACHMENT2
	/*
	
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D,_gLinearDepthHandle,0);
*/
	// Set the target for the fragment shader outputs
	GLenum drawBufs[] = {GL_NONE, GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4};
	glDrawBuffers(6, drawBufs);//this function only specify the output layout of framebuffer for the shader,
	//not actually draw anything


	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



FBO::FBO()
{

}

FBO::~FBO()
{

}

void FBO::Dellocate()
{
	glDeleteFramebuffers(1,&_fboHandle);
}


