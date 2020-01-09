
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

	Door() : Door(Rect(-1,-1,-1,-1), nullptr) {}

	static const std::string m_DoorOpenStr;
	static const std::string m_DoorClosedStr;
	static const std::string m_DoorClosingStr; 
	static const std::string m_DoorOpeningStr;

protected:
	Door(Door const&) = default;
	Door& operator=(Door const&) = default;

public:
	Door(Rect Dimensions, Scene* Parent);
	~Door();

	virtual MapAsset* Copy();	// Returns a copy of this object
	virtual bool OnInteraction(GameEntity* Entity);
};

