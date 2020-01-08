#pragma once

#include <string>
#include <vector>

#include "..\BasicTypes\BasicTypes.h"
#include "Drawable.h"
#include "SimpleEntity.h"

class Action : public Drawable
{
private:
	bool m_Start;
	bool m_Completed = false;
	std::string m_Name;
	double m_Duration;
	Rect m_Dimensions;

protected:
	
	static void DeleteObject(Action* Object) { delete Object; }

	Drawable* m_DrawableObject;
	
	// drawn texture data
	// NOTE: this object draws a single texture every frame
	// it is manipulated by modifying the data of this texture by derived classes.
	SimpleEntity* m_CurrentImage;

public:

	Action(Drawable* DrawableObject, std::string Name, double Duration, Rect Dimensions, Scene* Parent);

	std::string GetName() { return m_Name; }


	// this is implementation dependent
	virtual void Start();
	virtual void Stop();
	virtual bool IsCompleted() { return m_Completed; }
	virtual bool IsStarted() { return m_Start; }

	virtual void Update();

	// drawable method overrides
	virtual bool Draw(double X, double Y) override;
	virtual void AddReducedTexture(std::string RedTextureID, int RedTextIndex, int AnimationSpeed, int NumberFrames) override;
	virtual void ClearTextures() override;
	virtual std::vector<std::string> GetTextures() override;
	virtual std::vector<int> GetTextureIndices() override;
	virtual std::vector<int> GetAnimationSpeeds() override;
	virtual std::vector<int> GetNumberFrames() override;

	int GetDuration() { return m_Duration; }

};

