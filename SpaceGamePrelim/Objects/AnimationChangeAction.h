#pragma once
#include "Action.h"
#include "SimpleEntity.h"

class AnimationChangeAction : public Action
{
private:

	// contains a start image
	SimpleEntity* m_FirstImage;
	SimpleEntity* m_MiddleAnimation;
	SimpleEntity* m_LastImage;

public:
	AnimationChangeAction(Drawable* DrawableObject, std::string Name, double Duration, Rect Position, Scene* Parent);

	void SetInitialImage(SimpleEntity* NewImage) { m_FirstImage = NewImage; }
	void SetChangeImage(SimpleEntity* NewImage) { m_MiddleAnimation = NewImage; }
	void SetLastImage(SimpleEntity* NewImage) { m_LastImage = NewImage; }

 	virtual void Start() override;
 	virtual void Stop() override;
};

