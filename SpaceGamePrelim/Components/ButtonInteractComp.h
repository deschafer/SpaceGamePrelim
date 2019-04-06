#pragma once
#include "Component.h"


class ButtonInteractComp : public Component
{
private:

	enum States { MOUSE_OUT, MOUSE_OVER, MOUSE_CLICKED };

	bool m_Released;
	bool m_Clicked;

public:

	virtual void Execute();

	ButtonInteractComp();
	ButtonInteractComp(GameEntity* Owner);

	virtual ~ButtonInteractComp();
};

class ButtonInteractCompCreator : public ComponentCreator
{
public:
	ButtonInteractCompCreator() {}
	~ButtonInteractCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new ButtonInteractComp(Owner);
	}
};

