
#include "Component.h"

class GameEntity;

class PlayerInteractComp : public Component
{
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