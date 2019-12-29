#pragma once

#include "..\BasicTypes\BasicTypes.h"
#include ".\Colorable.h"

// Basic, abstract object class
class GameObject : public Colorable
{
protected:

	virtual ~GameObject();

public:

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Delete() = 0;
	virtual bool Load(
		TextureProperties* Properties,
		std::string TypeID,
		std::string SpecTypeID,
		Rect DestRect,
		Vector InitVelocity,
		Vector InitAccel,
		Vector InitPosition,
		Callback Handler) = 0;

	GameObject();
};