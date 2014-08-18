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
	gEngine=Raptor::RaptorEngine::GetInstance();
	//Setup renderer
	gEngine->startUp();
	

	gEngine->loadObjMesh("ghost.obj");
	gEngine->loadObjMesh("unitBox.obj");
	gEngine->loadTexture("pacman.png");
	gEngine->loadTexture("white.jpg");
	gEngine->loadTexture("blue.jpg");
	gEngine->loadTexture("red.jpg");
	
	//Set the world camerea
	gEngine->setUpCameraAbsolute(cml::vector3f(10.0,10.0,20.0), cml::vector3f(0.0,0.0,0.0));
	gEngine->setLightTarget(0.0,0.5,0.0);
	gEngine->setLightPos(-5.0, 5.0,0.0);

	xAxis = new Raptor::PhysicalBody();
	xAxis->setMesh("unitBox.obj");
	xAxis->setTexture(Raptor::MapType::diffuse, "wally.jpg");
	xAxis->setTexture(Raptor::MapType::normal, "white.jpg");
	xAxis->translateAbsolute(2.5,0.0,0.0);
	xAxis->setScale(5.0,0.1,0.1); 

	yAxis = new Raptor::PhysicalBody();
	yAxis->setMesh("unitBox.obj");
	yAxis->setTexture(Raptor::MapType::diffuse, "red.jpg");
	yAxis->setTexture(Raptor::MapType::normal, "white.jpg");
	yAxis->setScale(0.1,5.0,0.1); 
	yAxis->translateAbsolute(0.0,0.0,0.0);

	zAxis = new Raptor::PhysicalBody();
	zAxis->setMesh("unitBox.obj");
	zAxis->setTexture(Raptor::MapType::diffuse, "red.jpg");
	zAxis->setTexture(Raptor::MapType::normal, "white.jpg");
	zAxis->setScale(0.1,0.1,5.0); 
	zAxis->translateAbsolute(0.0,0.0,2.5);
	
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
	gEngine->renderPhysicalBody(xAxis);
	gEngine->renderPhysicalBody(yAxis);
	gEngine->renderPhysicalBody(zAxis);
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

