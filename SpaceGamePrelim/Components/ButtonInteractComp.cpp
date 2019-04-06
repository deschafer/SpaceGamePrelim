#include "ButtonInteractComp.h"
#include "../Frame/InputManager.h"


ButtonInteractComp::ButtonInteractComp()
{
}

ButtonInteractComp::ButtonInteractComp(GameEntity* Owner) :
	Component(Owner),
	m_Clicked(false),
	m_Released(true)
{
}


ButtonInteractComp::~ButtonInteractComp()
{
}

void ButtonInteractComp::Execute()
{
	Vector Position = m_Owner->GetPosition();
	Rect Dimensions = m_Owner->GetDimensions();
	int CurrentFrame = 0;

	Vector* CurrentMousePosition =
		InputManager::Instance()->GetCurrentMousePosition();

	if (CurrentMousePosition->getX() < (Position.getX() + Dimensions.Width())
		&& CurrentMousePosition->getX() > Position.getX()
		&& CurrentMousePosition->getY() < (Position.getY() + Dimensions.Height())
		&& CurrentMousePosition->getY() > Position.getY())
	{
		if (InputManager::Instance()->GetMouseButtonState(MouseInput::LEFT_MB)
			&& m_Released)
		{
			CurrentFrame = MOUSE_CLICKED;

			m_Clicked = true;

		}
		else if (m_Clicked)
		{
			m_Owner->ExecuteCallback();
			m_Released = false;
			m_Clicked = false;

		}
		else if (!InputManager::Instance()
			->GetMouseButtonState(MouseInput::LEFT_MB))
		{
			m_Released = true;
			CurrentFrame = MOUSE_OVER;
			m_Clicked = false;
		}
		else
		{
			m_Released = false;
			m_Clicked = false;
		}
	}
	else
	{
		CurrentFrame = MOUSE_OUT;
		m_Released = false;
		m_Clicked = false;
	}

	m_Owner->SetFrame(CurrentFrame);
}