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
void Player::Update(Map* m)
{
	bool canMove=true;
	//at center of cell:
	
	
		//look for portals:
		if (position[1]==3000)
		{
			InitializePos(position[0], 100);
		}
		else if (position[1]==100)
		{
			InitializePos(position[0], 3000);
		}
		Dir newDirection=currentDirection;
		if(keyPress['a'])
			newDirection=dleft;
		else if(keyPress['d'])
			newDirection=dright;
		else if(keyPress['w']) 
			newDirection=dup;
		else if(keyPress['s'])
			newDirection=ddown;

		//vector<Dir> newDirections=m->GetPossibleDirection(position/100,GetOppositeDirection(currentDirection));
		//if(newDirections.size()>0)
		//{
		//ChangeDirection(newDirections[rand()%newDirections.size()]); 
		//}
	if(newDirection==GetOppositeDirection(currentDirection))
		 ChangeDirection(newDirection);

	if(position[0]%100==0&&position[1]%100==0)
	{
		//corn eating:
		int key=(position[0]/100-1)*m->nColumns+(position[1]/100-1);
		if(m->cornHash.count(key)>0)
			m->cornHash.erase(key);
		
		canMove=m->CanMove(position/100,newDirection);
		if (canMove) ChangeDirection(newDirection);
		else canMove=m->CanMove(position/100,currentDirection);
		cout<<position<<"\n";
	}
	if(canMove)
	{
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
}
void Player::UpdateAppearance()
{
	//convert the board coord to actual appearance:




}