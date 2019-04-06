#pragma once

#include "GameEntity.h"
#include "BaseCreator.h"
#include "..\Frame\InputManager.h"

class Button : public GameEntity
{	
private:

	enum States {MOUSE_OUT, MOUSE_OVER, MOUSE_CLICKED};

	bool m_Clicked;
	bool m_Released;

public:
	Button();
	Button(std::string RedTexture,
		TextureProperties* Prop,
		std::string TypeID,
		std::string SpecID,
		Vector InitPosition,
		Vector InitVelocity,
		Vector InitAccel,
		Callback callBack);

	~Button();
};

class ButtonCreator : public BaseCreator
{
public:

	ButtonCreator() {}
	virtual ~ButtonCreator() {}

	virtual GameObject* CreateObject() override
	{
		return new Button;
	}

};

