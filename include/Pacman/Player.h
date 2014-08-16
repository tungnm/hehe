#ifndef PLAYER_H
#define PLAYER_H
#include "glew.h"
#include "cml\cml.h"
#include "Renderer.h"
#include "Appearance.h"

typedef enum{dleft, dright, dup, ddown, upleft, upright, downleft, downright} Dir;

class Player
{
protected:
	Appearance* myAppearance;
	int speed;
	int* keyPress;
	virtual void UpdateAppearance();

public:
	cml::vector2i position;
	cml::vector2i velocity;
	

	void SetKeyBuffer(int* keyBuffer);

	void draw();

	void InitializePos(int x,int y);
	void SetAppearance(Appearance* a);
	
	
	void SetSpeed(int s){speed=s;}

	
	virtual void Update();
	
	virtual ~Player(){}//Destructor
};


class PacMan:public Player
{
public:
	Dir currentDirection;	
	Dir GetOppositeDirection(Dir d);
	void ChangeDirection(Dir d);

	void Update() override;

};

#endif
