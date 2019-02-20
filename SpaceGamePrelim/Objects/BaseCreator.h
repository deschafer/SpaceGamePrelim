#pragma once

#include "GameObject.h"

class BaseCreator
{
protected:
	
	BaseCreator() {}

public:
	
	virtual GameObject* CreateObject() = 0;
};


