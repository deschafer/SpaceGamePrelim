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

class GameEntity;

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
	int m_VisibleRows;
	int m_VisibleColumns;
	int m_Rows;
	int m_Columns;
	int m_CellWidth;
	int m_CellHeight;
	int m_OffsetX;		
	int m_OffsetY;		
	int m_PixelOffsetY;
	int m_PixelOffsetX;
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
	MapObject*** m_VisibleObjectArray;	// 2D array of pointers to mapobjects visible
	Map* m_ActiveMap;					// Active map pointer
	RoomManager* m_RoomManager;
	std::vector<Map*> m_LoadedMaps;
	std::map<std::pair<int, int>, Map*> m_CoordinateMaps;
	std::vector<Map*> m_GeneratingMaps;	// Holds maps that need to be linked in the coordinate map structure
	std::vector <std::pair<MapDirection, bool>> m_SwappingMovement;	// Which direction to map needs to move towards when swapping
	std::vector<MapObject****> m_VisibleMapCells;
	std::list<std::pair<Map*, Map*>> m_QueuedEastConnections;
	std::list<std::pair<Map*, Map*>> m_QueuedNorthConnections;
	Vector m_RequestedMovement;
	MapDirection m_RequestedHorizMovement;
	MapDirection m_RequestedVertiMovement;

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
	void CheckPhysicalConnections();
	Collision* CheckCellForCollision(Vector Position, MapCollisionDir Direction);
	Collision* CheckCellForCollision(Vector Position, MapCollisionDir Direction, MapDirection SpecDirection, Vector Movement, Rect ObjectDimensions);

	MapManager();

public:

	static MapManager* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new MapManager;
			return m_Instance;
		} 
		else return m_Instance;
	}

	void DrawGrid();
	void Draw();
	void Update();

	int GetCellWidth() { return m_CellWidth; }
	int GetCellHeight() { return m_CellHeight; }
	Cell GetCellType(Vector ScreenPosition);
	MapCoordinate GetCellIndex(Vector ScreenPosition, Map* &MapWithCell);
	MapCoordinate GetCellIndex(Vector ScreenPosition, Map* &MapWithCell, Vector &CellTopLeft, Vector &OriginPos);
	std::vector<Collision*> CheckCollisions(Vector PosWithMovement, Vector PosWithoutMovement, GameEntity* Object);
	void OffsetMap(Vector PixelOffsets) { m_RequestedMovement = PixelOffsets; }

	void ConnectTwoMaps(Map* Map1, Map* Map2, MapDirection LinkBetween);
	void RemoveQueuedMap();
	void ResetMap();
	
	~MapManager();
};

