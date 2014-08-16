#ifndef PLAYER_H
#define PLAYER_H

#include "cml\cml.h"
//#include "DbgOut.h"

class Appearance;

typedef enum{dleft, dright, dup, ddown, upleft, upright, downleft, downright} Dir;

class Player
{
protected:
	Appearance* myAppearance;
	int speed;
	int* keyPress;
	virtual void UpdateAppearance();

public:
	cml::vector3f position;
	cml::vector3f velocity;

	void SetKeyBuffer(int* keyBuffer);
	void draw();

	void InitializePos(float x,float y, float z);
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


class Monster : public Player
{
public:
	Monster();
	~Monster();


};

class Survivor: public Player
{
	Survivor();
	~Survivor();
};



#endif
