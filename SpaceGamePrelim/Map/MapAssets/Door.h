
#include "MapAsset.h"

class Door : public MapAsset
{
public:
	Door();
	~Door();

	virtual MapAsset* Copy();	// Returns a copy of this object
	virtual bool OnInteraction(GameEntity* Entity);
};

