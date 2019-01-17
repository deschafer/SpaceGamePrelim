#pragma once

#include "GameObject.h"
#include "BasicTypes.h"


#include <string>

class GameEntity : public GameObject
{
public:
	GameEntity();
	virtual ~GameEntity();
	GameEntity(TextureProperties* Properties, 
		Vector InitPosition, Vector InitVelocity, Vector InitAccel);
	GameEntity::GameEntity(TextureProperties* Properties, Vector InitPosition);


	virtual bool Load(TextureProperties* Properties);
	virtual void Update();
	virtual void Draw();
	virtual void Delete();


private:

	// Movement properties
	Vector m_Position;
	Vector m_Velocity;
	Vector m_Acceleration;

	Rect m_Dimensions;

	// Animation properties
	int m_CurrentFrame;
	int m_NumberFrames;
	int m_AnimationSpeed;
	int m_CurrentRow;

	std::string m_TextureID;
};

