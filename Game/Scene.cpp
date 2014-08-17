
#include "Scene.h"
#include "Player.h"

#include "DbgOut.h"
#include "..\RaptorEngine\interface\PhysicalBody.h"

Scene::Scene()
{
	this->next=NULL;
	this->prev=NULL;


}

void Scene::update(float dt) /*Scene update itself */
{
	this->player->Update(dt);
	
	for(unsigned int i=0;i<this->objList.size();i++)
	{
		this->objList[i]->Update(dt);
	}
	
}

void Scene::draw()
{
	this->player->draw();

	for(unsigned int i=0;i<this->objList.size();i++)
	{
		this->objList[i]->draw();
	}
}

void Scene::add( Player * obj )
{
	this->objList.push_back(obj);
}

Scene * Scene::nextScene()
{
	return this->next;
}

Scene * Scene::prevScene()
{
	return this->prev;
}


Scene * Scene::buildScene( Player *mainPlayer )
{
	auto retScene=new Scene();
	retScene->setMainPlayer(mainPlayer);

	return retScene;
}

void Scene::setMainPlayer( Player *mainPlayer )
{
	this->player=mainPlayer;
}

void Scene::updateKeyBoard( int keys [])
{
	if (keys['a'])
	{
		
		this->player->SetRotateSpeed(2.25f);
		this->player->rotateY();
	}

	if (keys['d'])
	{
		this->player->SetRotateSpeed(-2.25f);
		this->player->rotateY();
	}


	if (keys['w'])
	{
		this->player->SetSpeed(2.25f);
		this->player->forward();
	}

	if (keys['s'])
	{
		this->player->SetRotateSpeed(-2.25f);
		this->player->SetSpeed(2.25f);
		this->player->backward();
	}
}
