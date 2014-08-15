#ifndef __PARTICLE_H
#define __PARTICLE_H
#include <time.h>
#include <math.h>
#include <vector>
#include "glew.h"
#include "cml\cml.h"

enum particleSystemType{explosion, fire, smoke};

const int FIRE_MAX_SIZE=1;
const GLfloat FIRE_MIN_FACTOR=0.8;
const GLfloat FIRE_DECAY_SPEED=20.0;


const int NFIRE_PARTICLES=180;
const int NEXPLOSION_PARTICLES=15;
const int FIRE_MIN_LIFE=1500;
const int FIRE_MAX_RANDOM=900;

const int SMOKE_MAX_SIZE=150;
const GLfloat SMOKE_DECAY_SPEED=18.0;
const int NSMOKE_PARTICLES=300;
const int SMOKE_MIN_LIFE=1600;
const int SMOKE_MAX_RANDOM=400;

const int SMOKE_OSCILATE_RANGE=1700;
const int SMOKE_MAX_OFFSET=1900;


const float MIN_OSCILATION_RANGE=0.05;
const float	MAX_OSCILATION_RANGE=0.7;
const float MIN_OSCILATION_SPEED=0.002;
const float MAX_OSCILATION_SPEED=0.004;

const float EMITTER_RADIUS=0.6;




