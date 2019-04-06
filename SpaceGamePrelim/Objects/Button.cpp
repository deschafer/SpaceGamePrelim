
#include "Button.h"
#include "..\Components\ButtonInteractComp.h"

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

