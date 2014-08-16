#include "Renderer.h"
#include "Appearance.h"
#include "Pacman\Player.h"

void Player::SetAppearance(Appearance* a)
{
	myAppearance=a;
}

void Player::InitializePos(float x,float y, float z)
{
	position=cml::vector3f(x,y,z);
	myAppearance->Translate(-(float)position[0]/100.0,0.4,(float)position[1]/100.0);//offset, accumulate by adding to the exisinting
}

void Player::draw()
{
	//todo: ko hieu access singleton th'e nay co' du'ng kieu ko. buoi!
	Renderer::GetInstance()->RenderNormalMapObject(myAppearance);
}

void Player::Update()
{
	
}
void Player::UpdateAppearance()
{
	//convert the board coord to actual appearance:

}

void Player::SetKeyBuffer( int* keyBuffer )
{
	keyPress=keyBuffer;
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

void PacMan::Update()
{
	bool canMove=true;
	//at center of cell:
	position+=cml::vector3f(-velocity[0],velocity[1],0.0f);

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
