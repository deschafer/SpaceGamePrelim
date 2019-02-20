#pragma once

#include "GameObject.h"
#include "..\BasicTypes\BasicTypes.h"


#include <string>

class GameEntity : public GameObject
{
public:
	GameEntity();
	virtual ~GameEntity();

	GameEntity(TextureProperties* Properties, 
		Vector InitPosition, 
		Vector InitVelocity, 
		Vector InitAccel);
	GameEntity(TextureProperties* Properties, 
		Vector InitPosition);

	// New constructors
	GameEntity(TextureProperties* Properties, 
		std::string TypeID, 
		std::string SpecTypeID,
		Vector InitPosition, 
		Vector InitVelocity, 
		Vector InitAccel);
	GameEntity(TextureProperties* Properties, 
		std::string TypeID,
		std::string SpecTypeID,
		Vector InitPosition);

	virtual bool Load(
		TextureProperties* Properties,
		std::string TypeID,
		std::string SpecTypeID,
		Vector InitVelocity,
		Vector InitAccel,
		Vector InitPosition,
		Callback Handler);
	virtual void Update();
	virtual void Draw();
	virtual void Delete();


protected:

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

	bool m_UsingReducedTextures;
	Callback m_Callback;

	std::string m_TextureID;
	std::string *m_ReducedTextureID;
	std::string m_TypeID;
	std::string m_SpecTypeID;
};

