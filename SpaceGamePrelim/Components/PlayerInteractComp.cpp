
#include "PlayerInteractComp.h"
#include "..\Objects\GameEntity.h"
#include "..\Frame\InputManager.h"
#include "..\Interactions\InteractionManager.h"
#include "..\Time\Time.h"

#include <iostream>
using namespace std;

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
	if ((m_Timer += Time::Instance()->GetDeltaTime()) >= m_Time && 
		InputManager::Instance()->IsKeyDown(SDL_SCANCODE_E))
	{
		cout << "Interaction made\n";

		m_Timer = 0;

		// then we interact with surrounding objects
		InteractionManager::Instance()->InteractWithSurroundings(m_Owner);
	}
}