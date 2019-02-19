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
	Button(TextureProperties* Properties, Vector InitPosition, 
		Vector InitVelocity, Vector InitAccel, Callback callBack);

	//virtual bool Load(TextureProperties* Properties);
	virtual void Update() override;



	~Button();
};

class ButtonCreator : public BaseCreator
{
public:

	virtual GameObject* CreateObject() override
	{
		return new Button;
	}

};

