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
#include "Scene.h"

void Close()
{
	gEngine->shutDown();
}
void UpdateKeyboard()
{

	mainScene->updateKeyBoard(keysPressed);
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
	
	//Set the world camerea
	gEngine->setUpCameraAbsolute(cml::vector3f(10.0,10.0,20.0), cml::vector3f(0.0,0.0,0.0));
	gEngine->setLightTarget(0.0,0.5,0.0);
	gEngine->setLightPos(-5.0, 5.0,0.0);
	
	
	player = new Survivor();
	mainScene = Scene::buildScene(player);


	//todo: 2 cai' boxes nay chi la PhysicalBody, ve sau no' se nam trong monster class, hay gi do...
	enemy=new Monster();
	
	mainScene->add(enemy);

	//deo hieu ca'i nay de lam gi, cha'c xem sau
	//player.SetKeyBuffer(keysPressed); 
}

void display()
{
	int elapsedTime=glutGet(GLUT_ELAPSED_TIME);

	gEngine->beginRendering();

	mainScene->draw();

	gEngine->endRendering();
	glutSwapBuffers();

}

void Update(int value)
{
	UpdateKeyboard();
	
	float deltaTime=1.0/60.0; //Assume 60FPS

	mainScene->update(deltaTime);
	
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

