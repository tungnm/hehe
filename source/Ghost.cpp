#include "Pacman\Ghost.h"
#include <time.h>


void Ghost::SetAppearance(Appearance* a)
{
	myAppearance=a;
	myOriginalTexture=a->GetDiffuseMap();
}

void Ghost::InitializePos(int xBoard, int yBoard)
{
	//myAppearance->Translate(pos[0],pos[1],pos[2]);
	position=cml::vector2i(xBoard,yBoard);
	myAppearance->Translate(-(float)position[0]/100.0,0.4,(float)position[1]/100.0);//offset, accumulate by adding to the exisinting
	//	ChangeDirection(d);
}

vector< Dir> GetRelativeOrientation(cml::vector2i currentobj, cml::vector2i targetobj)
{
	vector<Dir> result;
	if(currentobj[0]==targetobj[0]) 
	{
		if(targetobj[1]>currentobj[1]) result.push_back(dright);
		else result.push_back(dleft);
	}
	else 
		if(currentobj[1]==targetobj[1]) 
		{
			if(targetobj[0]>currentobj[0])  result.push_back(ddown);
			else result.push_back(dup);
		}
		else if (targetobj[1]>currentobj[1]&&targetobj[0]<currentobj[0])
		{result.push_back(dup);result.push_back(dright);}
		else if (targetobj[1]>currentobj[1]&&targetobj[0]>currentobj[0])
		{result.push_back(ddown);result.push_back(dright);}
		else if (targetobj[1]<currentobj[1]&&targetobj[0]<currentobj[0])
		{result.push_back(dup);result.push_back(dleft);}
		else if (targetobj[1]<currentobj[1]&&targetobj[0]>currentobj[0])
		{result.push_back(ddown);result.push_back(dleft);}
		return result;
}

void Ghost::ChangeDirection(Dir d)
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
Dir Ghost::GetOppositeDirection(Dir d)
{
	if (d==dleft) return dright;
	else if (d==dright) return dleft;
	else if (d==dup) return ddown;
	else if (d==ddown) return dup;
}


