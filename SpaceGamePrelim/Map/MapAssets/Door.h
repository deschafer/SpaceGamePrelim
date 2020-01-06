
#include "MapAsset.h"
#include "../../Objects/SimpleEntity.h"

class Door : public MapAsset
{
private:

	bool m_Open = false;

	SimpleEntity* m_DoorOpen;
	SimpleEntity* m_DoorClosed;
	SimpleEntity* m_DoorClosing;
	SimpleEntity* m_DoorOpening;

public:
	Door();
	~Door();

	virtual MapAsset* Copy();	// Returns a copy of this object
	virtual bool OnInteraction(GameEntity* Entity);
};