const GLfloat EXPLOSION_DECAY_SPEED=37.0;
const int EXPLOSION_MAX_SIZE=20;
	class Particle
	{

	private:
	
	public:
		
		GLfloat ySpeed;//0.002
		int initialDelay;
		//cml::vector3f velocity;	
		cml::vector3f color;	
		GLfloat maxLife;
		cml::vector3f position;
		//the initial P0(x0,yx,z0)
		cml::vector3f basePosition;
		GLfloat size;
		GLfloat lifetime;   
		cml::vector3f offset;
		GLfloat decaySpeed;
		GLfloat beginningYvelo;
		bool isDead;
		GLfloat _scale;

		//right now for both x and z
		float oscilationRangeX;
		float oscilationSpeedX;

		float oscilationRangeZ;
		float oscilationSpeedZ;
		float randomXOffset;
		//float currentZOffset;

		//below is for fire only:
		//GLfloat oscillateRange;
		/*bool deadCounted;*/
		cml::matrix44f_c modelMatrix;

		void RestartFire(cml::vector3f pos)
		{

			//position=pos+offset;
			//basePosition=pos;
			//	velocity[1]=beginningYvelo;
			position=basePosition;
			
			isDead=false;
			lifetime=maxLife;

		}
		void SetFireBehavior(cml::vector3f pos,GLfloat speeD, float scale)
		{
			_scale=scale;
			//initial delay to spawn particle in different phase for the first time. So they do not
			//start up as a line of particle
			initialDelay=rand()%200;
		
			int randomRangeFactor=rand()%((int)(MAX_OSCILATION_RANGE*100)-(int)(MIN_OSCILATION_RANGE*100)); //0 to 500
			oscilationRangeX=MIN_OSCILATION_RANGE+(float)randomRangeFactor/100.0;
			int randomSpeedFactor=rand()%((int)(MAX_OSCILATION_SPEED*10000)-(int)(MIN_OSCILATION_SPEED*10000)); //0 to 500
			oscilationSpeedX=MIN_OSCILATION_SPEED+(float)randomSpeedFactor/10000.0;

			randomRangeFactor=rand()%((int)(MAX_OSCILATION_RANGE*100)-(int)(MIN_OSCILATION_RANGE*100)); //0 to 500
			oscilationRangeZ=MIN_OSCILATION_RANGE+(float)randomRangeFactor/100.0;
			randomSpeedFactor=rand()%((int)(MAX_OSCILATION_SPEED*10000)-(int)(MIN_OSCILATION_SPEED*10000)); //0 to 500
			oscilationSpeedZ=MIN_OSCILATION_SPEED+(float)randomSpeedFactor/10000.0;

			oscilationRangeX*=scale;
			oscilationRangeZ*=scale;
			//y speed is from 0.002 to 0.004
			ySpeed= (0.001f+ (float)(rand()%200)/100000 )*scale;
	

			//initial position of particle:
			//will disperse in the sphere with center is pos, and a fixed radius:
			//randomize a unit direction vector:
			cml::vector3f randDirection(50-rand()%100,50-rand()%100,50-rand()%100);
			randDirection=randDirection.normalize();
			
			int ra=rand()%100;
			if (ra>20) //only used 40 percent for random x offset, so we still have the tongue of the flame
			randomXOffset=(float)(ra)/100.0f;
		
			position= pos+randDirection*EMITTER_RADIUS*scale ;
			basePosition=position;
			//offset=off;

			size=FIRE_MIN_FACTOR;//+(rand()%FIRE_MAX_SIZE)/1000.0;
			size*=1.3*scale;
			decaySpeed=FIRE_DECAY_SPEED;
			maxLife=FIRE_MIN_LIFE+rand()%FIRE_MAX_RANDOM; //random maximumlife time of a particle
			lifetime= maxLife;

		}
	
		Particle()
		{
			isDead=false;

		}

		cml::matrix44f_c GetModelMatrix()
		{
			cml::matrix44f_c _modelMatrix;
			_modelMatrix.identity();
			_modelMatrix(0,0)=size;_modelMatrix(1,1)=size;_modelMatrix(2,2)=size;
			cml::matrix_set_translation(_modelMatrix,position[0],position[1],position[2]);
			return _modelMatrix;

		}

		//return true if the particle meet some requirement. rightnow true if it's color is bright yellow
		bool Update(particleSystemType ptype, int runTime, int elaspedTimeSinceLastUpdate,cml::vector3f systemOrigin, float scale)//return false if dead, no update, return true if alive and updated
		{
			if(initialDelay>0)
			{
				initialDelay--;
				return false;
			}

			lifetime-=decaySpeed;//decrease life time
			if (lifetime<0) {isDead=true; return false;}

			position[1]+= elaspedTimeSinceLastUpdate*ySpeed;//increase y

			if(ptype==particleSystemType::fire)
			{
				position[0]=basePosition[0]+sin(runTime*oscilationSpeedX)*oscilationRangeX;
			
				//0.01 here should be the oscilation speed
				position[2]=basePosition[2]+sin((runTime+100)*oscilationSpeedZ)*oscilationRangeZ;
			return false;
			}

			
return true;
		}
		
	} ;

	class ParticleSystem
	{
	public:
		particleSystemType myType;
		bool isActive;
		int nParticles;
		int nAlive;
		GLfloat myScale;
		int nBrightParticles;
		cml::vector3f color;

		//for moving particle system:
		Appearance* host;
		bool isMoving;

		Particle*  particleList;
		//this one below is for continuous particle system:
		cml::vector3f myPosition;

		bool continuous; //particles will not die but start from origin again, repeatly
		//constructor

		ParticleSystem(GLfloat speeD,cml::vector3f pos,particleSystemType pType, float scale)
		{
			myScale=scale;
			myType=pType;
			myPosition=pos;
			isActive=false;

			nBrightParticles=0;

			if (pType==particleSystemType::explosion)
			{nParticles=NEXPLOSION_PARTICLES;nAlive=nParticles;}
			else if (pType==particleSystemType::fire)
			{nParticles=NFIRE_PARTICLES;nAlive=nParticles;}
			else if (pType==particleSystemType::smoke)
			{nParticles=NSMOKE_PARTICLES;nAlive=nParticles;}

			particleList=new Particle[nParticles];
			//particleList=gcnew array<Particle ^,1>(nParticles);
			//initialize all particles:
		
			if (pType==particleSystemType::fire)
			{//randomize the offset from particle system position to each of the fire particle:

				for (int i=0;i<=nParticles-1;i++)	
				{
					particleList[i].SetFireBehavior(pos,speeD, scale);

				}
				cout<<"Done Create Fire\n";
			}

					isMoving=false;
			host=NULL;
		}
		~ParticleSystem()
		{
			delete [] particleList;
		}
		void SetContinuous()
		{
			continuous=true;
		}
		void SetHost(Appearance* myHost)
		{
			host=myHost;
			isMoving=true;
		}
		void Start()
		{
			isActive=true;
		}
		void Stop()
		{
			isActive=false;
		}
		void Restart(cml::vector3f pos)
		{
			myPosition=pos;
			nAlive=nParticles;
			//reinitialize all particles:
			for (int i=0;i<=nParticles-1;i++)
			{
			 if (myType==particleSystemType::fire||myType==particleSystemType::smoke)
					particleList[i].RestartFire(pos);


			}
		}
		void Update(int runTime, int elaspedTimeSinceLastUpdate)
		{
		
			if (isActive)
			{	nBrightParticles=0;
				//folow host:
			/*	if (isMoving)
					myPosition=host->_position+cml::vector3f(0.0,host->_size[1],0.0f);*/

				for (int i=0;i<=nParticles-1;i++)
				{
					if (!particleList[i].isDead)//if still alive then update
					{
						if(particleList[i].Update(myType,runTime, elaspedTimeSinceLastUpdate,myPosition,myScale ))
							nBrightParticles++;

					}
					else //if already dead, then depend on which type of particle system to act accordingly
					{
						if(myType==particleSystemType::fire||myType==particleSystemType::smoke)
						{
							particleList[i].RestartFire(myPosition);
							//	nAlive--;
							//if (nAlive==0) isActive=false;
						}
						else if (myType==particleSystemType::explosion)
						{
							nAlive--;
							if (nAlive==0) isActive=false;
						}
					}

				}
			}
		}
		GLfloat GetLightStrength()
		{
			return (float)(nBrightParticles)/(float)(nParticles);
		}
		void SetColor(int AIle)//set color for all particles
		{
			if (AIle==0) //AI level 0 -> yellow
				color=cml::vector3f(1,1,0); 
			else if (AIle==1) //AI level 1 -> orange
				color= cml::vector3f(1,0.5,0.0);
			else if (AIle==2) //AI level 2 -> red
				color=cml::vector3f(1,0,0.0);
			else if (AIle==3) //3 is white
				color=cml::vector3f(1,1,1);
		}
		

	};




#endif
