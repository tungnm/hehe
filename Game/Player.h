#ifndef PLAYER_H
#define PLAYER_H

#include "cml\cml.h"
//#include "DbgOut.h"
#include "PhysicalBody.h"
class Raptor::PhysicalBody;

typedef enum{dleft, dright, dup, ddown, upleft, upright, downleft, downright} Dir;

class Player
{
protected:
	Raptor::PhysicalBody* myPhysicalBody;
	int speed;
	int* keyPress;
	virtual void UpdatePhysicalBody();

	float deltaTime;
	float rotSpeed;

public:
	cml::vector3f orientation;
	cml::vector3f position;
	cml::vector3f velocity;
	cml::vector3f angularVelocity;

	void SetKeyBuffer(int* keyBuffer);
	virtual void draw();

	void InitializePos(float x,float y, float z);
	void SetPhysicalBody(Raptor::PhysicalBody* a);
	void SetSpeed(int s){speed=s;}
	
	virtual void Update(float dt);
	
	Player();
	virtual ~Player(){}//Destructor

	virtual void forward();
	virtual void backward();
	virtual void strafeRight();
	virtual void strafeLeft();
	virtual void rotateX();
	virtual void rotateY();
	virtual void rotateZ();

	void SetRotateSpeed( float rt );
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


class PacMan:public Player
{
public:
	Dir currentDirection;	
	Dir GetOppositeDirection(Dir d);
	void ChangeDirection(Dir d);

	void Update(float dt) override; 

};




#endif
