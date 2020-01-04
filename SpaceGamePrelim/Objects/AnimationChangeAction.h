#pragma once
#include "Action.h"

class AnimationChangeAction : public Action
{
private:

	// contains a start image
	// contains a middle animation for change
	// then contains a last image that will be used to ovewrite the drawable's texture

public:
	AnimationChangeAction(Drawable* DrawableObject, std::string Name, double Duration, Rect Position);

 	virtual void Start() override;
 	virtual void Stop() override;
};