void Ghost::SetFollowPath(int starti, int startj, int goali, int goalj, Map* m)
{

	moveList=m->GetPath(cml::vector2i(starti,startj), cml::vector2i(goali, goalj));
	isFollowingPath=true;
	SetSpeed(20);
	position=position/100*100;
	InitializePos(position[0],position[1]);
	SetMesh("eye.obj");

}
void Ghost::Update(Map* m, int timelapse, cml::vector2i playerPos, cml::vector2i playerVelocity, cml::vector2i redPosition)
{
	if(isVunerable)
	{
		currentTimer-=timelapse;
		if (currentTimer<=0)
		{
			SetNormalColor();
			InitializePos(position[0]/100*100,position[1]/100*100);
		}
	}
	bool canMove=true;
	//SPECIAL CASE IN THE MIDDLE OF THE MAP:
	if(position[0]==1400&&(position[1]==1500||position[1]==1600))
		ChangeDirection(dup);
	//
	if(position[1]>=1350&&position[1]<=1750&&position[0]>=1450&&position[0]<=1550)
	{
		//SetSpeed(10);
		if(position[1]==1350&&currentDirection==dleft)
		{	
			vector<Dir> newDirections;
			if(position[0]==1550)
			{newDirections.push_back(dup);newDirections.push_back(dright);}else newDirections.push_back(ddown);		
			ChangeDirection(newDirections[rand()%newDirections.size()]); 
		}
		else if(position[1]==1750&&currentDirection==dright)
		{	
			vector<Dir> newDirections;
			if(position[0]==1550)
			{newDirections.push_back(dup);newDirections.push_back(dleft);}else newDirections.push_back(ddown);
			ChangeDirection(newDirections[rand()%newDirections.size()]); 

		}
		else if(position[0]==1450&&(position[1]==1350||position[1]==1750)&& currentDirection==dup)
		{	
			ChangeDirection(ddown); 

		}
		else if(position[0]==1550&&currentDirection==ddown)
		{	
			vector<Dir> newDirections;newDirections.push_back(dup);
			if(position[1]!=1750)
				newDirections.push_back(dright);
			if(position[1]!=1350)
				newDirections.push_back(dleft);
			ChangeDirection(newDirections[rand()%newDirections.size()]); 
		}
		else if((position[1]==1600||position[1]==1500)
			&&(position[0]==1550||position[0]==1450)
			)
		{
			vector<Dir> newDirections;newDirections.push_back(dup);
			ChangeDirection(dup); 
		}

	}

	else
		//going randomly
		if(position[0]%100==0&&position[1]%100==0)
			//	if((position[0]+velocity[0])/100!= position[0]/100&&(position[1]+velocity[1])/100!= position[1]/100)
		{
			bool didfollowpath=false;
			if (isFollowingPath)
			{
				//SetSpeed(25);
				if (moveList.size()==1)
					SetSpeed(10);

				if(moveList.size()==0)
				{	
					isFollowingPath=false;
					SetNormal();
					SetNormalColor();
				}
				else
				{
					//speed=20;
					Dir direct=moveList.front();
					ChangeDirection(direct);
					moveList.pop();
					didfollowpath=true;
				}
			}
			if(!didfollowpath)
			{
			//	SetSpeed(20);
				//look for portals:
				if (position[1]==3000)
				{
					InitializePos(position[0], 100);
				}
				else if (position[1]==100)
				{
					InitializePos(position[0], 3000);
				}

				/*	if(newDirections.size()>0)
				{*/
				bool preferedMoveExecuted=false;
				if(aitype!=random)
				{

					cml::vector2i myTarget;
					if(aitype==chaser)
					{
						if(isVunerable)  //if vunerable then move to the corner
							myTarget=cml::vector2i(-100,2600);
						else myTarget=playerPos;

					}
					else if (aitype==pinky)
					{
						if(isVunerable)  //if vunerable then move to the corner
							myTarget=cml::vector2i(-100,400);
						else
							myTarget=playerPos+playerVelocity*4;
					}
					else if (aitype==bashful)
					{
						if(isVunerable)  //if vunerable then move to the corner
							myTarget=cml::vector2i(3200,2900);
						else
						{
							cml::vector2i playerHeading=playerPos+playerVelocity*2;
							myTarget=redPosition+(playerHeading-redPosition)*2;
						}
					}
					else if (aitype==clyde)
					{
						if(isVunerable)  //if vunerable then move to the corner
							myTarget=cml::vector2i(3200,0);
						else
						{
							if (abs(playerPos[0]- position[0])>8||abs(playerPos[1]- position[1])>8)
								//behave like chaser
									myTarget=playerPos;
							else 
								//behave like scattering
								myTarget=cml::vector2i(3200,0);
						}
					}

					//compare with direction of player to determine which direction to go
					vector<Dir> preferedDirection=GetRelativeOrientation(position, myTarget);
					unordered_set<Dir> moveableDirections=m->GetPossibleDirectionInSet(position/100,GetOppositeDirection(currentDirection));
					//remove unmovable prefered : can't use for loop here because the size of the vector is changed inside the loop

					if (preferedDirection.size()>0)
					{
						if ((position[0]==1200||position[0]==1300)&&(position[1]==1500||position[1]==1600))
						{
							moveableDirections.erase(ddown);
						}


						if(moveableDirections.count(preferedDirection[0])==0)
						{
							preferedDirection.erase(preferedDirection.begin());
							if(preferedDirection.size()==1&&moveableDirections.count(preferedDirection[0])==0)
								preferedDirection.erase(preferedDirection.begin());
						}
						else if (preferedDirection.size()==2)
						{
							if(moveableDirections.count(preferedDirection[1])==0)
								preferedDirection.erase(preferedDirection.begin()+1);
						}

						//now pick random one from the moveable, prefered directions:
						if (preferedDirection.size()>0)
						{
							ChangeDirection(preferedDirection[rand()%preferedDirection.size()]); 
							preferedMoveExecuted=true;
						}
					}

				}
				if (preferedMoveExecuted==false)
				{
					vector<Dir> newDirections=m->GetPossibleDirection(position/100,GetOppositeDirection(currentDirection));
					
					ChangeDirection(newDirections[rand()%newDirections.size()]); 
				}

			}
			
		}
		if(canMove)
		{
			position+=cml::vector2i(-velocity[0],velocity[1]);
			myAppearance->Translate(cml::vector3f((GLfloat)velocity[0]/100.0,0.0,(GLfloat)velocity[1]/100.0));//offset, accumulate by adding to the exisinting
		}
}
void Ghost::UpdateAppearance()
{

}