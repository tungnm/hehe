#include "glew.h"
#include<iostream>
#include <vector>
#include <map>
#include "GL/freeglut.h"
#include "cml\cml.h"
#include <stdlib.h>
#include <time.h>	

#include <sstream>
#include "Renderer.h"
#include "Input.h"
#include "Pacman\Map.h"
#include "Pacman\Ghost.h"
#include "Pacman\Player.h"
#include "UtilityFunctions.h"
#include "Pacman\Guitable.h"
#include <unordered_map>

Renderer * renderer1;


long lastTick=0;
long lastFunctionClick=0;
string objective;
void ShowVictory()
{


}
bool powerup[4];

void Close()
{
	renderer1->ShutDown();
}
void UpdateKeyboard()
{
	long current=glutGet(GLUT_ELAPSED_TIME);


	if (keysPressed['a'])
	{
		renderer1->StrafeCamRight((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	}
	else if (keysPressed['d'])
	{
		renderer1->StrafeCamLeft((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	}
	
	if (keysPressed['w'])
	{
		renderer1->MoveCamForward((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	//	renderer1->lightPos[1]+=0.2f;
	}
	else if (keysPressed['s'])
	{
		//renderer1->lightPos[1]-=0.2f;
		renderer1->MoveCamBackward((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	}

	 if (keysPressed['c'])//&&current-lastFunctionClick>700)
	{renderer1->lightPos[1]+=0.9f;
	 renderer1->UpdateLightMatrix();
		/*renderer1->ToggleSSAO();
		lastFunctionClick=current;*/
	}
	  if (keysPressed['b'])//&&current-lastFunctionClick>700)
	{
		renderer1->lightPos[1]-=0.9f;
		renderer1->UpdateLightMatrix();
		/*renderer1->ToggleBloom();
		lastFunctionClick=current;*/
	}
	if (keysPressed['p']&&current-lastFunctionClick>700)
	{
		cout<<renderer1->GetCameraPos();
		lastFunctionClick=current;
	}

	if (keysPressed[27]&&current-lastFunctionClick>700)	//press escape button: quit program
	{
		Close();
		exit(0);
	}

	lastTick=current;
}
Player player1;
Ghost ghost[4];
Map a;
Map simpleMap;

GLshort *array1;
GLuint cornHandle;
int currentNCorns;
int lives;
int powerupIndex[4];

void ResetPosition()
{
		ghost[0].InitializePos(1200,1500);
	ghost[0].ChangeDirection(dleft);
//	ghost[0].SetGoal(1200,1500);

	ghost[1].InitializePos(1500,1500);
	ghost[1].ChangeDirection(dup);
//	ghost[1].SetGoal(1200,1500);

	ghost[2].InitializePos(1500,1700);

	ghost[2].ChangeDirection(dup);
	//ghost[2].SetFollowPath(2,3,15,16,&a );
//	ghost[2].SetGoal(1200,1500);

	ghost[3].InitializePos(1500,1300);
	
	player1.InitializePos(2400,1600);
	player1.ChangeDirection(dleft);

	for(int i=0;i<4;i++)
	{
		ghost[i].SetNormal();
		ghost[i].SetNormalColor();
	}

}

void init(void)
{	
	//simpleMap.ReadBitArray("simpleMap.txt");

	//initialize render
	renderer1=Renderer::GetInstance();
	a.ReadBitArray("map1.txt");
//	unordered_map<string, Dir> *result=GetGuideHashTable(a.bitArray, a.nRows,a.nColumns);
//	Ghost::SetGuideTable(result);

	//a.PrintOutBitArray();
	a.SetCellDimension(1.0,1.0);
	cml::vector3f mapCenter=a.GetCenterPosition(43.0);
	renderer1->SetCameraPosition(mapCenter-cml::vector3f(2.0,0.0,0.0), cml::vector3f(mapCenter[0]+0.5,0.0,mapCenter[2]));

	renderer1->StartUp();
	renderer1->SetLightTarget(mapCenter[0],1.0,mapCenter[2]);
//	renderer1->SetupWeight(48,250);
//	renderer1->AddNormalMapOBJ("extrudedBox.obj","textures\\red.jpg","textures\\brick1_normalmap.jpg", cml::vector3f(-0.08,-1.5,-2.5),cml::vector3f(0.5,0.5,0.5),0.0);
//	renderer1->AddNormalMapOBJ("box.obj","textures\\red.jpg","textures\\white.jpg", cml::vector3f(0.0,-0.5,0.0),cml::vector3f(0.3,0.04,0.3),0.0);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	//test map:
	
	renderer1->AddNormalMapOBJ("map1.obj","textures\\blue.jpg","textures\\white.jpg", cml::vector3f(mapCenter[0]+0.5,0.0,mapCenter[2]),cml::vector3f(1.0,1.0,1.0),0.0, false);
	renderer1->AddNormalMapOBJ("particleHalf.obj","textures\\default.jpg","textures\\red.jpg", cml::vector3f(0.0,1.5,0.0),cml::vector3f(0.5,0.5,0.5),0.0, false);
	renderer1->AddNormalMapOBJ("box.obj","textures\\black.jpg","textures\\white.jpg", cml::vector3f(mapCenter[0],-0.5,mapCenter[2]),cml::vector3f(1.5,0.04,1.5),0.0,true);
	
	for (int i=0;i<4;i++)
		powerup[i]=true;

	//add powerup
	renderer1->AddNormalMapOBJ("sphere.obj","textures\\wally.jpg","textures\\white.jpg", cml::vector3f(-4,0.5,3),cml::vector3f(1.0,1.0,1.0),0.0,true);
	powerupIndex[0]=renderer1->GetNObjects()-1;

	renderer1->AddNormalMapOBJ("sphere.obj","textures\\wally.jpg","textures\\white.jpg", cml::vector3f(-4,0.5,28),cml::vector3f(1.0,1.0,1.0),0.0,true);
		powerupIndex[1]=renderer1->GetNObjects()-1;

	renderer1->AddNormalMapOBJ("sphere.obj","textures\\wally.jpg","textures\\white.jpg", cml::vector3f(-24,0.5,3),cml::vector3f(1.0,1.0,1.0),0.0,true);
		powerupIndex[2]=renderer1->GetNObjects()-1;

	renderer1->AddNormalMapOBJ("sphere.obj","textures\\wally.jpg","textures\\white.jpg", cml::vector3f(-24,0.5,28),cml::vector3f(1.0,1.0,1.0),0.0,true);
		powerupIndex[3]=renderer1->GetNObjects()-1;

	//hack: remember the index of there spereh to remove later when player eat it
	
	srand(time(NULL));
//	a.SetCellMesh("unitBox.obj","textures\\red.jpg","textures\\brick1_normalmap.jpg", cml::vector3f(1.0,1.0,1.0),0.0f, renderer1);
	for(int i=0;i<4;i++)
	{
	//ghost[i].SetAppearance(renderer1->GetAppeance("ghost.obj","textures\\ghost.jpg","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
		ghost[i].SetSpeed(5);	ghost[i].aitype=random;
	}

	ghost[0].SetAppearance(renderer1->GetAppeance("ghost.obj","textures\\redghost.jpg","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
	ghost[1].SetAppearance(renderer1->GetAppeance("ghost.obj","textures\\pinkghost.jpg","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
	ghost[2].SetAppearance(renderer1->GetAppeance("ghost.obj","textures\\orangeghost.jpg","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
	ghost[3].SetAppearance(renderer1->GetAppeance("ghost.obj","textures\\tealghost.jpg","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
	
	renderer1->LoadTexture("textures\\blueghost.jpg");
	renderer1->LoadMesh("eye.obj");

	//set ais:
	ghost[0].aitype=chaser;
	ghost[1].aitype=pinky;
	ghost[3].aitype=bashful;
	ghost[2].aitype=clyde;

	//add text:
	lives=3;
	renderer1->AddText("score","Score",cml::vector2f(0.60,0.75),0.03f,0.01f);
	renderer1->AddText("lives","Lives",cml::vector2f(0.60,0.65),0.03f,0.01f);
	renderer1->AddText("goal","no", cml::vector2f(0.0,0.0), 0.1,0.0);
	renderer1->HideText("goal");

	//player:
	player1.SetAppearance(renderer1->GetAppeance("pacman.obj","textures\\pacman.png","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
	player1.SetSpeed(20);

	player1.SetKeyBuffer(keysPressed);
	
	ResetPosition();
	//try out the instancing
	//bind VAO of the particle object
	MeshInfo*mesh=&(renderer1->meshMan->GetMeshInfo("particleHalf.obj"));
	glBindVertexArray(mesh->VAOHandle);
	//generate 1 handles for VBOs in GPU to store: test color:
	GLuint handles[1];
	glGenBuffers(1,handles);
	
	array1=new GLshort[a.nCorns*2];//[9]={0.0,0.0,0.0,-1.0,0.0,0.0,-2.0,0.0,1.0};
	a.GetcornPosArray(array1,&currentNCorns);
	//result.posVBOHandle=handles[0];
		//create copy data to, and bind attribute pointer for posVBO
	cornHandle=handles[0];
	currentNCorns=a.nCorns;
	glBindBuffer(GL_ARRAY_BUFFER,handles[0]);
	glEnableVertexAttribArray(4);//enable vertex attribute location 4
	glVertexAttribPointer(4, 2, GL_SHORT, GL_FALSE, 0, 0); //Note that this function uses the currently binded VBO to set the attribute pointer to
	glVertexAttribDivisor(4,1);
	glBufferData(GL_ARRAY_BUFFER,a.nCorns*2*sizeof(short),array1,GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


}
//free resource
int pauseTimer;
bool isPause;
int lasttick;
void display()
{
	int elapsedTime=glutGet(GLUT_ELAPSED_TIME);

	renderer1->Display(elapsedTime,currentNCorns);
	glutSwapBuffers();
	
	
//	glBindBuffer(GL_ARRAY_BUFFER,cornHandle);
	//glBufferSubData(GL_ARRAY_BUFFER,0,currentNCorns*2*sizeof(short),array1);
	

}

void SetPause()
{
	isPause=true;
	pauseTimer=3000;
}

void Update(int value)
{
	int elapsedTime=glutGet(GLUT_ELAPSED_TIME);

	int timelapse=elapsedTime-lasttick;
	lasttick=elapsedTime;
	
	if(isPause)
		{
			pauseTimer-=timelapse;
		if(pauseTimer<=0)
		{	isPause=false;	ResetPosition();	glutTimerFunc(15, Update, 0);return;}
			glutTimerFunc(15, Update, 0);
		return;
		
	}

	
	UpdateKeyboard();
//	renderer1->Update(elapsedTime);

	bool getpower=false;
	//check if eat power
	if(player1.position/100==cml::vector2i(4,3)&&powerupIndex[0]!=-1)
	{
		renderer1->RemoveObject(powerupIndex[0]);getpower=true;
		powerupIndex[0]=-1;
	}
	
	else if (player1.position/100==cml::vector2i(4,28)&&powerupIndex[1]!=-1)
	{
		renderer1->RemoveObject(powerupIndex[1]);getpower=true;
		powerupIndex[1]=-1;
	}
	else if (player1.position/100==cml::vector2i(24,3)&&powerupIndex[2]!=-1)
	{
		renderer1->RemoveObject(powerupIndex[2]);getpower=true;
		powerupIndex[2]=-1;
	}
	else if (player1.position/100==cml::vector2i(24,28)&&powerupIndex[3]!=-1)
	{
		renderer1->RemoveObject(powerupIndex[3]);getpower=true;
		powerupIndex[3]=-1;
	}
	if (getpower)
	for(int i=0;i<4;i++)
	{
		ghost[i].ChangeTexture("textures\\blueghost.jpg");
		ghost[i].SetVunerable();
	}

	//renderer1->OrientCameraWithMousePos(mouseDisplacementX,mouseDisplacementY);
	for(int i=0;i<4;i++)
	{
			//check if player collide with ghosts:
		if(((player1.position[0]==ghost[i].position[0]&&abs(player1.position[1]-ghost[i].position[1])<200)
			||(player1.position[1]==ghost[i].position[1]&&abs(player1.position[0]-ghost[i].position[0])<200))
			)
		{	
			if(ghost[i].isVunerable)
			{
				if (ghost[i].isFollowingPath==false)
				{ghost[i].SetFollowPath(ghost[i].position[0]/100,ghost[i].position[1]/100,15,15,&a);
			//	ghost[i].isVunerable=false;
				}
			}
			else if (ghost[i].isFollowingPath==false)
			{
				lives--;
				SetPause();
			
				break;
			}
		}
		ghost[i].Update(&a,timelapse,player1.position, player1.velocity, ghost[0].position);
	}
	if(keysPressed['a']>0)
	keysPressed['a']--;
	else if(keysPressed['d']>0)
	keysPressed['d']--;
	else if(keysPressed['w']>0)
	keysPressed['w']--;
	else if(keysPressed['s']>0)
	keysPressed['s']--;


	//cout<<keysPressed['a']<<"\n";


	player1.Update(&a);
	//renderer1->UpdateCamera();
	a.UpdateCornHash(array1, &currentNCorns);
	//update corn instanced array
	glBindBuffer(GL_ARRAY_BUFFER,cornHandle);
	glBufferSubData(GL_ARRAY_BUFFER,0,currentNCorns*2*sizeof(short),array1);
	
	//update score text:
	renderer1->ChangeTex("score","Score:"+Utility::inToString(a.GetCornEatean()));
	renderer1->ChangeTex("lives","Lives:"+Utility::inToString(lives));

	glutPostRedisplay();
	glutTimerFunc(15, Update, 0);

}

int main(int argc, char **argv)
{
	lastTick=0;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize( W_WIDTH, W_HEIGHT);
	glViewport(0,0, W_WIDTH, W_HEIGHT);
	glutCreateWindow("Tung Demo");

	//glutPassiveMotionFunc(processMouse);
	glutDisplayFunc(display); 
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutTimerFunc(15, Update, 0);
	init();
	glutDisplayFunc(display); 
	glutWarpPointer(W_WIDTH/2.0,W_HEIGHT/2.0);
	glutMainLoop();
	return 0;
}

