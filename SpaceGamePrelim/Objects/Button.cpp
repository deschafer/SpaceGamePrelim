
#include "Button.h"
#include "..\Components\ButtonInteractComp.h"

#include <iostream>

using namespace std;

Button::Button() : 
	m_Released(false),
	m_Clicked(false),
	GameEntity(nullptr)
{
}

Button::Button(Scene* Parent,
	std::string RedTexture, 
	TextureProperties* Prop,
	std::string TypeID, 
	std::string SpecID, 
	Vector InitPosition,
	Vector InitVelocity, 
	Vector InitAccel, 
	Callback callBack) :
	GameEntity(Parent, RedTexture, Prop, TypeID, SpecID, InitPosition, InitVelocity, InitAccel),
	m_Released(false),
	m_Clicked(false)
{
	m_Callback = callBack;
}

Button::~Button()
{
}

