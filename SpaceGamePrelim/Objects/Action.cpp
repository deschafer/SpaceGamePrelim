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
Action::Action(Drawable* DrawableObject, std::string Name, double Duration, Rect Position) :
	m_DrawableObject(DrawableObject),
	m_Name(Name),
	m_Duration(Duration),
	m_Dimensions(Position),
	m_Start(false),
	m_CurrentFrame(0),
	m_CurrentRow(0),
	m_DrawnRectTextureIndex(-1),
	m_DrawnRedTextureID("Empty"),
	m_DrawnAnimationSpeed(0),
	m_DrawnNumberFrames(0),
	Drawable(Position)
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

	// set the drawable back as visible when complete
	if (m_DrawableObject)
	{
		m_DrawableObject->SetVisible(true);
	}
}

void Action::Update()
{
	m_CurrentFrame = int(((SDL_GetTicks() / m_DrawnAnimationSpeed) % m_DrawnNumberFrames));
}

bool Action::Draw(double X, double Y)
{
	if (m_Visible && m_DrawnRectTextureIndex >= 0)
	{
		SDL_Renderer* Temp = MainApplication::Instance()->GetRenderer();

		TextureManager::Instance()->DrawCurrentFrame(
			m_Dimensions.TopLeftX(),
			m_Dimensions.TopLeftX(),
			m_DrawnRectTextureIndex,
			SDL_FLIP_NONE,
			MainApplication::Instance()->GetRenderer(),
			m_Dimensions,
			GetColor(),
			m_CurrentRow,
			m_CurrentFrame);
	}
	return m_Visible;
}

void Action::AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames)
{
	// this removes our current texture, and replaces it with a new one
	m_DrawnRedTextureID = RedTextureID;
	m_DrawnRectTextureIndex = RedTextureIndex;
	m_DrawnAnimationSpeed = AnimationSpeed;
	m_DrawnNumberFrames = NumberFrames;

	m_CurrentFrame = 0;
	m_CurrentRow = 0;
}

void Action::ClearTextures()
{
	m_Visible = false;

	// then proceeds to remove the textures
}

std::vector<std::string> Action::GetTextures()
{
	return std::vector<std::string>();
}

std::vector<int> Action::GetTextureIndices()
{
	return std::vector<int>();
}
