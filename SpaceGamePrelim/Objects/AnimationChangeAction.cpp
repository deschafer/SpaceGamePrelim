#include "AnimationChangeAction.h"

AnimationChangeAction::AnimationChangeAction(Drawable* DrawableObject, std::string Name, double Duration, Rect Position) : 
	Action(DrawableObject, Name, Duration, Position)
{
}

void AnimationChangeAction::Start()
{
	// call the parent's method
	Action::Start();

	// then, since the object is invisible at this point, we get its texture(s)
}

void AnimationChangeAction::Stop()
{
}
