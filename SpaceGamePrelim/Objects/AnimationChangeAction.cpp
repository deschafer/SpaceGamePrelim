#include "AnimationChangeAction.h"

AnimationChangeAction::AnimationChangeAction(Drawable* DrawableObject, 
	std::string Name, 
	double Duration, 
	Rect Position, 
	Scene* Parent) : 
	Action(DrawableObject, Name, Duration, Position, Parent),
	m_FirstImage(nullptr),
	m_LastImage(nullptr),
	m_MiddleAnimation(nullptr)
{
	// we create a new simple entity with the dimensions and parent scene of the action
	m_FirstImage = new SimpleEntity(GetDimensions(), GetParentScene());
	// then add the drawable component to this object based on the drawable its placed over
	m_FirstImage->AddReducedTexture(DrawableObject->GetTextures()[0], DrawableObject->GetTextureIndices()[0], 1, 1);
}

void AnimationChangeAction::Start()
{
	// call the parent's method
	Action::Start();

	if (m_FirstImage && m_MiddleAnimation && m_LastImage)
	{
		// first clear the current image
		m_CurrentImage->ClearTextures();

		// then we can actually start
		// set the duration of the second simpleEntity (animation entity)
		m_MiddleAnimation->SetAnimationSpeed(GetDuration());

		std::vector<int> AnimationSpeeds = m_MiddleAnimation->GetAnimationSpeeds();
		std::vector<int> NumberFrames = m_MiddleAnimation->GetNumberFrames();
		std::vector<int> TextureIndices = m_MiddleAnimation->GetTextureIndices();
		std::vector<std::string> Textures = m_MiddleAnimation->GetTextures();

		// then change the drawn image in the action class to this image
		// for each image we have in the middle animation
		for (size_t i = 0; i < Textures.size(); i++)
		{
			m_CurrentImage->AddReducedTexture(Textures[i], TextureIndices[i], AnimationSpeeds[i], NumberFrames[i]);
		}
	}
}

void AnimationChangeAction::Stop()
{
	// stop displaying the middle image, set visibility to zero
	m_Visible = false;

	// then save the new image in the drawable object
	// start by clearing its textures
	m_DrawableObject->ClearTextures();

	// then we set the new textures
	for (int i = 0; i < m_LastImage->GetTextures().size(); i++)
	{
		m_DrawableObject->AddReducedTexture(m_LastImage->GetTextures()[i],
			m_LastImage->GetTextureIndices()[i],
			m_LastImage->GetAnimationSpeeds()[i],
			m_LastImage->GetNumberFrames()[i]);
	}

	// then delete this object
	Action::DeleteObject(this);
}
