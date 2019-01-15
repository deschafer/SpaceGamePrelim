#pragma once

#include "GameObject.h"
#include "BasicTypes.h"

#include <string>

class GameEntity : public GameObject
{
public:
	GameEntity();
	virtual ~GameEntity();
	GameEntity(/*loader class,*/ Vector InitPosition, Vector InitVelocity, Vector InitAccel);

	virtual bool Load(/* params here*/) {}
	virtual void Update();
	virtual void Draw();
	virtual void Delete();


private:

	// Movement properties
	Vector m_Position;
	Vector m_Velocity;
	Vector m_Acceleration;

	Rect Dimensions;

	// Animation properties
	int m_CurrentFrame;
	int m_NumberFrames;
	int m_AnimationSpeed;
	int m_CurrentRow;

	std::string m_TextureID;
};

