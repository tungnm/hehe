#ifndef TEXTURE_H
#define TEXTURE_H
#include "glew.h"
#include <string>
#include <unordered_map>

using namespace std;

class TextureManager
{
	//Todo: right now texture object will not supposed to change in runtime, such as loading another image or texture object point to another c
	//texture unit(with glActivateTexture) or changing the sampler 
private:
	unordered_map<string, GLuint> _textureList;
	int _cubeMapID;//used only for cubeMap
	int _count;

public:
	//constructor and destructor would do nothing, use startUp and shutdown method to
	TextureManager(){};
	~TextureManager(){};

	static TextureManager* GetInstance()
	{
		static TextureManager singleton;
		return &singleton;
	}
	
	void StartUp();
	void ShutDown();
	//Interface to load textures to GPU memory
	void LoadCubeMap(string forward,string backward,string top, string bottom, string right, string left);
	void LoadSingleImage(string fileName);
	
	int GetTextureID(string textureName);//return the integer id of the texture, given it name such as "fire1.jpg"

	int GetCubeMapID(){return _cubeMapID;};
	bool ExistTexture(string textureFileName);

	int ReserveTexture(); //used by FBO to get the next available texture
};

#endif