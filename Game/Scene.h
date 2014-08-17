#ifndef Scene_h__
#define Scene_h__

#include<vector>

//class Appearance;
//class Survivor;
class Player;

class Scene //Should be converted into abstract or base Scene. It's ok for now.
{
	public:
		Scene();
		

		~Scene()
		{

		}

		void update(float dt); //Scene update itself;
		void draw();
		void add(Player * obj);

		Scene * nextScene();
		Scene * prevScene();

		static Scene * buildScene(Player *mainPlayer); //Factory method

		void updateKeyBoard(int keys[]);
		void setMainPlayer(Player *mainPlayer);
	private:
		std::vector<Player *> objList;
		Player *player; 

		//Entity background;
		//cml::vector3f playerStartPos;
		Scene *next;
		Scene *prev;

		
};

//RegularScene

//PuzzeScene

//Intro, story Scene

class SceneList
{
	std::vector<Scene *> scenes;
};

#endif // Scene_h__