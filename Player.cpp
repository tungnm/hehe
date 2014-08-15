#include "Pacman\Player.h"

void Player::SetAppearance(Appearance* a)
{
	myAppearance=a;
}

void Player::InitializePos(int xBoard, int yBoard)
{
	//myAppearance->Translate(pos[0],pos[1],pos[2]);
	position=cml::vector2i(xBoard,yBoard);
	myAppearance->Translate(-(float)position[0]/100.0,0.4,(float)position[1]/100.0);//offset, accumulate by adding to the exisinting
//	ChangeDirection(dright);
}

void Player::ChangeDirection(Dir d)
{
	if(d==dup)
		velocity.set(speed,0);
	else 	if(d==ddown)
		velocity.set(-speed,0);
	else 	if(d==dleft)
		velocity.set(0,-speed);
	else 	if(d==dright)
		velocity.set(0,speed);
	currentDirection=d;

}
Dir Player::GetOppositeDirection(Dir d)
{
	if (d==dleft) return dright;
	else if (d==dright) return dleft;
	else if (d==dup) return ddown;
	else if (d==ddown) return dup;
}
void Player::Update()
{
	bool canMove=true;
	//at center of cell:
	
	

	position+=cml::vector2i(-velocity[0],velocity[1]);
	//update orientation:
	myAppearance->transform._orientation.zero();
	if(currentDirection==dleft)
		myAppearance->transform._orientation[0]=3.14f;
	else if(currentDirection==dright)
		myAppearance->transform._orientation[0]=0.0f;
	else if(currentDirection==dup)
		myAppearance->transform._orientation[1]=1.57f;
	else if(currentDirection==ddown)
		myAppearance->transform._orientation[1]=-1.57f;
	
	myAppearance->Translate(cml::vector3f((GLfloat)velocity[0]/100.0,0.0,(GLfloat)velocity[1]/100.0));//offset, accumulate by adding to the exisinting

}
void Player::UpdateAppearance()
{
	//convert the board coord to actual appearance:




}