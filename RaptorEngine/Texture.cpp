
#include "Texture.h"
#include <iostream>
#include "SOIL.h"
#include <stdio.h>
#include "UtilityFunctions.h"
#include <string.h>

using namespace std;

void TextureManager::StartUp()
{
	_count=0;
	_cubeMapID=-1;
	cout<<"Texture Manager started...";
	cout<<"Done\n";
}
void TextureManager::ShutDown()
{
	//delete textures to free memory
	cout<<"Texture Manager is shuting down...";
	for(std::unordered_map<string, GLuint>::iterator iter =  _textureList.begin(); iter != _textureList.end(); ++iter)
	{
	 glDeleteTextures(1, &iter->second);
	}
	cout<<"Done\n";
}
int TextureManager::GetTextureID(string textureName)
{
	return _textureList[textureName];
}
	void  TextureManager::LoadCubeMap(string forward,string backward,string top, string bottom, string right, string left)
	{
		
		int textureID=_count+1;// need to assign each texture to each texture unit
		_count++;

		glActiveTexture(GL_TEXTURE0+textureID);
		cout<<"Loading Environment Map...";

		GLuint tex_cube = SOIL_load_OGL_cubemap(
			forward.c_str(),//right   //note this is is in reverse order for left and right?
			backward.c_str(),//left
			top.c_str(),//top
			bottom.c_str(),//bottom
			right.c_str(),//forward
			left.c_str(),//backward
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
			);
		if (tex_cube<=0) cout<<"--Error: can't load environment Map texture\n";
		else cout<<"Done\n";

		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
		// Typical cube map settings
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		_cubeMapID=textureID;
	}

	
	void TextureManager::LoadSingleImage(string filePath)
	{	
		if(_count==0)
			cout<<"Loading Textures:\n";
		cout<<"* "<< filePath <<"\n";
		string key=filePath;
		filePath = "textures\\" + filePath;
		if (Utility::CheckFileExist(filePath)==false)
			return;

		int textureID=_count+1;
		_count++;
	
		glActiveTexture(GL_TEXTURE0+textureID);
		
		GLuint textureHandles = SOIL_load_OGL_texture
			(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y
			|SOIL_FLAG_MIPMAPS);//this function generates a texture buffer object in GPU memory and return a handle to it

		//if(texture[0] == 0)
		//return false;
		if (textureHandles<=0) cout<<"--Error: can't load "<<filePath.c_str()<<"\n";else
			cout<<"Done\n";
		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, textureHandles);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	//	GLfloat border[]={1.0f,0.0f,0.0f,0.0f};
// 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
// 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	//glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,border);

		//add this new texture to hash table:
		
		_textureList[key]=textureID;
	}

	bool TextureManager::ExistTexture(string textureFileName)
	{
		if(_textureList.count(textureFileName)<1)
			return false;
		else
		{
			return true;
		}
	}

int TextureManager::ReserveTexture()
{
	_count++;
	return _count;

}
