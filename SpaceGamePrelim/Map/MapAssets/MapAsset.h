#pragma once
#include "..\MapCell.h"


class MapAsset : public MapCell
{

private:

	bool m_Collidable;

public:

	void Load(std::vector<std::string> RedTextureIDs, // Function used to create this object
		MapCoordinate Position,
		Rect DestRect,
		Cell CellType,
		std::vector<int> NumberFrames,
		std::vector<int> AnimationSpeed);
	void Load(std::vector<std::string> RedTextureIDs, // Function used to create this object
		MapCoordinate Position,
		Rect DestRect,
		Cell CellType);

	void Load(std::vector<std::string> RedTextureIDs, // Function used to create this object
		MapCoordinate Position,
		Rect DestRect,
		Cell CellType,
		std::vector<int> NumberFrames,
		std::vector<int> AnimationSpeed,
		bool Collidable);
	void Load(std::vector<std::string> RedTextureIDs, // Function used to create this object
		MapCoordinate Position,
		Rect DestRect,
		Cell CellType,
		bool Collidable);


	virtual MapAsset* Copy() = 0;	// Returns a copy of this object
	virtual void OnInteraction(GameEntity* Entity) = 0;
	virtual void Zoom() override;
	virtual bool IsCollidableType() override { return m_Collidable; }

	MapAsset();
	MapAsset(std::vector<std::string> RedTextureIDs,
		MapCoordinate Position,
		Rect SrcRect,
		Cell CellType);

	virtual ~MapAsset();
};

