#include "GameEntity.h"
#include "MainApplication.h"

#include "TextureManager.h"


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
	m_CurrentRow = Properties->GetCurrentRow();
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
	m_Velocity += m_Velocity;

	// need to set rect dimensions !!!

}


//
// Draw()
// Default version draws the current frame based on properties
// within this class.
//
void GameEntity::Draw()
{
	SDL_Renderer* Temp = MainApplication::Instance()->GetRenderer();
	
	TextureManager::Instance()->DrawCurrentFrame(m_TextureID, m_Dimensions, SDL_FLIP_NONE,
		MainApplication::Instance()->GetRenderer(), m_CurrentRow, m_CurrentFrame);
	//TextureManager::Instance()->DrawCurrentFrame("Source", Rect(0, 0, 100, 100), SDL_FLIP_NONE, Temp, 3);
}

//
// Delete()
// Used for termination and clean up prior
// to termination.
//
void GameEntity::Delete() 
{

}