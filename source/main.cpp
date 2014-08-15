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
#include "Pacman\Player.h"
#include "UtilityFunctions.h"
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
	
		/*renderer1->ToggleSSAO();
		lastFunctionClick=current;*/
	}
	  if (keysPressed['b'])//&&current-lastFunctionClick>700)
	{
		renderer1->lightPos[1]-=0.9f;
		
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

void init(void)
{	
	//simpleMap.ReadBitArray("simpleMap.txt");

	//initialize render
	renderer1=Renderer::GetInstance();

	renderer1->SetCameraPosition(cml::vector3f(6.0,16.0,0.0), cml::vector3f(0.0,0.5,0.0));

	renderer1->StartUp();
	renderer1->SetLightTarget(0.0,0.5,0.0);
	renderer1->setLightPos(-5.0, 5.0,0.0);
	//renderer1->AddSolidColorOBJ("map1.obj",cml::vector3f(0.0,0.0,0.8), cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0);
	//renderer1->AddNormalMapOBJ("map1.obj","textures\\blue.jpg","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0, false);
	/*
	renderer1->LoadTexture("textures\\blueghost.jpg");
	renderer1->LoadMesh("eye.obj");
	*/

	//renderer1->AddText("score","Score",cml::vector2f(0.60,0.75),0.03f,0.01f);
	//renderer1->AddText("lives","Lives",cml::vector2f(0.60,0.65),0.03f,0.01f);
	//renderer1->AddText("goal","no", cml::vector2f(0.0,0.0), 0.1,0.0);
	//renderer1->HideText("goal");

	//player:
	player1.SetAppearance(renderer1->GetAppeance("pacman.obj","textures\\pacman.png","textures\\white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
	renderer1->GetAppeance("box.obj","textures\\blue.jpg","textures\\white.jpg", cml::vector3f(2.0,0.0,3.0),cml::vector3f(0.2,0.3,0.1),0.01);
renderer1->GetAppeance("box.obj","textures\\red.jpg","textures\\white.jpg", cml::vector3f(2.0,3.0,3.0),cml::vector3f(0.05,0.05,0.05),0.01);

	player1.SetSpeed(20);

	player1.SetKeyBuffer(keysPressed);

}
//free resource
int pauseTimer;
bool isPause;
int lasttick;
void display()
{
	int elapsedTime=glutGet(GLUT_ELAPSED_TIME);

	renderer1->Display(elapsedTime,0);
	glutSwapBuffers();
	

}

void SetPause()
{
	isPause=true;
	pauseTimer=3000;
}

void Update(int value)
{
	/*int elapsedTime=glutGet(GLUT_ELAPSED_TIME);

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
	*/
	
	UpdateKeyboard();
//	renderer1->Update(elapsedTime);


	if(keysPressed['a']>0)
	keysPressed['a']--;
	else if(keysPressed['d']>0)
	keysPressed['d']--;
	else if(keysPressed['w']>0)
	keysPressed['w']--;
	else if(keysPressed['s']>0)
	keysPressed['s']--;


	//cout<<keysPressed['a']<<"\n";


	player1.Update();
	//renderer1->UpdateCamera();
	
	/*
	//update score text:
	renderer1->ChangeTex("score","Score:"+Utility::inToString(a.GetCornEatean()));
	renderer1->ChangeTex("lives","Lives:"+Utility::inToString(lives));
	*/
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

