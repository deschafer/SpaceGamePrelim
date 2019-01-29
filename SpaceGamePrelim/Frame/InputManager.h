#pragma once

#include <SDL.h>
#include <vector>

#include "..\BasicTypes\BasicTypes.h"

enum class MouseInput { LEFT_MB, MIDDLE_MB, RIGHT_MB};

// Singleton class structure
class InputManager
{
private:
	InputManager();

	static InputManager* m_Instance;

	// handle keyboard events
	void OnKeyDown();
	void OnKeyUp();

	// handle mouse events
	void OnMouseMove(SDL_Event& event);
	void OnMouseButtonDown(SDL_Event& event);
	void OnMouseButtonUp(SDL_Event& event);

	// Event containers
	std::vector<bool> m_MosueButtonStates;
	Vector* m_MousePosition;
	const Uint8* m_KeyStates;

public:
	
	static InputManager* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new InputManager;
			return m_Instance;
		}
		else return m_Instance;
	}

	// Potential future gamepad support, not at the moment however

	Vector* GetCurrentMousePosition() { return m_MousePosition; }
	bool GetMouseButtonState(MouseInput MouseButton) { return m_MosueButtonStates[static_cast<int>(MouseButton)]; }
	bool IsKeyDown(SDL_Scancode Key);

	void HandleEvents();
	void Clean();
	void Reset();

	~InputManager();
};

