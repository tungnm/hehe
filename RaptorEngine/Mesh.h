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
namespace Raptor
{
	//mesh: VAO handle, number of face
	//MeshRenderData has the VAO ID and nFaces of the mesh, will be used for rendering
	struct MeshRenderData
	{
		GLuint VAOHandle;
		int nFaces;

		//below are handles to VBOs of each mesh. The outsider will probally dont need these information
		//they only need to use VAOhandle and nface to draw a mesh. However, we still need to retain all other VBO handles so that
		//we can use glDeleteBuffer on them later to free memory
		GLuint posVBOHandle,normVBOHandle,texVBOHandle,tanVBOHandle,indiceHandle;
	};
//this singleton mesh manager class has a hash table of <texturename, MeshRenderDatar>
class MeshManager {

private:

	unordered_map<string, MeshRenderData> _meshList;

	//note that all vertex attribute arrays( pos, norm, tex, tant) have the same size=_nVertices, because Assimp already do the indexing when loadin mesh
	//this function return the MeshRenderData with all the generated VBO handles stored in it
	MeshRenderData UploadVertexData(aiVector3D * position,aiVector3D *normal,GLfloat * texture,GLfloat * tangent,unsigned int * indice,unsigned int nVerts,unsigned int nFaces);

public:
	MeshManager(){};
	//destructor:
	~MeshManager(){};

	static MeshManager* GetInstance()
	{
		static MeshManager singleton;
		return &singleton;
	}

	/**
	 * Return mesh data for rendering call 
	 */
	MeshRenderData GetMeshRenderData(string name)
	{
		return _meshList[name]; 
	};
	/**
	* Init mesh manager
	*/
	void StartUp();

	/**
	 * Free all VBOs, VAOs of all loaded meshes.
	 */
	void ShutDown();

	/**
	 * this function read an .obj file, and push the data to the GPU and keep track of the 
	 * loaded mesh data for rendering( the VAO handle and the number of faces)
	 * vertex normal buffer and face indice buffer
	 */
	void LoadObjFile(string fileName); 

	/*
	 * Check if the mesh with the string key has been loaded
	 */
	bool IsMeshExist(string meshKey);

};
}
#endif
