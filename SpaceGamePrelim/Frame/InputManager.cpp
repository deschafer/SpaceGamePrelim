
#include "InputManager.h"
#include "MainApplication.h"


InputManager* InputManager::m_Instance = nullptr;


InputManager::InputManager() : 
	m_MousePosition(new Vector()),
	m_MouseWheelMovement(0)
{

	for (int i = 0; i < 3; i++) m_MosueButtonStates.push_back(false);

}

InputManager::~InputManager() 
{
}


void InputManager::OnKeyDown()
{
	m_KeyStates = SDL_GetKeyboardState(0);
}

void InputManager::OnKeyUp()
{
	m_KeyStates = SDL_GetKeyboardState(0);
}

void InputManager::OnMouseMove(SDL_Event& Event)
{

	m_MousePosition->setX((float)Event.motion.x);
	m_MousePosition->setY((float)Event.motion.y);

}

void InputManager::OnMouseButtonDown(SDL_Event& event)
{
	switch (event.button.button)
	{
	case SDL_BUTTON_LEFT:
		m_MosueButtonStates[static_cast<int>(MouseInput::LEFT_MB)] = true;
		break;
	case SDL_BUTTON_MIDDLE:
		m_MosueButtonStates[static_cast<int>(MouseInput::MIDDLE_MB)] = true;
		break;
	case SDL_BUTTON_RIGHT:
		m_MosueButtonStates[static_cast<int>(MouseInput::RIGHT_MB)] = true;
		break;
	default: break;
	}
}

void InputManager::OnMouseButtonUp(SDL_Event& event)
{
	switch (event.button.button)
	{
	case SDL_BUTTON_LEFT:
		m_MosueButtonStates[static_cast<int>(MouseInput::LEFT_MB)] = false;
		break;
	case SDL_BUTTON_MIDDLE:
		m_MosueButtonStates[static_cast<int>(MouseInput::MIDDLE_MB)] = false;
		break;
	case SDL_BUTTON_RIGHT:
		m_MosueButtonStates[static_cast<int>(MouseInput::RIGHT_MB)] = false;
		break;
	default: break;
	}
}

bool InputManager::IsKeyDown(SDL_Scancode Key)
{
	if (m_KeyStates != nullptr)
	{
		if ((bool)m_KeyStates[Key] == true)
		{
			return true;
		}
		else return false;
	}
	return false;
}

void InputManager::OnMouseWheel(SDL_Event& Event)
{
	m_MouseWheelMovement = (int)Event.wheel.y;
	std::cout << "move " << m_MouseWheelMovement << std::endl;

}

void InputManager::HandleEvents()
{
	Reset(); // Reset events that are change based, like the mouse wheel

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			MainApplication::Instance()->Terminate();
			break;

		case SDL_MOUSEMOTION:
			OnMouseMove(event);
			break;

		case SDL_MOUSEBUTTONDOWN:
			OnMouseButtonDown(event);
			break;

		case SDL_MOUSEBUTTONUP:
			OnMouseButtonUp(event);
			break;

		case SDL_KEYDOWN:
			OnKeyDown();
			break;

		case SDL_KEYUP:
			OnKeyUp();
			break;

		case SDL_MOUSEWHEEL:
			OnMouseWheel(event);
			break;

		default:
			break;
		}
	}
}
void InputManager::Clean()
{
}
void InputManager::Reset()
{
	m_MouseWheelMovement = 0;
}