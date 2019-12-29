
#include "Component.h"

class GameEntity;

class PlayerInteractComp : public Component
{
private:

	double m_Timer = 0;
	const double m_Time = 0.25;	// one quarter second

public:
	PlayerInteractComp();
	PlayerInteractComp(GameEntity* Owner);

	virtual void Execute();

	virtual ~PlayerInteractComp() {}
};

class PlayerInteractCompCreator : public ComponentCreator
{
public:
	PlayerInteractCompCreator() {}
	~PlayerInteractCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new PlayerInteractComp(Owner);
	}
};