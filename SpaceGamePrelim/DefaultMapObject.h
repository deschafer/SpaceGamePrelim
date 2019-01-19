#pragma once

#include "MapObject.h"

#include <string>

class DefaultMapObject : public MapObject
{
private:

	std::string m_TextureID;



public:

	virtual void Draw() override
	{

	}


	DefaultMapObject() : m_TextureID("DefaultMap")
	{
	}
	virtual ~DefaultMapObject();
};

