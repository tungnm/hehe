#ifndef Scene_h__
#define Scene_h__

#include<vector>

class Appearance;

class Scene
{
	public:
		Scene()
		{

		}

		~Scene()
		{

		}

		void update(); //Scene update itself;
		void render();

	private:
		std::vector<Appearance*> objList;
		Appearance *player; 
		//Entity background;
		//cml::vector3f playerStartPos;
		Scene *next;
		Scene *prev;
};

#endif // Scene_h__