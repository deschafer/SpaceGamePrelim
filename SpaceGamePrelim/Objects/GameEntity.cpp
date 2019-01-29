
#include "GameEntity.h"
#include "..\Frame\MainApplication.h"
#include "..\TextureCode\TextureManager.h"


GameEntity::GameEntity()
{
}

GameEntity::~GameEntity()
{
}

GameEntity::GameEntity(TextureProperties* Properties,
	Vector InitPosition, Vector InitVelocity, Vector InitAccel)
{
	m_Acceleration = InitAccel;
	m_Velocity = InitVelocity;
	m_Position = InitPosition;

	m_Dimensions = Properties->GetDimensions();
	m_TextureID = Properties->GetTextureID();
	m_CurrentFrame = Properties->GetCurrentFrame();
	m_AnimationSpeed = Properties->GetAnimationSpeed();
	m_NumberFrames = Properties->GetNumberFrames();
	m_CurrentRow = Properties->GetCurrentRow();
}

GameEntity::GameEntity(TextureProperties* Properties, Vector InitPosition)
{
	m_Dimensions = Properties->GetDimensions();
	m_TextureID = Properties->GetTextureID();
	m_CurrentFrame = Properties->GetCurrentFrame();
	m_AnimationSpeed = Properties->GetAnimationSpeed();
	m_CurrentRow = Properties->GetCurrentRow();
	m_NumberFrames = Properties->GetNumberFrames();

	m_Acceleration = Vector(0, 0);
	m_Velocity = Vector(0, 0);
	m_Position = InitPosition;
}

//
// Load()
// Loads this Game object's corresponding texture in the
// the texturemanager object.
//
bool GameEntity::Load(TextureProperties* Properties)
{
	return true;
}

//
// Update()
// Default version is to update the current
// position based on velocity and acceleration
//
void GameEntity::Update()
{

	m_Position += m_Velocity;
	m_Velocity += m_Acceleration;

	m_CurrentFrame = int(((SDL_GetTicks() / m_AnimationSpeed) % m_NumberFrames));

}


//
// Draw()
// Default version draws the current frame based on properties
// within this class.
//
void GameEntity::Draw()
{
	SDL_Renderer* Temp = MainApplication::Instance()->GetRenderer();
	
	TextureManager::Instance()->DrawCurrentFrame(m_TextureID, m_Position.getX(), 
		m_Position.getY(), m_Dimensions, SDL_FLIP_NONE, MainApplication::Instance()->GetRenderer(), 
		m_CurrentRow, m_CurrentFrame);
}

//
// Delete()
// Used for termination and clean up prior
// to termination.
//
void GameEntity::Delete() 
{

}