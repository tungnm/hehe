#ifndef MESH_H
#define MESH_H
#include "glew.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include "assimp\Importer.hpp"
#include <unordered_map>

using namespace std;

//mesh: VAO handle, number of face
struct MeshInfo
{
	GLuint VAOHandle;
	int nFaces;

	//below are handles to VBOs of each mesh. The outsider will probally dont need these information
	//they only need to use VAOhandle and nface to draw a mesh. However, we still need to retain all other VBO handles so that
	//we can use glDeleteBuffer on them later to free memory
	GLuint posVBOHandle,normVBOHandle,texVBOHandle,tanVBOHandle,indiceHandle;
};

//this singleton mesh manager class has a hash table of <texturename, MeshInfor>
//meshInfo has the VAO ID and nFaces of the mesh, will be used for rendering

class MeshManager {
private:

	unordered_map<string, MeshInfo> _meshList;

	//note that all vertex attribute arrays( pos, norm, tex, tant) have the same size=_nVertices, because Assimp already do the indexing when loadin mesh
	//this function return the Meshinfo with all the generated VBO handles stored in it
	MeshInfo UploadVertexData(aiVector3D * position,aiVector3D *normal,GLfloat * texture,GLfloat * tangent,unsigned int * indice,unsigned int nVerts,unsigned int nFaces);

public:
	//unsigned int _nFaces;
	MeshManager(){};
	//destructor:
	~MeshManager(){};

	static MeshManager* GetInstance()
	{
		static MeshManager singleton;
		return &singleton;
	}
	MeshInfo GetMeshInfo(string name){return _meshList[name]; };
	
	void StartUp();
	//delete all buffers on shutdown, free GPU memory
	void ShutDown();
	//this function read an obj file, and push the data to the GPU then return the handles to vertex coordinate buffer,
	//vertex normal buffer and face indice buffer
	void Load(string fileName); //flip side it to flip the side of object front to back and vice versa;//end of Load

	bool ExistMesh(string meshKey);

};




#endif
