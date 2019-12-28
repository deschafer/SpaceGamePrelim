#pragma once

#include <vector>
#include <SDL.h>
#include <string>

#include "MapObject.h"
#include "..\TextureCode\TextureManager.h"
#include "MapCell.h"
#include "Map.h"
#include "RoomManager.h"
#include "..\Collision\MapCollision.h"
#include "MapAssetManager.h"

class GameEntity;
class MapCoordinate;

//
// Coordinate space of connected maps follows a traditonal cartesian coordinate space, pos X to the right, pos y to the top.
//

// Singleton class structure
class MapManager
{
private:
	static MapManager* m_Instance;
	
	int m_ActiveWndWidth;
	int m_ActiveWndHeight;
	int m_Rows;
	int m_Columns;
	int m_CellWidth;
	int m_CellHeight;
	double m_PixelOffsetY;
	double m_PixelOffsetX;
	int m_ActivelyLinkedCount;
	bool m_MovementLeft;
	bool m_MovementRight;
	bool m_MovementNorth;
	bool m_MovementSouth;
	bool m_Init;
	bool m_MapNeedsSwapping;
	bool m_MapsAreGenerating;
	bool m_HorizMovementSwapped;
	bool m_VertiMovementSwapped;
	bool m_MapPhysicallyLinked;
	bool m_NoMovementFlag;
	bool m_NoMovementXFlag;
	bool m_NoMovementYFlag;
	MapObject*** m_VisibleObjectArray;	// 2D array of pointers to mapobjects visible
	MapAsset*** m_VisibleAssetArray;	// 2D array of pointers to visible assets that get drawn on top of the base map
	Map* m_ActiveMap;					// Active map pointer
	RoomManager* m_RoomManager;
	std::vector<Map*> m_LoadedMaps;
	std::map<std::pair<int, int>, Map*> m_CoordinateMaps;
	std::vector<Map*> m_GeneratingMaps;	// Holds maps that need to be linked in the coordinate map structure
	std::vector <std::pair<MapDirection, bool>> m_SwappingMovement;	// Which direction to map needs to move towards when swapping
	std::vector<MapObject****> m_VisibleMapCells;
	std::vector<MapAsset****> m_VisibleAssetCells;
	std::list<std::pair<Map*, Map*>> m_QueuedEastConnections;
	std::list<std::pair<Map*, Map*>> m_QueuedNorthConnections;
	Vector m_RequestedMovement;
	MapDirection m_RequestedHorizMovement;
	MapDirection m_RequestedVertiMovement;
	MapAssetManager* m_AssetManager;
	MapFactory* m_MapFactory;

	MapCell* GetCellFromMaps(Map* CurrentMap, MapCoordinate RequestedCell);
	void SetLink(Map* NewMap);
	void GenerateNeighbors();
	void GenerateNeighbor(std::string MapType, MapDirection ActiveMapDir, MapDirection NewMapDir, MapCoordinate NewMapCoord);
	void HandleInput();
	void HandleExternalMovement();
	void CheckOffsets();
	void CheckGeneratingMaps();
	void CullMap();
	void MoveMap();
	void UpdateCells();
	void Zoom();
	void CheckPhysicalConnections();
	Collision* CheckCellForCollision(
		Vector Position,
		MapCollisionDir
		Direction,
		MapDirection SpecDirection,
		Vector Movement,
		Rect ObjectDimensions,
		GameEntity* Entity);

	void ComputeStartingOffsets();

	MapManager();

public:

	static const int CellWidthSrc = 64;							// Source for entire project's cell sizes
	static const int CellHeightSrc = 64;
	static const int MapSizeW = 100;							// Width of the Map objects
	static const int MapManager::MapSizeH = 100;							// Height of the Map objects
	static const int MapWidthPixels = CellWidthSrc * MapSizeW;
	static const int MapHeightPixels = CellHeightSrc * MapSizeH;
	static const int HorizontalMovementSpeed = 8;
	static const int VerticalMovementSpeed = 8;
	static const int VisibleHorizonBufferSize = 2;
	static const int VisibleVerticalBufferSize = 2;
	static const int CenterMapArrayIndex = 8;
	static const int ColumnWidth1 = 6;							// Different possible widths for the defined columns of a map
	static const int ColumnWidth2 = 8;
	static const int ColumnWidth3 = 10;
	static const int ColumnWidth4 = 12;
	static const int ColumnWidth5 = 14;
	static const int NumberColumns = MapSizeW / ColumnWidth5;
	static const int ActiveCellsWidth = 1000;
	static const int ActiveCellsHeight = 1000;
	static const int InitialMapID = 0;

	typedef std::pair<int, int> Coord;

	static MapManager* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new MapManager;
			return m_Instance;
		}
		else return m_Instance;
	}

	static int GetCellSourceWidth();
	static int GetCellSourceHeight();
	static int GetCellWidth() {
		if (!m_Instance) 
		{
			return CellWidthSrc;
		}
		else
		{	
			return m_Instance->m_CellWidth;
		}
	}
	static int GetCellHeight() {
		if (!m_Instance)
		{
			return CellHeightSrc;
		}
		else
		{
			return m_Instance->m_CellHeight;
		}
	}

	void HandleMapZoom();
	void DrawGrid();
	void Draw();
	void Update();
	void OffsetMap(Vector PixelOffsets) { m_RequestedMovement = PixelOffsets; }
	void CorrectZoom(Vector PixelOffsets);
	void ConnectTwoMaps(Map* Map1, Map* Map2, MapDirection LinkBetween);
	void RemoveQueuedMap();
	void ResetMap();
	Cell GetCellType(Vector ScreenPosition);
	MapCoordinate GetCellIndex(Vector ScreenPosition, Map* &MapWithCell);
	MapCoordinate GetCellIndex(Vector ScreenPosition, Map* &MapWithCell, Vector &CellTopLeft, Vector &OriginPos);
	std::vector<Collision*> CheckCollisions(Vector PosWithMovement, Vector PosWithoutMovement, GameEntity* Object);
	Collision* CheckCollidingPoint(Vector Position);
	Vector ConvertMapPositionToScreenPosition(MapCoordinate Position, Map* CurrentMap);
	~MapManager();
};