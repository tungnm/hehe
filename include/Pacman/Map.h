#ifndef MAP_H
#define MAP_H
#include "glew.h"
#include "cml\cml.h"
#include <string>
#include "PhysicalBody.h"
#include "Renderer.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
typedef enum{dleft, dright, dup, ddown, upleft, upright, downleft, downright} Dir;
class Map
{
private:
	
	
	bool ** cornArray;
	GLfloat cellWidth;
	GLfloat cellHeight;
	cml::vector3f Origin;
	PhysicalBody* cellMesh;
	cml::vector2i GetVectorFromHash(int hashKey);
Dir GetDirection(int sourcei, int sourcej, int goali, int goalj);
int GetHashKey(cml::vector2i a);
	int beginningNCorn;
public:
	queue<Dir> GetPath(cml::vector2i start,cml::vector2i goal); 
	bool ** bitArray;
	int GetCornEatean(){return beginningNCorn-cornHash.size(); }
	unordered_set<int> cornHash;
	int nRows;
	int nColumns;
	int nCorns;
	Map();
	void PrintOutBitArray();
	void ReadBitArray(std::string fileName);
	void SetCellDimension(GLfloat width, GLfloat height);
	void SetMapOrigin(GLfloat x, GLfloat y, GLfloat z);
	void SetCellMesh(string meshKey, string diffTex, string normTex, cml::vector3f scale, GLfloat rot,Renderer* r1 );
	
	cml::vector3f GetCellPosition(int row, int col);
	cml::vector3f GetCenterPosition(GLfloat height);
	bool IsAtCenterOfCell(cml::vector2i x);
	cml::vector2i GetCellIndex(cml::vector3f);
	vector<Dir> GetPossibleDirection(cml::vector3f pos);

	unordered_set<Dir>  GetPossibleDirectionInSet(cml::vector2i currentCell,Dir except);

	vector<Dir> GetPossibleDirection(cml::vector2i currentCell,Dir except);//with exception
	void GetcornPosArray(GLshort *storearr,int* nCorn);
	void UpdateCornHash(GLshort* storearray, int* nCorn);
	bool CanMove(cml::vector2i currentCell, Dir currentDirection);
};

#endif