#ifndef GUIDE_TABLE_H
#define GUIDE_TABLE_H

#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include "cml\cml.h"
#include "UtilityFunctions.h"
#include <vector>
#include "Map.h"
#include <unordered_set>

using namespace std;

Dir GetDirection(int sourcei, int sourcej, int goali, int goalj)
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

void Test(int startI, int startJ, int goalI, int goalJ, bool** map)
{
		unordered_map<int, int> parentOf;
		unordered_set<string> marked; 
		bool found=false;
		cml::vector2i examineNodeID;
		cml::vector2i startNode;
		startNode.set(startI, startJ);

			queue<cml::vector2i> nQueue;
			nQueue.push(startNode);
			string key=Utility::inToString(startI)+" "+Utility::inToString(startJ);

		

}


//original try guide table but too slow to compute, so use breath first search directly
unordered_map<string,Dir>*  GetGuideHashTable(bool ** map,int nRows, int nColumns)//assume that false is empty, true is has obstacle
{

	unordered_map<string, Dir>* result=new unordered_map<string, Dir>();
	//array to hold value for each node, in this case, each cell in the bitArray, there are nRows*nColumns such that cell
	bool* marked;
	unordered_map<int, int> parentOf;
	
	marked=new bool[nRows*nColumns];

	for(int i=0;i<nRows*nColumns;i++)
		marked[i]=false;
	
	
	
	for(int startNodeID=0;startNodeID<nRows*nColumns-1;startNodeID++) //(row* column is the total number of nodes, of the 2D array
		for(int goalNodeID=startNodeID+1;goalNodeID<nColumns*nColumns;goalNodeID++)
		{
			string entryKey = Utility::inToString(startNodeID)+" "+Utility::inToString(goalNodeID);
			
			int startI= startNodeID/nColumns; 
			int startJ=startNodeID%nColumns ; //the i and j index in the array
			//cml::vector2i goalNodeCellIndex(goalNodeID/nColumns, goalNodeID%nColumns) ;
			
					
			if(startNodeID==goalNodeID||result->count(entryKey)>0||map[startI][startJ]==true) continue;
		
			//reset parent and marked:
			for(int i=0;i<nRows*nColumns;i++)
				marked[i]=false;

			parentOf.clear();
			bool found=false;

			int examineNodeID;
			queue<int> nQueue;
			nQueue.push(startNodeID);
			marked[startNodeID]=true;
			do
			{
				 examineNodeID=nQueue.front();
							nQueue.pop();
							//add to the path:
							//path.push_back(examineNodeID);
							//if this is the destination:
							if(examineNodeID== goalNodeID )
							{
								found=true; 
								break;
							}
							else //if not destination, enqueue any neighbor nodes that has not been visited;
							{
								//find neighbors: we need to use the index in the bit array of each node
								//also need to make sure that the neighbor is not visited yet
								int examiningI=examineNodeID/nColumns;
								int examiningJ=examineNodeID%nColumns;
								if(examiningJ>0)
								{
									int leftNodeID=examineNodeID-1;
									int r=leftNodeID/nColumns;
									int c=leftNodeID%nColumns;

									if(map[r][c]==false&& marked[leftNodeID]==false) 
									{
										marked[leftNodeID]=true;nQueue.push(leftNodeID);
										//add to path:
										parentOf[leftNodeID]=examineNodeID;
						
									}
								}
								//right node neighbo:
								if(examiningJ<nColumns-1)
								{	int rightNodeID=examineNodeID+1;
									
								int r=rightNodeID/nColumns;
									int c=rightNodeID%nColumns;

									if(map[r][c]==false&&marked[rightNodeID]==false) 
									{
										marked[rightNodeID]=true;nQueue.push(rightNodeID);
										parentOf[rightNodeID]=examineNodeID;
									}
								}
								//top:
									if(examiningI>0)
								{	int topNodeID=(examiningI-1)*nColumns+examiningJ;
									int r=topNodeID/nColumns;
									int c=topNodeID%nColumns;

									if(map[r][c]==false&&marked[topNodeID]==false) 
									{
										marked[topNodeID]=true;nQueue.push(topNodeID);
									parentOf[topNodeID]=examineNodeID;
									}
								}
									//bottom
									if(examiningI<nRows-1)
								{	int bottomNodeID=(examiningI+1)*nColumns+examiningJ;
										int r=bottomNodeID/nColumns;
									int c=bottomNodeID%nColumns;

									if(map[r][c]==false&&marked[bottomNodeID]==false) 
									{
										marked[bottomNodeID]=true;nQueue.push(bottomNodeID);
										parentOf[bottomNodeID]=examineNodeID;
									}
								}
							}
						}while(nQueue.size()!=0);
				
			if (found)
			{
			//create a path and create a mapping for each pair of node in the path
			//goal is actually the examining node:
				vector<int> path;
				int current=examineNodeID;
				path.push_back(current);
				while (parentOf.count(current)>0)
				{
					current=parentOf[current];
						path.push_back(current);
				}

				for(int i=path.size()-1;i>0;i--)
				{
					int sourcei=path[i]/nColumns;
					int sourcej=path[i]%nColumns;
					int goali=path[i-1]/nColumns;
					int goalj=path[i-1]%nColumns;
					Dir direction=GetDirection(sourcei,sourcej, goali, goalj);
					for(int j=i-1;j>=0;j--)
					{
						if(path[i]<path[j])
						{
							//build the entry key string:
							string hashKey=Utility::inToString(path[i])+" "+Utility::inToString(path[j]);
							(*result)[hashKey]=direction;
						}
					}
				}


			}
				

		}
		return result;
}
		
#endif




