#include "DbgOut.h"
#include<iostream>
#include <vector>
#include <map>
#include "cml\cml.h"
#include <stdlib.h>
#include <time.h>	
#include <sstream>
#include "RaptorEngine.h"
#include "Input.h"
#include "Player.h"
#include <unordered_map>
#include "GlobalObjects.h"
#include "freeglut.h"

void Close()
{
	gEngine->shutDown();
}
void UpdateKeyboard()
{
	//long current=glutGet(GLUT_ELAPSED_TIME);

	//mainScene->updateKeyBoard(keysPressed);

	//if (keysPressed['a'])
	//{
	//	gEngine->StrafeCamRight((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	//}
	//else if (keysPressed['d'])
	//{
	//	gEngine->StrafeCamLeft((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	//}
	//
	//if (keysPressed['w'])
	//{
	//	gEngine->MoveCamForward((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	////	gEngine->lightPos[1]+=0.2f;
	//}
	//else if (keysPressed['s'])
	//{
	//	//gEngine->lightPos[1]-=0.2f;
	//	gEngine->MoveCamBackward((current-lastTick)*CAMERA_MOVEMENT_SPEED);
	//}

	// if (keysPressed['c'])//&&current-lastFunctionClick>700)
	//{//gEngine->lightPos[1]+=0.9f;
	//
	//	/*gEngine->ToggleSSAO();
	//	lastFunctionClick=current;*/
	//}
	//  if (keysPressed['b'])//&&current-lastFunctionClick>700)
	//{
	//	//gEngine->lightPos[1]-=0.9f;
	//	
	//	/*gEngine->ToggleBloom();
	//	lastFunctionClick=current;*/
	//}
	//if (keysPressed['p']&&current-lastFunctionClick>700)
	//{
	//	dInfo()<<gEngine->GetCameraPos();
	//	lastFunctionClick=current;
	//}

	//if (keysPressed[27]&&current-lastFunctionClick>700)	//press escape button: quit program
	//{
	//	Close();
	//	exit(0);
	//}

	//lastTick=current;
	
}


void init(void)
{	
	
	//initialize render
	gEngine=RaptorEngine::GetInstance();
	//Setup renderer
	gEngine->startUp();
	

	gEngine->loadObjMesh("ghost.obj");
	gEngine->loadObjMesh("unitBox.obj");
	gEngine->loadTexture("pacman.png");
	gEngine->loadTexture("white.jpg");
	gEngine->loadTexture("blue.jpg");
	//Init create main scene
	//set up game objects
	//player:
	PhysicalBody* playerBody = new PhysicalBody();
	playerBody->setMesh("ghost.obj");
	playerBody->setTexture(MapType::diffuse, "pacman.png");
	playerBody->setTexture(MapType::normal, "white.jpg");
	playerBody->translateAbsolute(0.0,0.0,0.0);
	playerBody->setScale(1.0,1.0,1.0);
	player1.SetPhysicalBody(playerBody);
	mainScene = Scene::buildScene(&player1);

	//Set the world camerea
	gEngine->setUpCameraAbsolute(cml::vector3f(0.0,0.0,20.0), cml::vector3f(0.0,0.0,0.0));
	gEngine->setLightTarget(0.0,0.5,0.0);
	gEngine->setLightPos(-5.0, 5.0,0.0);

	
	//todo: 2 cai' boxes nay chi la PhysicalBody, ve sau no' se nam trong monster class, hay gi do...
	box1 = new PhysicalBody();
	box1->setMesh("unitBox.obj");
	box1->setTexture(MapType::diffuse, "blue.jpg");
	box1->setTexture(MapType::normal, "white.jpg");
	box1->translateAbsolute(4.0,0.0,0.0);
	box1->setScale(1.0,1.0,1.0);
	
	//deo hieu ca'i nay de lam gi, cha'c xem sau
	player1.SetKeyBuffer(keysPressed); 
}

void display()
{
	int elapsedTime=glutGet(GLUT_ELAPSED_TIME);

	gEngine->beginRendering();

	//todo: cho het nhung ca'i draw() nay vao trong scene->draw(), hay gi do'...
	player1.draw();
	
	//todo: day chi la 2 ca'i test boxes, ve sau no' se thuoc vao` monster.draw()
	gEngine->renderPhysicalBody(box1);
	//gEngine->RenderNormalMapObject(box2);

	gEngine->endRendering();
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
	//gEngine->UpdateCamera();
	
	/*
	//update score text:
	gEngine->ChangeTex("score","Score:"+Utility::inToString(a.GetCornEatean()));
	gEngine->ChangeTex("lives","Lives:"+Utility::inToString(lives));
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
	glutInitWindowSize( 800, 600);
	glViewport(0,0, 800, 600);
	glutCreateWindow("Resident Evil Remake");

	//glutPassiveMotionFunc(processMouse);
	glutDisplayFunc(display); 
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutTimerFunc(15, Update, 0);
	
	init();
	
	glutDisplayFunc(display); 
	glutWarpPointer(800/2.0,600/2.0);
	glutMainLoop();
	return 0;
}
