#include "Pacman\Map.h"
#include <fstream>
#include "UtilityFunctions.h"
#include <iostream>
#include <vector>
#include <deque>

using namespace std;
Map::Map()
{
	nRows=0;
	nColumns=0;
	cellWidth=0.0;
	cellHeight=0.0;

}

void Map::ReadBitArray(std::string fileName)
{
	ifstream inputFile;
	inputFile.open(fileName);
	string line;
	getline(inputFile, line);
	nRows=Utility::ToInt(line);
	getline(inputFile, line);
	nColumns=Utility::ToInt(line);
	//init bit array:
	bitArray=new bool*[nRows];
	cornArray=new bool*[nRows];

	for(int i=0;i<nRows;i++)
	{	bitArray[i]=new bool[nColumns];
	cornArray[i]=new bool[nColumns];
	}

	nCorns=0;

	for(int i=0;i<nRows;i++)
	{
		getline(inputFile,line);
		for(int j=0;j<nColumns;j++)
		{
			if(line[j]=='0'||line[j]=='2')
				bitArray[i][j]=false;
			else bitArray[i][j]=true;
			if(line[j]=='2')
			{	cornArray[i][j]=true;nCorns++;}
			else cornArray[i][j]=false;
		}
	}
}
void Map::PrintOutBitArray()
{

	for(int i=0;i<nRows;i++)
	{
		cout<<"\n";
		for(int j=0;j<nColumns;j++)
		{

			cout<<bitArray[i][j];
		}

	}
}
void Map::SetCellDimension(GLfloat width, GLfloat height)
{
	cellHeight=height;
	cellWidth=width;
}
void Map::SetMapOrigin(GLfloat x, GLfloat y, GLfloat z)
{
	Origin.set(x,y,z);
}

void Map::UpdateCornHash(GLshort* storearray,int* nCorn)
{
	*nCorn=cornHash.size();
	int index=0;
	for (unordered_set<int>::iterator it = cornHash.begin(); 
		it != cornHash.end(); ++it) 
	{
		storearray[index]=*it/nColumns;
		storearray[index+1]=*it%nColumns;
		index+=2;
	}
}


Dir Map::GetDirection(int sourcei, int sourcej, int goali, int goalj)
{
	if(goalj==sourcej-1)
		return dleft;
	else if (goalj==sourcej+1)
		return dright;
	else if (goali==sourcei+1)
		return ddown;
	else if (goali==sourcei-1)
		return dup;
}
cml::vector2i Map::GetVectorFromHash(int hashKey)
{
	return cml::vector2i(hashKey/nColumns, hashKey%nColumns );
}
int Map::GetHashKey(cml::vector2i a)
{
	return (a[0]*nColumns+a[1] );
}

