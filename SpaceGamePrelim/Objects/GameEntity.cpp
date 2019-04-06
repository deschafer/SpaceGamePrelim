
#include "GameEntity.h"
#include "..\Frame\MainApplication.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Components\Component.h"



GameEntity::GameEntity() :
	m_CurrentFrame(1),
	m_CurrentRow(1)
{
}

GameEntity::~GameEntity()
{
}

GameEntity::GameEntity(std::string ReducedTexture,
	TextureProperties* Properties,
	std::string TypeID,
	std::string SpecTypeID,
	Vector InitPosition,
	Vector InitVelocity,
	Vector InitAccel) :
	GameEntity() 

{
	m_Acceleration = InitAccel;
	m_Velocity = InitVelocity;
	m_Position = InitPosition;

	m_SpecTypeID = SpecTypeID;
	m_TypeID = TypeID;

	m_ReducedTextureID = ReducedTexture;
	m_Dimensions = Properties->GetDimensions();
	m_AnimationSpeed = Properties->GetAnimationSpeed();
	m_NumberFrames = Properties->GetNumberFrames();

	m_RedTextureIndex = TextureManager::Instance()->GetRedTextureIndex(m_ReducedTextureID);
}

GameEntity::GameEntity(std::string ReducedTexture,
	TextureProperties* Properties,
	std::string TypeID,
	std::string SpecTypeID,
	Vector InitPosition) :
	GameEntity()
{

	m_ReducedTextureID = ReducedTexture;
	m_Dimensions = Properties->GetDimensions();
	m_AnimationSpeed = Properties->GetAnimationSpeed();
	m_NumberFrames = Properties->GetNumberFrames();

	m_RedTextureIndex = TextureManager::Instance()->GetRedTextureIndex(m_ReducedTextureID);

	m_Acceleration = Vector(0, 0);
	m_Velocity = Vector(0, 0);
	m_Position = InitPosition;
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
	Vector InitVelocity,
	Vector InitAccel,
	Vector InitPosition,
	Callback Handler)
{

	m_Position = InitPosition;
	m_Velocity = InitVelocity;
	m_Acceleration = InitAccel;
	m_SpecTypeID = SpecTypeID;
	m_TypeID = TypeID;

	m_CurrentFrame = 1;
	m_CurrentRow = 1;

	m_Dimensions = Properties->GetDimensions();
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

	// Update position and velcoity
	m_Position += m_Velocity;
	m_Velocity += m_Acceleration;
}


//
// Draw()
// Default version draws the current frame based on properties
// within this class.
//
void GameEntity::Draw()
{
	SDL_Renderer* Temp = MainApplication::Instance()->GetRenderer();
	
	TextureManager::Instance()->DrawCurrentFrame(
		static_cast<int>(round(m_Position.getX())), 
		static_cast<int>(round(m_Position.getY())),
		m_RedTextureIndex,
		SDL_FLIP_NONE,
		MainApplication::Instance()->GetRenderer(), 
		m_CurrentRow, 
		m_CurrentFrame);
}

//
// Delete()
// Used for termination and clean up prior
// to termination.
//
void GameEntity::Delete() 
{

}

void GameEntity::OnCollision()
{

}

void GameEntity::OnDamaged()
{

}

void GameEntity::OnMovement()
{

}

void GameEntity::OnInteraction()
{

}

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