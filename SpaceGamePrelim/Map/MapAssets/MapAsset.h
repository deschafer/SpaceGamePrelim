#pragma once
#include "..\MapCell.h"

class MapRoom;
class Map;

class MapAsset : public MapCell, Interactable
{

private:

	bool m_Collidable;
	bool m_Updated;
	bool m_Drawn;
	int m_IntegerWidth;
	int m_IntegerHeight;
	MapRoom* m_ParentRoom;

	virtual MapCoordinate CheckAssetPosition(MapCoordinate TopLeftPosition, MapCell*** Cells, MapAsset*** Assets);

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

	void SetParentRoom(MapRoom* Room) { m_ParentRoom = Room; }
	MapRoom* GetParentRoom() { return m_ParentRoom; }
	Map* GetParentMap();
	bool IsDrawn() { return m_Drawn; }

	virtual MapAsset* Copy() = 0;	// Returns a copy of this object
	virtual void Zoom() override;
	virtual bool IsCollidableType() override { return m_Collidable; }
	virtual int GetIntegerWidth();
	virtual int GetIntegerHeight();
	virtual MapCoordinate PlaceAsset(MapObject*** RoomCells, MapAsset*** RoomAssets, bool*** Doorways);
	virtual MapCoordinate PlaceAssetBorderingRoom(MapCell*** RoomCells, MapAsset*** RoomAssets);
	virtual void Draw(MapCoordinate Coords) override;
	virtual void Draw(double X, double Y) override;
	virtual void Update() override;
	virtual void Reset();	// should be called by derived classes when this object is copied

	MapAsset();
	MapAsset(std::vector<std::string> RedTextureIDs,
		MapCoordinate Position,
		Rect SrcRect,
		Cell CellType,
		MapRoom* Parent);
	virtual ~MapAsset();

	virtual Vector GetInteractablePosition() override;
};

