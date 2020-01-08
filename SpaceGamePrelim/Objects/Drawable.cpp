#include "Drawable.h"
#include "..\Scene\Scene.h"
#include "Action.h"

Drawable::Drawable(Rect Dimensions, Scene* ParentScene) :
	m_Dimensions(Dimensions),
	m_Visible(true),
	m_ParentScene(ParentScene),
	Locatable(Vector((float)Dimensions.TopLeftX(), (float)Dimensions.TopLeftY()))
{
	if (ParentScene)
		ParentScene->AddDrawable(this);
}

Drawable::~Drawable() 
{
	if (m_ParentScene)
		m_ParentScene->RemoveDrawable(this);
}

bool Drawable::Draw(double X, double Y)
{
	Action* RemovedAction = nullptr;

	for (Action* CurrAction : m_Actions)
	{
		if (CurrAction && CurrAction->IsStarted())
		{
			continue;
		}
		else if (CurrAction->IsCompleted())
		{
			RemovedAction = CurrAction;
		}
	}
	
	if (RemovedAction)
	{
		m_Actions.remove(RemovedAction);
		delete RemovedAction;
	}

	return m_Visible;
}

void Drawable::SetParentScene(Scene* ParentScene)
{
	if (m_ParentScene)
	{
		// then we need to remove from the last
		m_ParentScene->RemoveDrawable(this);
	}

	// and then always add to the new scene
	(m_ParentScene = ParentScene)->AddDrawable(this);
}
