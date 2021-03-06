#pragma once

#include "GameObject.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\BasicTypes\EntityDirection.h"
#include "..\Interactions\Interactable.h"
#include "Locatable.h"

#include <string>
#include <vector>

class Component;

class GameEntity : 
	public GameObject, 
	protected Interactable
{
public:
	GameEntity(Scene* Parent);
	GameEntity(Scene* ParentScene, 
		std::string ReducedTexture,
		TextureProperties* Prop,
		std::string TypeID, 
		std::string SpecTypeID,
		Vector InitPosition, 
		Vector InitVelocity, 
		Vector InitAccel);
	GameEntity(Scene* ParentScene, 
		std::string ReducedTexture,
		TextureProperties* Prop,
		std::string TypeID,
		std::string SpecTypeID,
		Rect DestRect,
		Vector InitPosition,
		Vector InitVelocity,
		Vector InitAccel);
	GameEntity(Scene* ParentScene, 
		std::string ReducedTexture,
		TextureProperties* Prop,
		std::string TypeID,
		std::string SpecTypeID,
		Vector InitPosition);
	virtual ~GameEntity();
	virtual bool GameEntity::Load(
		TextureProperties* Properties,
		std::string TypeID,
		std::string SpecTypeID,
		Rect DestRect,
		Vector InitVelocity,
		Vector InitAccel,
		Vector InitPosition,
		Callback Handler);
	virtual void Update();
	virtual bool Draw();
	virtual void Delete();
	virtual void OnCollision();
	virtual void OnDamaged();
	virtual void OnMovement();
	virtual bool OnInteraction(GameEntity * InteractingEntity) override;

	virtual void SetFrame(int FrameNumber);
	virtual void SetVelocity(Vector NewVel) { m_Velocity = NewVel; }
	virtual void SetComponent(Component* NewComp) { m_Components.push_back(NewComp); }
	virtual void SetDirection(EntityDirection Horiz, EntityDirection Verti) { m_HorizMovement = Horiz, m_VertiMovement = Verti; }
	virtual void SetPosition(Vector NewPos) { m_Position = NewPos; }
	virtual void EnglargeDestination(int Number) { m_DestDimensions.Enlarge(Number); }
	virtual void SetDestinationRect(Rect Dest) { m_DestDimensions = Dest; }

	virtual Vector GetPosition() { return m_Position; }
	virtual Rect GetDimensions();
	virtual Rect GetDestDimensions() { return m_DestDimensions; }
	virtual EntityDirection GetDirectionHoriz() { return m_HorizMovement; }
	virtual EntityDirection GetDirectionVerti() { return m_VertiMovement; }
	virtual Vector GetVelocity() { return m_Velocity; }

	virtual void ExecuteCallback() { m_Callback(); }

	// Interactable and Locatable overrides
	virtual Vector GetInteractablePosition() override { return m_Position; }
	virtual Vector GetLocatableScreenPosition() override { return m_Position; }

	// drawable implementation
	virtual void AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames);
	virtual std::vector<std::string> GetTextures();
	virtual std::vector<int> GetTextureIndices();
	virtual std::vector<int> GetAnimationSpeeds();
	virtual std::vector<int> GetNumberFrames();
	virtual void ClearTextures();

protected:

	// Movement properties
	Vector m_Position;
	Vector m_Velocity;
	Vector m_Acceleration;
	EntityDirection m_HorizMovement;
	EntityDirection m_VertiMovement;

	Rect m_Dimensions;		// Same as the red. text. dimensions
	Rect m_DestDimensions;	// If the entity is diff than the red. texture

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

