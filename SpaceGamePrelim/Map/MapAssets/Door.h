
#include "MapAsset.h"
#include "../../Objects/SimpleEntity.h"
#include "..\..\BasicTypes\BasicTypes.h"
#include "..\..\Scene\Scene.h"
#include "..\..\Objects\AnimationChangeAction.h"

class Door : public MapAsset
{
private:

	bool m_Open = false;

	SimpleEntity* m_DoorOpen;
	SimpleEntity* m_DoorClosed;
	SimpleEntity* m_DoorClosing;
	SimpleEntity* m_DoorOpening;

	const std::string m_DoorOpenStr = "Door_Open";
	const std::string m_DoorClosedStr = "Door";
	const std::string m_DoorClosingStr = "Door_Closing";
	const std::string m_DoorOpeningStr = "Door_Opening";

	AnimationChangeAction *m_CurrentAction = nullptr;

public:
	Door() : Door(Rect (0,0,0,0), nullptr) {}
	Door(Rect Dimensions, Scene* Parent);
	~Door();

	virtual MapAsset* Copy();	// Returns a copy of this object
	virtual bool OnInteraction(GameEntity* Entity);
};

