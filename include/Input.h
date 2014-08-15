#ifndef U_INPUT1
#define U_INPUT1

//=====================USER INPUT FUNCTIONS
	int keysPressed[255];
	//displacement of mouse since last mouse update
	int mouseDisplacementX;
	int mouseDisplacementY;
		
	void processNormalKeysUp(unsigned char key, int x, int y)
	{
	if(key!='a'&&key!='d'&&key!='s'&&key!='w')
		keysPressed[key]=0;
	}
	void processNormalKeys(unsigned char key, int x, int y) 
	{
		if (key>=0&&key<=254 )
		{
		
			if(key=='a'){keysPressed['d']=0;keysPressed['w']=0;keysPressed['s']=0;	keysPressed[key]=15;}
			else
				if(key=='s'){keysPressed['d']=0;keysPressed['w']=0;keysPressed['a']=0;	keysPressed[key]=15;}
				else if(key=='d'){keysPressed['a']=0;keysPressed['w']=0;keysPressed['s']=0;	keysPressed[key]=15;}
				else if(key=='w'){keysPressed['a']=0;keysPressed['a']=0;keysPressed['s']=0;	keysPressed[key]=15;}
				else keysPressed[key]=1;
		}
	}
	void processMouse(int x, int y)
	{

	 mouseDisplacementX=x-W_WIDTH/2.0;
	 mouseDisplacementY=y-W_HEIGHT/2.0;

	}
//=========================================



#endif