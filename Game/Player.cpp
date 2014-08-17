#include "RaptorEngine.h"
#include "PhysicalBody.h"
#include "Player.h"


Player::Player()
{
	this->velocity.zero();
	this->angularVelocity.zero();
	this->position.zero();
	this->orientation.zero();
}

void Player::SetPhysicalBody(PhysicalBody* a)
{
	myPhysicalBody=a;
}

void Player::InitializePos(float x,float y, float z)
{
	position=cml::vector3f(x,y,z);
	myPhysicalBody->translateAbsolute(-(float)position[0]/100.0,0.4,(float)position[1]/100.0);//offset, accumulate by adding to the exisinting
}

void Player::draw()
{
	//Render default physbody
	RaptorEngine::GetInstance()->renderPhysicalBody(this->myPhysicalBody);
}

void Player::Update(float dt)
{
	this->deltaTime=dt;
}
void Player::UpdatePhysicalBody()
{
	//convert the board coord to actual PhysicalBody:
}

void Player::SetKeyBuffer( int* keyBuffer )
{
	keyPress=keyBuffer;
}

void Player::forward()
{
	//Decrease in local Z
	velocity.set(0,0,-this->speed * deltaTime);
	position+=velocity;

	myPhysicalBody->translateAbsolute(position[0],position[1],position[2]);
} 

void Player::backward()
{
	//Increase in local Z
	velocity.set(0,0,this->speed * deltaTime);
	position+=velocity;

	myPhysicalBody->translateAbsolute(position[0],position[1],position[2]);
}

void Player::strafeRight()
{

}

void Player::strafeLeft()
{

}

void Player::rotateX()
{
	angularVelocity.set(this->rotSpeed * deltaTime,0,0);
	orientation+=angularVelocity;

	myPhysicalBody->rotateX(orientation[0]);

}

void Player::rotateY()
{
	angularVelocity.set(0,this->rotSpeed * deltaTime,0);
	orientation+=angularVelocity;

	myPhysicalBody->rotateY(orientation[1]);
}

void Player::rotateZ()
{

}

void Player::SetRotateSpeed( float rt )
{
	this->rotSpeed=rt;
}





//Pacman class. Tempore save this for refacotr the player class. Still have PACMAN Guy. He is safe and sound.
void PacMan::ChangeDirection(Dir d)
{
	if(d==dup)
		velocity.set(speed,0,0);
	else 	if(d==ddown)
		velocity.set(-speed,0,0);
	else 	if(d==dleft)
		velocity.set(0,-speed,0);
	else 	if(d==dright)
		velocity.set(0,speed,0);
	currentDirection=d;

}

Dir PacMan::GetOppositeDirection(Dir d)
{
	if (d==dleft) return dright;
	else if (d==dright) return dleft;
	else if (d==dup) return ddown;
	else if (d==ddown) return dup;
}

void PacMan::Update(float dt)
{
	bool canMove=true;
	//at center of cell:
	position+=cml::vector3f(-velocity[0],velocity[1],0.0f);

	////update orientation:
	//myPhysicalBody->transform._orientation.zero();
	//if(currentDirection==dleft)
	//	myPhysicalBody->transform._orientation[0]=3.14f;
	//else if(currentDirection==dright)
	//	myPhysicalBody->transform._orientation[0]=0.0f;
	//else if(currentDirection==dup)
	//	myPhysicalBody->transform._orientation[1]=1.57f;
	//else if(currentDirection==ddown)
	//	myPhysicalBody->transform._orientation[1]=-1.57f;
	//myPhysicalBody->Translate(cml::vector3f((GLfloat)velocity[0]/100.0,0.0,(GLfloat)velocity[1]/100.0));//offset, accumulate by adding to the exisinting


}

/////////////////////////////////////
///////////////////////////////
Monster::Monster()
{

}

Monster::~Monster()
{

}

Survivor::Survivor()
{

}

Survivor::~Survivor()
{

}
