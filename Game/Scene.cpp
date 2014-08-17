
#include "Scene.h"
#include "Player.h"

#include "DbgOut.h"

Scene::Scene()
{
	this->next=NULL;
	this->prev=NULL;

	
}

void Scene::update() /*Scene update itself */
{
	for(unsigned int i=0;i<this->objList.size();i++)
	{
		this->objList[i]->Update();
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
		debug()<<"Key a is pressed";
		this->player->SetSpeed(2.5f)	;
		((PacMan *)player)->ChangeDirection(Dir::dleft); //
	}
}
