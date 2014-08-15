#include "Mesh.h"
#include "cml\cml.h"
#include "UtilityFunctions.h"

using namespace std;

//this template function is how to convert a string into any numeric type:
Assimp::Importer importer;

template <class T>
bool from_string(T& t, 
	const std::string& s, 
	std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}


void MeshManager::LoadObjFile( string fileName )
{
	if(_meshList.size()==0) cout<<"Loading Textures:\n";
	cout<<"* "<<fileName<<"\n";

	string key=fileName;

	fileName="models\\"+fileName;

	if(Utility::CheckFileExist(fileName)==false)
		return ;

	MeshRenderData newMesh; //the mesh info to be added


	const struct aiScene* scene=importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality );
	// For each mesh
	
	const struct aiMesh* mesh = scene->mMeshes[0];

	// have to convert from Assimp format to array
	//indices array( mesh has been already indexed by Assimp), size = nVertices=nFace*3	
	unsigned int *faceArray; 
		faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		//loop through each face: this loop copy the all indices to a 1D array, from set of faces stored in mesh->mFaces
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) 
		{
			const struct aiFace* face = &mesh->mFaces[t];

			memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(float));
			faceIndex += 3;
		}
		//aMesh.numFaces = sc->mMeshes[n]->mNumFaces;
	
		int _nVertices=scene->mMeshes[0]->mNumVertices;

		float *texCoords = new float[2*mesh->mNumVertices];
		for (unsigned int k = 0; k < mesh->mNumVertices; ++k) 
		{

			texCoords[k*2]   = mesh->mTextureCoords[0][k].x;
			texCoords[k*2+1] = mesh->mTextureCoords[0][k].y; 

		}
		//build tangent array with handedness:

		GLfloat *tang=(GLfloat *)malloc(sizeof(GLfloat)*4*mesh->mNumVertices);
		for(int i = 0; i < mesh->mNumVertices; ++i) 
		{
			cml::vector3f n(mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z);
			cml::vector3f t(mesh->mTangents[i].x,mesh->mTangents[i].y,mesh->mTangents[i].z);
			cml::vector3f b(mesh->mBitangents[i].x,mesh->mBitangents[i].y,mesh->mBitangents[i].z);
			
			tang[i*4]=t[0];
			tang[i*4+1]=t[1];
			tang[i*4+2]=t[2];
			//determine handedness:
		//	 tangent[a].w =1.0f;// (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
			
			cml::normalize(t);
			cml::normalize(b);
			cml::normalize(n);
			GLfloat a=cml::dot(cml::cross(t, b), n);

		//	GLfloat w=( a< 0.0f) ? -1.0f : 1.0f;
			tang[i*4+3]=1.0f;

		}
		newMesh=UploadVertexData(scene->mMeshes[0]->mVertices,scene->mMeshes[0]->mNormals,texCoords,tang,faceArray,scene->mMeshes[0]->mNumVertices,scene->mMeshes[0]->mNumFaces);
		newMesh.nFaces=scene->mMeshes[0]->mNumFaces;
		
		//add this new map to hash table
		_meshList[key]=newMesh;

		delete []faceArray;
		delete []texCoords;
		delete []tang;
		

	return;
}

MeshRenderData MeshManager::UploadVertexData(aiVector3D * position,aiVector3D *normal,GLfloat * texture,GLfloat * tangent,unsigned int * indice,unsigned int nVerts,unsigned int nFaces)
{
	MeshRenderData result;
	
	// push data to GPU
	GLuint arrayHandle[1];
	glGenVertexArrays(1,arrayHandle);

	result.VAOHandle=arrayHandle[0];

	MeshRenderData error;
	error.VAOHandle=-1;
	error.nFaces=0;
	if(result.VAOHandle<=0) {cout<<"Error, cannot generate VAO\n";return error;}
	glBindVertexArray(result.VAOHandle);

	//generate 4 handles for VBOs in GPU to store: position, normal, texCord, tangent
	GLuint handles[5];
	glGenBuffers(5,handles);
	
	result.posVBOHandle=handles[0];
	result.normVBOHandle=handles[1];
	result.texVBOHandle=handles[2];
	result.tanVBOHandle =handles[3];
	result.indiceHandle=handles[4];

	//create copy data to, and bind attribute pointer for posVBO
	glBindBuffer(GL_ARRAY_BUFFER,result.posVBOHandle);
	glBufferData(GL_ARRAY_BUFFER,nVerts*3*sizeof(float),position,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);//enable vertex attribute location 0: vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //Note that this function uses the currently binded VBO to set the attribute pointer to
	//( right now is the vertex coordinate VBO: posVBO

	//create copy data to, and bind attribute pointer for normVBO
	glBindBuffer(GL_ARRAY_BUFFER,result.normVBOHandle);
	glBufferData(GL_ARRAY_BUFFER,nVerts*3*sizeof(float),normal,GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);//enable vertex attribute location 1: normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); 

	//texture VBO:
	glBindBuffer(GL_ARRAY_BUFFER,result.texVBOHandle);
	glBufferData(GL_ARRAY_BUFFER,nVerts*2*sizeof(float),texture,GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);//enable vertex attribute location 2: textureCoordinate
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); 

	//tangent VBO:
	glBindBuffer(GL_ARRAY_BUFFER,result.tanVBOHandle);
	glBufferData(GL_ARRAY_BUFFER,nVerts*4*sizeof(float),tangent,GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);//enable vertex attribute location 3: tangent
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0); 

	//indice VBO:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.indiceHandle);//note that for indice, use GL_ELEMENT_ARRAY_BUFFER instead of GL_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nFaces* 3, indice, GL_STATIC_DRAW);
	//note that nFaces*3 is not necessary to = nVerts, because depending on how idexing done by assimp, nVerts may < nFaces*3
	
	//done, unbind VBO, VAO:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	return result;
}

bool MeshManager::IsMeshExist(string meshFileName)
	{
		if(_meshList.count(meshFileName)<1)
			return false;
		else
		{
			return true;
		}
	}
void MeshManager::StartUp()
{
	cout<<"Mesh Manager is starting up...";
	cout<<"Done\n";
}

void MeshManager::ShutDown()
{
	cout<<"Mesh Manager is shuting down...";
	for(std::unordered_map<string, MeshRenderData>::iterator iter =  _meshList.begin(); iter != _meshList.end(); ++iter)
	{
		MeshRenderData current=iter->second;
		glDeleteBuffers(1, &current.indiceHandle);
		glDeleteBuffers(1, &current.normVBOHandle);
		glDeleteBuffers(1, &current.posVBOHandle);
		glDeleteBuffers(1, &current.tanVBOHandle);
		glDeleteBuffers(1, &current.texVBOHandle);

		glDeleteVertexArrays(1,&current.VAOHandle);

	}
	cout<<"Done\n";
}