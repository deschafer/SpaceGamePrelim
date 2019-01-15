#pragma once


// Basic, abstract object class
class GameObject
{
protected:

	virtual ~GameObject();

	//GameObject(loader class goes here);

public:

	virtual bool Load(/* takes the loader class here too*/) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Delete() = 0;

	GameObject();

};