queue<Dir> Map::GetPath(cml::vector2i startNode, cml::vector2i goalNode)
{
	unordered_set<int> marked;
	unordered_map<int,int> parentOf;
	bool found=false;

	cml::vector2i currentNode;
	queue<cml::vector2i> nodeQueue;
	nodeQueue.push(startNode);
	marked.insert(GetHashKey(startNode));
	do
	{
		currentNode=nodeQueue.front();
		nodeQueue.pop();
		if(currentNode==goalNode)
		{
			found=true;
			break;
		}
		else //adding neightborrs
		{
			if(currentNode[1]>0) //left neightbor
			{
				cml::vector2i leftNeighborNode=currentNode+cml::vector2i(0,-1) ;
				int key=GetHashKey(leftNeighborNode);
				if(CanMove(currentNode,dleft)&& marked.count(key)==0) 
				{
					marked.insert(key);
					nodeQueue.push(leftNeighborNode);
					//add to path:
					parentOf[key]=GetHashKey( currentNode);

				}
			}
			if(currentNode[1]<nColumns-1 ) //right neightbor
			{
				cml::vector2i rightNeightbor=currentNode+cml::vector2i(0,1) ;
				int key=GetHashKey(rightNeightbor);
				if(CanMove(currentNode,dright)&& marked.count(key)==0) 
				{
					marked.insert(key);
					nodeQueue.push(rightNeightbor);
					//add to path:
					parentOf[key]=GetHashKey(currentNode);

				}
			}
			if(currentNode[0]>0 ) //top neightbor
			{
				cml::vector2i topNeibor=currentNode+cml::vector2i(-1,0) ;
				int key=GetHashKey(topNeibor);
				if(CanMove(currentNode,dup)&& marked.count(key)==0) 
				{
					marked.insert(key);
					nodeQueue.push(topNeibor);
					//add to path:
					parentOf[key]=GetHashKey(currentNode);

				}
			}

			if(currentNode[0]<nRows-1 ) //bottom neightbor
			{
				cml::vector2i bottomNei=currentNode+cml::vector2i(1,0) ;
				int key=GetHashKey(bottomNei);
				if(CanMove(currentNode,ddown)&& marked.count(key)==0) 
				{
					marked.insert(key);
					nodeQueue.push(bottomNei);
					//add to path:
					parentOf[key]=GetHashKey(currentNode);

				}
			}
		} //end else adding neighbors

	}while(nodeQueue.size()!=0);
	deque<cml::vector2i> path;
	queue<Dir> result;
	if (found)
	{
		//create a path and create a mapping for each pair of node in the path
		//goal is actually the current node:
		
		//int current=examineNodeID;
		path.push_back(currentNode);
		while (parentOf.count(GetHashKey(currentNode))>0)
		{
			currentNode=GetVectorFromHash(parentOf[GetHashKey(currentNode)]);
			path.push_front(currentNode);
		}
		
		while(path.size()!=1)
	{
		cml::vector2i current;
		current=path.front();
		path.pop_front();

		result.push(GetDirection(current[0], current[1], path.front()[0], path.front()[1] ));


	}
	return result;

	}
	else return result;//not found then return an empty
	
	
}


bool Map::CanMove(cml::vector2i currentCell, Dir currentDirection)
{
	if(currentDirection==dright&& bitArray[currentCell[0]-1][currentCell[1]+1]==false&& bitArray[currentCell[0]][currentCell[1]+1]==false)
		return true;
	if(currentDirection==dleft&& bitArray[currentCell[0]-1][currentCell[1]-2]==false&& bitArray[currentCell[0]][currentCell[1]-2]==false)
		return true;
	if(currentDirection==ddown&& bitArray[currentCell[0]+1][currentCell[1]-1]==false&& bitArray[currentCell[0]+1][currentCell[1]]==false)
		return true;
	if(currentDirection==dup&& bitArray[currentCell[0]-2][currentCell[1]-1]==false&& bitArray[currentCell[0]-2][currentCell[1]]==false)
		return true;
	return false;
}
void Map::GetcornPosArray(GLshort* storeArray,int* nCorn)
{
	//*nCorn=nCorns;
	int index=0;
	//storeArray=new GLfloat[nCorns];
	int count=0;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nColumns;j++)
		{
			if(cornArray[i][j]==true)
			{
				cml::vector2i pos=cml::vector2i(i,j);//-i*cellHeight-cellHeight, 0.5, j*cellWidth+cellWidth);
				storeArray[index]=pos[0];
				storeArray[index+1]=pos[1];
				//storeArray[index+2]=pos[2];
				index+=2;
				cornHash.insert(i*nColumns+j);
			}
		}
		beginningNCorn=cornHash.size();
}
void Map::SetCellMesh(string meshKey, string diffTex, string normTex, cml::vector3f scale, GLfloat rot,Renderer* r1)
{
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nColumns;j++)
		{
			if(bitArray[i][j]==true)
			{
				cml::vector3f pos=cml::vector3f(-i*cellHeight-cellHeight/2.0, 0.0, j*cellWidth+cellWidth/2.0);
//				r1->AddNormalMapOBJ( meshKey,  diffTex,  normTex,pos, scale,  rot, true);
			}
		}
}
vector<Dir> Map::GetPossibleDirection(cml::vector3f pos)
{
	vector <Dir>result;
	cml::vector2i currentCell=GetCellIndex(pos);
	if(bitArray[currentCell[0]-1][currentCell[1]+1]==false&& bitArray[currentCell[0]][currentCell[1]+1]==false)
		result.push_back(dright);
	if(bitArray[currentCell[0]-1][currentCell[1]-2]==false&& bitArray[currentCell[0]][currentCell[1]-2]==false)
		result.push_back(dleft);
	if(bitArray[currentCell[0]+1][currentCell[1]-1]==false&& bitArray[currentCell[0]+1][currentCell[1]]==false)
		result.push_back(ddown);
	if(bitArray[currentCell[0]-2][currentCell[1]-1]==false&& bitArray[currentCell[0]-2][currentCell[1]]==false)
		result.push_back(dup);
	return result;
}
unordered_set<Dir> Map::GetPossibleDirectionInSet(cml::vector2i currentCell, Dir except)
{
	bool ok=false;
	unordered_set <Dir>result;
	if(except!=dright&& bitArray[currentCell[0]-1][currentCell[1]+1]==false&& bitArray[currentCell[0]][currentCell[1]+1]==false)
	{ok=true;result.insert(dright);}
	if(except!=dleft&& bitArray[currentCell[0]-1][currentCell[1]-2]==false&& bitArray[currentCell[0]][currentCell[1]-2]==false)
	{ok=true;	result.insert(dleft);}
	if(except!=ddown&& bitArray[currentCell[0]+1][currentCell[1]-1]==false&& bitArray[currentCell[0]+1][currentCell[1]]==false)
	{ok=true;	result.insert(ddown);}
	if(except!=dup&& bitArray[currentCell[0]-2][currentCell[1]-1]==false&& bitArray[currentCell[0]-2][currentCell[1]]==false)
	{ok=true;	result.insert(dup);}

	return result;
}

