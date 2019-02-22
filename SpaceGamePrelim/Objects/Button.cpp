
#include "Button.h"

#include <iostream>

using namespace std;

Button::Button() : 
m_Released(false),
m_Clicked(false)
{
}

Button::Button(std::string RedTexture, 
	TextureProperties* Prop,
	std::string TypeID, 
	std::string SpecID, 
	Vector InitPosition,
	Vector InitVelocity, 
	Vector InitAccel, 
	Callback callBack) :
	GameEntity(RedTexture, Prop, TypeID, SpecID, InitPosition, InitVelocity, InitAccel),
	m_Released(false),
	m_Clicked(false)
{
	m_Callback = callBack;
}

Button::~Button()
{
}

void Button::Update()
{

	GameEntity::Update();

	Vector* CurrentMousePosition = 
		InputManager::Instance()->GetCurrentMousePosition();

	if (CurrentMousePosition->getX() < (m_Position.getX() + m_Dimensions.Width())
		&& CurrentMousePosition->getX() > m_Position.getX()
		&& CurrentMousePosition->getY() < (m_Position.getY() + m_Dimensions.Height())
		&& CurrentMousePosition->getY() > m_Position.getY())
	{
		if (InputManager::Instance()->GetMouseButtonState(MouseInput::LEFT_MB)
			&& m_Released)
		{
			m_CurrentFrame = MOUSE_CLICKED;

			m_Clicked = true;

		}
		else if (m_Clicked)
		{
			cout << "yep";
			m_Callback();
			m_Released = false;
			m_Clicked = false;

		}
		else if (!InputManager::Instance()
			->GetMouseButtonState(MouseInput::LEFT_MB))
		{
			m_Released = true;
			m_CurrentFrame = MOUSE_OVER;
			m_Clicked = false;
		}
		else
		{
			m_Released = false;
			m_Clicked = false;
		}
	}
	else
	{
		m_CurrentFrame = MOUSE_OUT;
		m_Released = false;
		m_Clicked = false;
	}

}

