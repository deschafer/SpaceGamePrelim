#include "MapObject.h"


MapObject::MapObject(Rect Dimensions, Scene* Parent) :
	m_ObjType(MapObjectTypes::N_A),
	Drawable(Dimensions, Parent)
{
}

MapObject::~MapObject()
{
}
