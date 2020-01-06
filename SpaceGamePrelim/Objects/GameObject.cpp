#include "GameObject.h"



GameObject::GameObject(Scene* ParentScene) : 
	Drawable(Rect(0, 0, 0, 0), ParentScene)
{
}


GameObject::~GameObject()
{
}