vector<Dir> Map::GetPossibleDirection(cml::vector2i currentCell, Dir except)
{
	bool ok=false;
	vector <Dir>result;
	if(except!=dright&& bitArray[currentCell[0]-1][currentCell[1]+1]==false&& bitArray[currentCell[0]][currentCell[1]+1]==false)
	{ok=true;result.push_back(dright);}
	if(except!=dleft&& bitArray[currentCell[0]-1][currentCell[1]-2]==false&& bitArray[currentCell[0]][currentCell[1]-2]==false)
	{ok=true;	result.push_back(dleft);}
	if(except!=ddown&& bitArray[currentCell[0]+1][currentCell[1]-1]==false&& bitArray[currentCell[0]+1][currentCell[1]]==false)
	{ok=true;	result.push_back(ddown);}
	if(except!=dup&& bitArray[currentCell[0]-2][currentCell[1]-1]==false&& bitArray[currentCell[0]-2][currentCell[1]]==false)
	{ok=true;	result.push_back(dup);}

	return result;
}

GLfloat roundNumber(GLfloat x)
{
	float result=x;
	//1.99999
	//sub is the fraction
	GLfloat sub= abs(x-(int)x);
	if (1.0-sub<0.1f)
		result=ceil(x);
	return result;

}

//this funcitno only make sense when use at the center of cell
cml::vector2i Map::GetCellIndex(cml::vector3f pos)
{
	/*	float sub= abs(pos[2]-(int)pos[2]);
	float sub2= abs(pos[0]-(int)pos[0]);
	if (sub==0||1.0-sub<0.1f)

	&&(sub2==0||1.0-sub2<0.1f)
	{*/
	return cml::vector2i((int)(-roundNumber(pos[0])/cellHeight), (int)(roundNumber(pos[2])/cellWidth));
}
cml::vector3f Map::GetCenterPosition(GLfloat height)
{
	cml::vector3f center;
	center[0]=-(nRows/2)*cellHeight-cellHeight/2.0;
	center[1]=height;
	center[2]=(nColumns/2)*cellWidth+cellWidth/2.0;
	return center;
}


cml::vector3f Map::GetCellPosition(int row, int col)
{
	cml::vector3f pos=cml::vector3f(-row*cellHeight, 0.0, col*cellWidth);

	return pos;
}