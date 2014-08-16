#ifndef GHOST_H
#define GHOST_H
#include "glew.h"
#include "cml\cml.h"
#include "Map.h"
#include "PhysicalBody.h"
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
	PhysicalBody* myPhysicalBody;
	int speed;
	cml::vector2i velocity;
	
	void UpdatePhysicalBody();
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

		myPhysicalBody->setMesh(key);
	}
	void SetNormal()
	{
		isFollowingPath=false;
	//	SetSpeed(20);
		myPhysicalBody->setMesh("ghost.obj");
	}
	void SetNormalColor()
	{
	isVunerable=false;
		ChangeTexture(myOriginalTexture);
	SetSpeed(20);
	}
	void ChangeTexture(string newTexture)
	{
		myPhysicalBody->setTexture(MapType::diffuse, newTexture);
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
	void SetPhysicalBody(PhysicalBody* a);
	void Update(Map* m, int timelapsed, cml::vector2i playerPos, cml::vector2i playerVelocity, cml::vector2i redPosition);
	
	
};



#endif
