#include "DbgOut.h"
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
#include "GlobalObjects.h"


void Close()
{
	renderer1->ShutDown();
}
void UpdateKeyboard()
{
	long current=glutGet(GLUT_ELAPSED_TIME);

	mainScene->updateKeyBoard(keysPressed);

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
	{//renderer1->lightPos[1]+=0.9f;
	
		/*renderer1->ToggleSSAO();
		lastFunctionClick=current;*/
	}
	  if (keysPressed['b'])//&&current-lastFunctionClick>700)
	{
		//renderer1->lightPos[1]-=0.9f;
		
		/*renderer1->ToggleBloom();
		lastFunctionClick=current;*/
	}
	if (keysPressed['p']&&current-lastFunctionClick>700)
	{
		dInfo()<<renderer1->GetCameraPos();
		lastFunctionClick=current;
	}

	if (keysPressed[27]&&current-lastFunctionClick>700)	//press escape button: quit program
	{
		Close();
		exit(0);
	}

	lastTick=current;
	
}


void init(void)
{	
	//initialize render
	renderer1=Renderer::GetInstance();
	//Setup renderer
	renderer1->StartUp();
	
	//Init create main scene
	//set up game objects
	//player:
	player1.SetPhysicalBody(renderer1->GetAppeance("ghost.obj","pacman.png","white.jpg", cml::vector3f(0.0,0.0,0.0),cml::vector3f(1.0,1.0,1.0),0.0));
	mainScene = Scene::buildScene(&player1);

	//Set the world camerea
	renderer1->SetCameraPosition(cml::vector3f(0.0,0.0,20.0), cml::vector3f(0.0,0.0,0.0));
	renderer1->SetLightTarget(0.0,0.5,0.0);
	renderer1->setLightPos(-5.0, 5.0,0.0);

	
	//todo: 2 cai' boxes nay chi la PhysicalBody, ve sau no' se nam trong monster class, hay gi do...
	box1 = renderer1->GetAppeance("unitBox.obj","blue.jpg","white.jpg", cml::vector3f(4.0,0.0,0.0) /*pos*/
																	,cml::vector3f(1.0,1.0,1.0), /*scale*/
																	0.00);
	//box2 = renderer1->GetAppeance("box.obj","red.jpg","white.jpg", cml::vector3f(2.0,3.0,3.0),cml::tevector3f(0.05,0.05,0.05),0.01);

	//deo hieu ca'i nay de lam gi, cha'c xem sau
	player1.SetKeyBuffer(keysPressed); 
}

void display()
{
	int elapsedTime=glutGet(GLUT_ELAPSED_TIME);

	renderer1->BeginRendering();
	
	//todo: cho het nhung ca'i draw() nay vao trong scene->draw(), hay gi do'...
	player1.draw();
	
	//todo: day chi la 2 ca'i test boxes, ve sau no' se thuoc vao` monster.draw()
	renderer1->RenderNormalMapObject(box1);
	//renderer1->RenderNormalMapObject(box2);

	renderer1->EndRendering();
	glutSwapBuffers();
	

}

static float angle=0;

void Update(int value)
{
		
	UpdateKeyboard();
	/*
	if(keysPressed['a']>0)
	keysPressed['a']--;
	else if(keysPressed['d']>0)
	keysPressed['d']--;
	else if(keysPressed['w']>0)
	keysPressed['w']--;
	else if(keysPressed['s']>0)
	keysPressed['s']--;*/
	//cout<<keysPressed['a']<<"\n";


	player1.Update();
	box1->rotate(angle);

	angle+=0.01f;
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
	dWarning()<<"Starting Engine.! Loading Resources";

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

