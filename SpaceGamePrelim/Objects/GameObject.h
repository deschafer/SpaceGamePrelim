#pragma once

#include "..\BasicTypes\BasicTypes.h"

// Basic, abstract object class
class GameObject
{
protected:

	virtual ~GameObject();

	//GameObject(loader class goes here);

public:

	virtual bool Load(TextureProperties *Properties) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Delete() = 0;

	GameObject();

};

