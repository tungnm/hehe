#ifndef IGameObject_h__
#define IGameObject_h__

class IGameObject
{

public:
	virtual ~IGameObject(){}; //virtual destructor for interface
		virtual void Update()=0;
		virtual void Draw() =0;

protected:
	IGameObject(){}; //Forbid interface to be instantiated

};

#endif // IGameObject_h__