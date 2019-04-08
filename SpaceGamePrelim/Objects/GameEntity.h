#pragma once

#include "GameObject.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\BasicTypes\EntityDirection.h"

#include <string>
#include <vector>

class Component;

class GameEntity : public GameObject
{
public:
	GameEntity();
	GameEntity(std::string ReducedTexture,
		TextureProperties* Prop,
		std::string TypeID, 
		std::string SpecTypeID,
		Vector InitPosition, 
		Vector InitVelocity, 
		Vector InitAccel);
	GameEntity(std::string ReducedTexture,
		TextureProperties* Prop,
		std::string TypeID,
		std::string SpecTypeID,
		Vector InitPosition);
	virtual ~GameEntity();
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
	virtual void OnCollision();
	virtual void OnDamaged();
	virtual void OnMovement();
	virtual void OnInteraction();

	virtual void SetFrame(int FrameNumber);
	virtual void SetVelocity(Vector NewVel) { m_Velocity = NewVel; }
	virtual void SetComponent(Component* NewComp) { m_Components.push_back(NewComp); }
	virtual void SetDirection(EntityDirection Horiz, EntityDirection Verti) { m_HorizMovement = Horiz, m_VertiMovement = Verti; }

	virtual Vector GetPosition() { return m_Position; }
	virtual Rect GetDimensions() { return m_Dimensions; }
	virtual void ExecuteCallback() { m_Callback(); }

protected:

	// Movement properties
	Vector m_Position;
	Vector m_Velocity;
	Vector m_Acceleration;
	EntityDirection m_HorizMovement;
	EntityDirection m_VertiMovement;

	Rect m_Dimensions;

	// Animation properties
	int m_CurrentFrame;
	int m_NumberFrames;
	int m_AnimationSpeed;
	int m_CurrentRow;

	Callback m_Callback;

	int m_RedTextureIndex;
	std::string m_ReducedTextureID;
	std::string m_TypeID;
	std::string m_SpecTypeID;

	std::vector<Component*> m_Components;
};

