#ifndef U_INPUT1
#define U_INPUT1

//=====================USER INPUT FUNCTIONS
	int keysPressed[255];
	//displacement of mouse since last mouse update
	int mouseDisplacementX;
	int mouseDisplacementY;
		
	void processNormalKeysUp(unsigned char key, int x, int y)
	{
		keysPressed[key]=0;
	}
	void processNormalKeys(unsigned char key, int x, int y) 
	{
		keysPressed[key]=1;
	
	}
	void processMouse(int x, int y)
	{

	 //mouseDisplacementX=x-W_WIDTH/2.0;
	 //mouseDisplacementY=y-W_HEIGHT/2.0;

	}
//=========================================



#endif