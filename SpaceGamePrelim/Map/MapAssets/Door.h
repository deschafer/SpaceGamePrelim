
#include "MapAsset.h"
#include "../../Objects/SimpleEntity.h"
#include "..\..\BasicTypes\BasicTypes.h"
#include "..\..\Scene\Scene.h"

class Door : public MapAsset
{
private:

	bool m_Open = false;

	SimpleEntity* m_DoorOpen;
	SimpleEntity* m_DoorClosed;
	SimpleEntity* m_DoorClosing;
	SimpleEntity* m_DoorOpening;

public:
	Door() = delete;
	Door(Rect Dimensions, Scene* Parent);
	~Door();

	virtual MapAsset* Copy();	// Returns a copy of this object
	virtual bool OnInteraction(GameEntity* Entity);
};

