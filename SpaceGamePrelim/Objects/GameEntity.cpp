
#include "GameEntity.h"
#include "..\Frame\MainApplication.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Components\Component.h"



GameEntity::GameEntity(Scene* ParentScene) :
	m_CurrentFrame(1),
	m_CurrentRow(1),
	m_DestDimensions(0, 0, 0, 0),
	Interactable(true),
	GameObject(ParentScene)
{
}

GameEntity::~GameEntity()
{
}

GameEntity::GameEntity(Scene* ParentScene,
	std::string ReducedTexture,
	TextureProperties* Properties,
	std::string TypeID,
	std::string SpecTypeID,
	Vector InitPosition,
	Vector InitVelocity,
	Vector InitAccel) :
	GameEntity(ParentScene)
{
	m_Acceleration = InitAccel;
	m_Velocity = InitVelocity;
	m_Position = InitPosition;

	m_SpecTypeID = SpecTypeID;
	m_TypeID = TypeID;

	m_ReducedTextureID = ReducedTexture;
	m_Dimensions = Properties->GetDimensions();
	m_DestDimensions = Properties->GetDimensions();
	m_AnimationSpeed = Properties->GetAnimationSpeed();
	m_NumberFrames = Properties->GetNumberFrames();

	m_RedTextureIndex = TextureManager::Instance()->GetRedTextureIndex(m_ReducedTextureID);
}

GameEntity::GameEntity(Scene* ParentScene, 
	std::string ReducedTexture,
	TextureProperties* Prop,
	std::string TypeID,
	std::string SpecTypeID,
	Rect DestRect,
	Vector InitPosition,
	Vector InitVelocity,
	Vector InitAccel) :
	GameEntity(ParentScene, ReducedTexture, Prop, TypeID, SpecTypeID, InitPosition, InitVelocity, InitAccel)
{
	m_DestDimensions = DestRect;
}

GameEntity::GameEntity(Scene* ParentScene, 
	std::string ReducedTexture,
	TextureProperties* Properties,
	std::string TypeID,
	std::string SpecTypeID,
	Vector InitPosition) :
	GameEntity(ParentScene, ReducedTexture, Properties, TypeID, SpecTypeID, InitPosition, Vector(0, 0), Vector(0, 0))
{
}

//
// Load()
// Loads this Game object's corresponding texture in the
// the texturemanager object.
//
bool GameEntity::Load(
	TextureProperties* Properties,
	std::string TypeID,
	std::string SpecTypeID,
	Rect DestRect,
	Vector InitVelocity,
	Vector InitAccel,
	Vector InitPosition,
	Callback Handler)
{
	// Less than 0 for pos means to be manually set by somewhere else
	m_Position = InitPosition;
	m_Velocity = InitVelocity;
	m_Acceleration = InitAccel;
	m_SpecTypeID = SpecTypeID;
	m_TypeID = TypeID;
	SetLocatableScreenPosition(m_Position);

	m_CurrentFrame = 1;
	m_CurrentRow = 1;

	m_Dimensions = Properties->GetDimensions();
	(DestRect.Height()) ? m_DestDimensions = DestRect : m_DestDimensions = m_Dimensions;
	m_AnimationSpeed = Properties->GetAnimationSpeed();
	m_NumberFrames = Properties->GetNumberFrames();
	m_ReducedTextureID = Properties->GetTextureID();

	m_RedTextureIndex = TextureManager::Instance()->GetRedTextureIndex(m_ReducedTextureID);

	m_Callback = Handler;

	return true;
}

//
// Update()
// Default version is to update the current
// position based on velocity and acceleration
//
void GameEntity::Update()
{
	for (size_t i = 0; i < m_Components.size(); i++)
	{
		m_Components[i]->Execute();
	}

	// Update position and velocity
	//m_Position += m_Velocity;
	//m_Velocity += m_Acceleration;

	// We Eliminate any floating point errors here
	float X = abs(m_Velocity.getX());
	float Y = abs(m_Velocity.getY());

	if (X < 0.01 && X != 0)
		m_Velocity.setX(0);
	if (Y < 0.01 && Y != 0)
		m_Velocity.setY(0);

	// set the dimensions of the drawable class
	Drawable::m_Dimensions = Rect(m_Position, m_DestDimensions.Width(), m_DestDimensions.Height());
}


//
// Draw()
// Default version draws the current frame based on properties
// within this class.
//
bool GameEntity::Draw()
{
	if (Drawable::Draw())
	{
		SDL_Renderer* Temp = MainApplication::Instance()->GetRenderer();
		SetLocatableScreenPosition(m_Position);

		TextureManager::Instance()->DrawFrame(
			static_cast<int>(round(m_Position.getX())),
			static_cast<int>(round(m_Position.getY())),
			m_RedTextureIndex,
			SDL_FLIP_NONE,
			MainApplication::Instance()->GetRenderer(),
			m_DestDimensions,
			GetColor(),
			m_CurrentRow,
			m_CurrentFrame);
	}

	return m_Visible;
}

//
// Delete()
// Used for termination and clean up prior
// to termination.
//
void GameEntity::Delete() 
{
}

//
//
//
//
void GameEntity::OnCollision()
{

}

//
//
//
//
void GameEntity::OnDamaged()
{

}

//
//
//
//
void GameEntity::OnMovement()
{

}

//
//
//
//
bool GameEntity::OnInteraction(GameEntity * InteractingEntity)
{
	return Interactable::OnInteraction(InteractingEntity);
}

Rect GameEntity::GetDimensions()
{
	if (m_DestDimensions.Empty())
	{
		return m_Dimensions;
	}
	else return m_DestDimensions;
}

//
// Note: Since a GameEntity only supports a single image/texture, we just replace it
//
void GameEntity::AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames)
{
	m_RedTextureIndex = RedTextureIndex;
	m_ReducedTextureID = RedTextureID;
	m_AnimationSpeed = AnimationSpeed;
	m_NumberFrames = NumberFrames;
}

std::vector<std::string> GameEntity::GetTextures()
{
	std::vector<std::string> Textures;
	Textures.push_back(m_ReducedTextureID);
	return Textures;
}

std::vector<int> GameEntity::GetTextureIndices()
{
	std::vector<int> Indices;
	Indices.push_back(m_RedTextureIndex);
	return Indices;
}

std::vector<int> GameEntity::GetAnimationSpeeds()
{
	std::vector<int> AnimationSpeed;
	AnimationSpeed.push_back(m_AnimationSpeed);
	return AnimationSpeed;
}

std::vector<int> GameEntity::GetNumberFrames()
{
	std::vector<int> NumberFrames;
	NumberFrames.push_back(m_NumberFrames);
	return NumberFrames;
}

void GameEntity::ClearTextures()
{
	m_Visible = false;
}

//
//
//
//
void GameEntity::SetFrame(int FrameNumber)
{
	if (m_CurrentFrame < m_NumberFrames)
	{
		m_CurrentFrame = FrameNumber;
	}
	else
	{
#ifdef _DEBUG
		std::cout << "Frame number not within range" << std::endl;
#endif // _DEBUG
	}
}
