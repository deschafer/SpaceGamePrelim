#include "Action.h"
#include "..\Frame\MainApplication.h"
#include "..\TextureCode\TextureManager.h"

//
// Action()
// Creates a new Action object. The DrawableObject param
//	is the object this Action is being drawn over.
// The Name is for identification and debugging, and the duration
//	is the time taken from start to completion
// Position refers to the top left position of this action in screen coordinates
//
Action::Action(Drawable* DrawableObject, std::string Name, double Duration, Rect Position, Scene* Parent) :
	m_DrawableObject(DrawableObject),
	m_Name(Name),
	m_Duration(Duration),
	m_Dimensions(Position),
	m_Start(false),
	Drawable(Position, Parent)
{
}

void Action::Start()
{
	m_Start = true;
	
	// set the drawable object we are drawing over to be invisible
	if (m_DrawableObject)
	{
		m_DrawableObject->SetVisible(false);
	}
}

void Action::Stop()
{
	m_Start = false;
	m_Completed = true;

	// set the drawable back as visible when complete
	if (m_DrawableObject)
	{
		m_DrawableObject->SetVisible(true);
	}
}

void Action::Update()
{
	m_CurrentImage->Update();
}

bool Action::Draw(double X, double Y)
{
	if (m_Visible && m_CurrentImage)
	{
		m_CurrentImage->Draw(X,Y);
	}
	return m_Visible;
}

void Action::AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames)
{
	m_CurrentImage->AddReducedTexture(RedTextureID, RedTextureIndex, AnimationSpeed, NumberFrames);
}

void Action::ClearTextures()
{
	m_Visible = false;

	m_CurrentImage->ClearTextures();
}

std::vector<std::string> Action::GetTextures()
{
	return m_CurrentImage->GetTextures();
}

std::vector<int> Action::GetTextureIndices()
{
	return m_CurrentImage->GetTextureIndices();
}

std::vector<int> Action::GetAnimationSpeeds()
{
	return m_CurrentImage->GetAnimationSpeeds();
}

std::vector<int> Action::GetNumberFrames()
{
	return m_CurrentImage->GetNumberFrames();
}
