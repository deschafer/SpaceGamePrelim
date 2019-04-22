#pragma once

#include "..\BasicTypes\BasicTypes.h"

// Basic, abstract object class
class GameObject
{
protected:

	virtual ~GameObject();

public:

	virtual bool Load(
		TextureProperties *Properties,
		std::string TypeID,
		std::string SpecTypeID,
		Rect DestRect,
		Vector Velocity,
		Vector Accel,
		Vector Position,
		Callback Handler) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Delete() = 0;

	GameObject();


};

