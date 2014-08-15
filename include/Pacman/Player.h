#ifndef PLAYER_H
#define PLAYER_H
#include "glew.h"
#include "cml\cml.h"
#include "Map.h"
#include "RenderableObject.h"

class Player
{
private:
	Dir currentDirection;
	Appearance* myAppearance;
	int speed;
	
	

	void UpdateAppearance();

		
	int* keyPress;
public:
	cml::vector2i position;
	cml::vector2i velocity;
	void ChangeDirection(Dir d);
	void SetKeyBuffer(int* keyBuffer)
	{
		keyPress=keyBuffer;
	}
	void InitializePos(int x,int y);
	void SetAppearance(Appearance* a);
	void Update(Map* m);
	void SetSpeed(int s){speed=s;}
	Dir GetOppositeDirection(Dir d);


};
#endif
