
#include "PlayerInteractComp.h"
#include "..\Objects\GameEntity.h"
#include "..\Frame\InputManager.h"
#include "..\Interactions\InteractionManager.h"


PlayerInteractComp::PlayerInteractComp()
{
}

PlayerInteractComp::PlayerInteractComp(GameEntity* Owner) :
	Component(Owner)
{
}

void PlayerInteractComp::Execute() {

	// this checks for user input 
	// and if there is user input, it checks all surrounding cells and
	// interacts with all of thems
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_E))
	{
		// then we interact with surrounding objects
		InteractionManager::Instance()->InteractWithSurroundings(m_Owner);
	}
}