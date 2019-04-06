#include "Component.h"

Component::Component()
{}

Component::~Component()
{}

Component::Component(GameEntity* Owner) :
	m_Owner(Owner)
{
}
