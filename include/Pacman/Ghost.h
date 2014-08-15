#ifndef GHOST_H
#define GHOST_H
#include "glew.h"
#include "cml\cml.h"
#include "Map.h"
#include "RenderableObject.h"
#include <unordered_map>
#include <queue>

typedef enum {chaser, pinky,bashful,pokey,clyde, random} AIType;

class Ghost

{
private:
	
	//timer:
	
	
	int currentTimer;
	string myOriginalTexture;
	//for following path:
	queue<Dir> moveList;


	Dir currentDirection;
	Appearance* myAppearance;
	int speed;
	cml::vector2i velocity;
	
	void UpdateAppearance();
	Dir GetOppositeDirection(Dir d);

public:
	AIType aitype;

		bool isVunerable;
	bool isFollowingPath;
	cml::vector2i position;
	void SetFollowPath(int starti, int startj, int goali, int goalj, Map* m);
	
	Ghost()
	{isVunerable=false;}
	void SetVunerable()
	{
		isVunerable=true;
		SetSpeed(10);
	currentTimer=6000;
	
	}
	void SetMesh(string key)
	{

		myAppearance->meshKey=key;
	}
	void SetNormal()
	{
		isFollowingPath=false;
	//	SetSpeed(20);
		myAppearance->meshKey="ghost.obj";
	}
	void SetNormalColor()
	{
	isVunerable=false;
		ChangeTexture(myOriginalTexture);
	SetSpeed(20);
	}
	void ChangeTexture(string newTexture)
	{
		myAppearance->SetTexture(MapType::diffuse, newTexture);
	}
	void SetSpeed(int x)
	{
		if(speed>0)
		velocity/=speed;
		velocity*=x;
		speed=x;
	}
	void ChangeDirection(Dir d);
	void InitializePos(int x,int y);
	void SetAppearance(Appearance* a);
	void Update(Map* m, int timelapsed, cml::vector2i playerPos, cml::vector2i playerVelocity, cml::vector2i redPosition);
	
	
};



#endif
