
#include "Map.h"
#include "MapRoom.h"
#include "MapWall.h"
#include "MapInactive.h"
#include "GenRoomComp.h"
#include "RoomConnector.h"
#include "CorridorVertical.h"
#include "CorridorHoriz.h"


#include "..\Frame\MainApplication.h"

#include <chrono>

//#define DEBUG_CORRIDOR_VERTICAL
//#define DEBUG_CORRIDOR_HORIZ

static const int WidthRandomRate = 5;
static const int HeightRandomRate = 5;

static const int MaxRoomHeight = 20;
static const int MaxRoomWidth = 14;
static const int MinRoomWidth = 6;
static const int RoomWidthDiff = 8;
static const int RoomSizeVolatility = 5;

static const int ColumnWidth1 = 6;
static const int ColumnWidth2 = 8;
static const int ColumnWidth3 = 10;
static const int ColumnWidth4 = 12;
static const int ColumnWidth5 = 14;
static const int ColumnSeparatorMax = 3;
static const int ColumnSeparatorMin = 1;
static const int ComparisonSideWidth = 6;
static const int NeighMapCount = 8;

using namespace std;
using namespace std::chrono;

const static string WallCornerRight = "Wall_Corner_Right";
const static string WallCornerLeft = "Wall_Corner_Left";
const static string WallSideRight = "Wall_Side_Right";
const static string WallSideLeft = "Wall_Side_Left";
const static string WallBottom = "Wall_Bottom";
const static string Default = "Wall";
const static string WallTopGroup = "Wall_Top";
const static string FloorGroup = "Floors";



Map::Map()
{
}

//
// Map()
// Constructor to be used with this object
//
Map::Map(string MapType, int Width, int Height, MapCoordinate Coords) :
	m_Width(Width),
	m_Height(Height),
	m_MapType(MapType),
	m_MapCoordinates(Coords),
	m_NeighborMapsSize(NeighMapCount),
	m_Generated(false),
	m_PhysicallyLinkedEast(false),
	m_PhysicallyLinkedNorth(false),
	m_PhysicallyLinkedSouth(false),
	m_PhysicallyLinkedWest(false)
{
	// Generating the array for this map
	m_Cells = new MapObject**[m_Height];
	m_CorridorCells = new MapObject**[m_Height];
	for (int i = 0; i < m_Height; i++)
	{
		m_Cells[i] = new MapObject*[m_Width];
		m_CorridorCells[i] = new MapObject*[m_Width];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Height; i++)
	{
		for (int j = 0; j < m_Width; j++)
		{
			m_Cells[i][j] = nullptr;
			m_CorridorCells[i][j] = nullptr;
		}
	}

	for (size_t i = 0; i < NeighMapCount; i++)
	{
		m_NeighboringMaps[i] = nullptr;
	}
}

Map::~Map()
{
}


//
// GetCell()
// Gets the cell from m_Cells specefied by position
// X,Y. If not within the bounds of m_Cells,
// it will return nullptr, so check the return.
//
MapObject* Map::GetCell(int X, int Y)
{
	if (X < m_Width && X >= 0 && Y < m_Height && Y >= 0)
	{
		return m_Cells[X][Y];
	}
	else
	{
		return nullptr;
	}
}

MapObject* Map::GetCorridorCell(int X, int Y)
{
	if (X < m_Width && X >= 0 && Y < m_Height && Y >= 0)
	{
		return m_CorridorCells[X][Y];
	}
	else
	{
		return nullptr;
	}
}

//
// Generate()
// Generates this map objects -- may take longer 
// as this is a one-time process
//
void Map::Generate()
{
	int Width = 0;
	int Index = 0;

#ifdef _DEBUG
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif // DEBUG

	// Generating each column of rooms
	while (Width + MaxRoomWidth < m_Width)
	{
		m_Rooms.push_back(vector<MapRoom*>());	// Adds a new vector to store the rooms in this col.
		m_ColumnOffsetsX.push_back(vector<int>());
		m_ColumnOffsetsY.push_back(vector<int>());
		GenerateRoom(Width, 0, MaxRoomWidth, Index);
		Width += MaxRoomWidth;
		Index++;
	}

	// Set this map as generated
	m_Generated = true;
	
#ifdef _DEBUG
	// Preformance based -- not in final version
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	cout << duration << endl;
#endif // _DEBUG
}

//
// GenerateRoom()
// Recursive function repsonsible for generating a single room within its boundaries
// Generate rooms column-wise, so it only moves downwards.
//
void Map::GenerateRoom(int OffsetX, int OffsetY, int MaxWidth, int ColNumber)
{			
	RoomProperties* Properties;
	string RoomType;
	int RoomWidth = MinRoomWidth + rand() % RoomWidthDiff;
	int RoomHeight = 0;
	MapRoom* Room;
	MapRoom* RoomAbove = (m_Rooms[ColNumber].size()) ? m_Rooms[ColNumber][m_Rooms[ColNumber].size() - 1] : nullptr;
	int xOffset;

	// We need to get a room height, first check if there is a boundary within 20 cells
	for (size_t i = OffsetY, Count = 0; i < (size_t)(m_Height - 1) && Count < (size_t)MaxRoomHeight; i++, Count++)
	{
		RoomHeight++;
	}

	// Checking the vertical space to fit this room
	// within this map
	if (RoomHeight != MaxRoomHeight)
	{
		Properties = RoomManager::Instance()->GetRandomTypeThatFits(RoomType, RoomWidth, RoomHeight);

		if (Properties == nullptr)
		{
			// Do not generate a room
			return;
		}
		// Adding random variation to the size
		if(RoomHeight > 8) RoomHeight -= rand() % RoomSizeVolatility;
	}
	else
	{
		Properties = RoomManager::Instance()->GetRandomTypeThatFits(RoomType, RoomWidth, MaxRoomHeight);
		
		if (Properties == nullptr)
		{
		#ifdef _DEBUG
			cout << "No Room received for " << RoomType << endl;
		#endif // _DEBUG
			return;
		}
		else
		{
			RoomHeight = Properties->m_MinHeight + rand() % 5;
		}
	}

	// Creating the new room
	Room = new MapRoom(RoomType, RoomWidth, RoomHeight);
	Room->Generate();

	// Adding a offset within the column so all the rooms
	// do not start at the same x position
	if (RoomWidth < MaxRoomWidth)
	{
		int Difference = MaxRoomWidth - RoomWidth;

		xOffset = rand() % Difference;
	}

	// Add new rooms to the storage vector
	m_Rooms[ColNumber].push_back(Room);
	// Adding this rooms column offset to simplify corridor gen later on
	m_ColumnOffsetsX[ColNumber].push_back(xOffset);
	m_ColumnOffsetsY[ColNumber].push_back(OffsetY);

	MapCell* Place = nullptr;
	// Now since the space has been allocated, and the room has been 
	// generated, let's place the object in the array
	for (size_t IndexX = OffsetX + xOffset, MagX = 0;
		MagX < (size_t)RoomWidth;
		MagX++, IndexX++)
	{
		for (size_t IndexY = OffsetY, MagY = 0;
			MagY < (size_t)RoomHeight;
			MagY++, IndexY++)
		{
			Place = (MapCell*)m_Cells[IndexX][IndexY];
			if (!Place)
			{
				m_Cells[IndexX][IndexY] = Room->GetCell(MagX, MagY);
			}
			else
			{
				vector<string>* Textures = Place->ReturnRedTextures();
				m_Cells[IndexX][IndexY] = Room->GetCell(MagX, MagY);

				MapCell* ChangeCell = dynamic_cast<MapCell*>(m_Cells[IndexX][IndexY]);
				if (ChangeCell)
				{
					vector<string> OtherTextures = *ChangeCell->ReturnRedTextures();

					ChangeCell->ChangeRedTextures(*Textures);
					for (size_t i = 0; i < OtherTextures.size(); i++)
					{
						ChangeCell->AddRedTexture((OtherTextures)[i]);
					}
					delete Place;
				}
				else m_Cells[IndexX][IndexY] = Place;
			}
		}
	}

	// Setting appropriate link to room above
	if (RoomAbove)
	{
		RoomAbove->AddLinkedRoom(Side::BOTTOM, Room);	// Link from above room to this room
		Room->AddLinkedRoom(Side::TOP, RoomAbove);	// Link from this room to the room above
	}

	SetUpVertiCorridor(ColNumber, OffsetX, OffsetY, xOffset, Room);
	SetUpHorizCorridor(ColNumber - 1, OffsetX, OffsetY, xOffset, Room);

	// Generating the next room below this one
	GenerateRoom(OffsetX, OffsetY + RoomHeight + rand() % ColumnSeparatorMax + ColumnSeparatorMin, MaxWidth, ColNumber);
}

//
// SetUpCorridor()
// Finds suitable starting cells for the corridor to be created.
//
void Map::SetUpVertiCorridor(int ColumnNumber, int ColumnOffsetX, int OffsetY, int RoomOffsetX, MapRoom* BottomRoom)
{
	int Size = 0;
	int Index = m_Rooms[ColumnNumber].size() - 1;

	// Generate a path to above room if possible
	if ((Size = Index - 1) >= 0)
	{
		// There is a room above us, so we need to find a suitable location for a corridor.
		MapRoom* RoomAbove = m_Rooms[ColumnNumber][Size];
		MapCoordinate CurrLocation(ColumnOffsetX + RoomOffsetX, OffsetY);
		MapCoordinate AboveLocation(m_ColumnOffsetsX[ColumnNumber][Size] + (ColumnNumber * ColumnWidth5), m_ColumnOffsetsY[ColumnNumber][Size]);

		RoomConnector Connector(RoomAbove, AboveLocation, BottomRoom, CurrLocation);
		DeterminedRoomPositions* Positions = Connector.FindRoomPositionsVerti();

		if (Positions)
		{
			int Distance = abs(AboveLocation.GetPositionY() + RoomAbove->GetHeight() - CurrLocation.GetPositionY());
			if (!Distance) 
				Distance = 1;

			// Translating back into map coordinates
			AboveLocation = MapCoordinate(
				Positions->BaseOffset.GetPositionX() + AboveLocation.GetPositionX(), 
				Positions->BaseOffset.GetPositionY() + AboveLocation.GetPositionY());
			CurrLocation = MapCoordinate(
				Positions->OtherPosOffset.GetPositionX() + CurrLocation.GetPositionX(),
				Positions->OtherPosOffset.GetPositionY() + CurrLocation.GetPositionY());
			
			MapCoordinate TopLeft;
			MapCoordinate BottomRight;

			// If the AboveLoc is furthest right
			if (AboveLocation.GetPositionX() <= CurrLocation.GetPositionX())
			{
				int X = AboveLocation.GetPositionX();
				TopLeft = MapCoordinate(AboveLocation.GetPositionX() - ((AboveLocation.GetPositionX() - 1 >= 0) ? 1 : 0), AboveLocation.GetPositionY() - 1);
				BottomRight = MapCoordinate(CurrLocation.GetPositionX() + ((CurrLocation.GetPositionX() + 1 < m_Width) ? 1 : 0), CurrLocation.GetPositionY() + 1);
			}
			else
			{
				TopLeft = MapCoordinate(CurrLocation.GetPositionX() - ((CurrLocation.GetPositionX() - 1 >= 0) ? 1 : 0), AboveLocation.GetPositionY() - 1);
				BottomRight = MapCoordinate(AboveLocation.GetPositionX() + ((AboveLocation.GetPositionX() + 1 < m_Width) ? 1 : 0), CurrLocation.GetPositionY() + 1);
			}

			int DistanceX = abs(TopLeft.GetPositionX() - BottomRight.GetPositionX()) + 1;
			int DistanceY = abs(TopLeft.GetPositionY() - BottomRight.GetPositionY()) + 1;

			// Create the corridor and generate it
			CorridorVertical* VertiCorrridor = new CorridorVertical(DistanceX, DistanceY);
			m_Corridors.push_back(VertiCorrridor);

			// Generate this new corridor and place it within the array
			GenerateAndPlaceCorridor(
				VertiCorrridor,
				TopLeft,
				BottomRight,
				AboveLocation,
				CurrLocation,
				DistanceX,
				DistanceY,
				Distance);
		}
	}
}

//
// ManageHorizCorridor()
// 
//
void Map::SetUpHorizCorridor(int ColumnNumber, int OffsetX, int OffsetY, int RoomOffsetX, MapRoom* Room)
{
	// No rooms to connect to
	if (ColumnNumber < 0) return;

	vector<pair<MapRoom*, MapCoordinate>> CandidateRooms;	// Holds a mapRoom and its starting topleft pos
	vector<int> CandidateIndices;
	int RoomHeight = Room->GetHeight();
	int RoomEnd = OffsetY + RoomHeight;
	MapRoom* CurrRoom = nullptr;
	int CurrRoomEnd = 0;
	int CurrRoomBeg = 0;
	pair<MapCoordinate, MapCoordinate>* TempSide = nullptr;
	MapCoordinate Beginning;
	MapCoordinate Ending;
	int Distance = 0;

	vector<MapCoordinate> RoomOffsets;

	for (size_t i = 0; i < m_ColumnOffsetsX[ColumnNumber].size(); i++)
		RoomOffsets.push_back(MapCoordinate(m_ColumnOffsetsX[ColumnNumber][i] + ColumnNumber * ColumnWidth5, m_ColumnOffsetsY[ColumnNumber][i]));


	RoomConnector HorizConnector(Room, MapCoordinate(OffsetX + RoomOffsetX, OffsetY), m_Rooms[ColumnNumber], RoomOffsets);

	if (CurrRoom = HorizConnector.FindRoomToConnectToHoriz())
	{
		DeterminedRoomPositions *Positions = HorizConnector.FindRoomPositionsHoriz();
		if (Positions)
		{
			MapCoordinate LeftMapPos = HorizConnector.GetOtherRoomPosition();

			Beginning = MapCoordinate(
				Positions->BaseOffset.GetPositionX() + OffsetX + RoomOffsetX,
				Positions->BaseOffset.GetPositionY() + OffsetY);
			Ending = MapCoordinate(
				Positions->OtherPosOffset.GetPositionX() + LeftMapPos.GetPositionX(),
				Positions->OtherPosOffset.GetPositionY() + LeftMapPos.GetPositionY());

			int Distance = abs(Beginning.GetPositionX() - Ending.GetPositionX());
			if (!Distance)
				Distance = 1;

			MapCoordinate TopLeft;
			MapCoordinate BottomRight;

			// If the AboveLoc is furthest north
			if (Beginning.GetPositionY() >= Ending.GetPositionY())
			{
				BottomRight = MapCoordinate(Beginning.GetPositionX() + 1, Beginning.GetPositionY() + 1);
				TopLeft = MapCoordinate(Ending.GetPositionX() - 1, Ending.GetPositionY() - 1);
			}
			else
			{
				BottomRight = MapCoordinate(Beginning.GetPositionX() + 1, Ending.GetPositionY() + 1);
				TopLeft = MapCoordinate(Ending.GetPositionX() - 1, Beginning.GetPositionY() - 1);
			}

			int DistanceX = abs(TopLeft.GetPositionX() - BottomRight.GetPositionX()) + 1;
			int DistanceY = abs(TopLeft.GetPositionY() - BottomRight.GetPositionY()) + 1;

			// Create the corridor and generate it
			CorridorHoriz* HorizCorrridor = new CorridorHoriz(DistanceX, DistanceY);
			m_Corridors.push_back(HorizCorrridor);

			Corridor* Pass = dynamic_cast<CorridorHoriz*>(HorizCorrridor);

			// Generate this new corridor and place it within the array
			GenerateAndPlaceCorridor(
				Pass,
				TopLeft,
				BottomRight,
				Beginning,
				Ending,
				DistanceX,
				DistanceY,
				Distance);
		}
	}
}

//
// GenerateAndPlaceCorridor()
//
//
void Map::GenerateAndPlaceCorridor(Corridor* NewCorridor, MapCoordinate TopLeft, MapCoordinate BottomRight, 
	MapCoordinate Begin, MapCoordinate End, int DistanceX, int DistanceY, int MajorDistance)
{
	vector<vector<MapCell*>> BoundsMatrix;
	vector<vector<MapCell*>> CorridorLocaleMatrix;
	MapObject* TempCell;

	MapCoordinate BoundedTop = MapCoordinate(Begin.GetPositionX() - TopLeft.GetPositionX(), Begin.GetPositionY() - TopLeft.GetPositionY());
	MapCoordinate BoundedBottom = MapCoordinate(End.GetPositionX() - TopLeft.GetPositionX(), End.GetPositionY() - TopLeft.GetPositionY());

	// Create the bounds matrix and corridor locale matrix
	for (int i = 0, X = TopLeft.GetPositionX(); i < DistanceX; i++, X++)
	{
		BoundsMatrix.push_back(vector<MapCell*>());
		CorridorLocaleMatrix.push_back(vector<MapCell*>());
		for (int j = 0, Y = TopLeft.GetPositionY(); j < DistanceY; j++, Y++)
		{
			if (TempCell = m_Cells[X][Y])
			{
				BoundsMatrix[i].push_back(static_cast<MapCell*>(m_Cells[X][Y]));
			}
			else BoundsMatrix[i].push_back(nullptr);

			if (TempCell = m_CorridorCells[X][Y])
			{
				CorridorLocaleMatrix[i].push_back(static_cast<MapCell*>(m_CorridorCells[X][Y]));
			}
			else CorridorLocaleMatrix[i].push_back(nullptr);
		}
	}

	NewCorridor->GenerateCorridor(BoundsMatrix, CorridorLocaleMatrix, BoundedTop, BoundedBottom, MajorDistance);

	// Then place the corridor cells into the map
	MapCell*** CorridorCells = NewCorridor->GetCells();

	// Create the bounds matrix and corridor locale matrix
	for (int i = 0, X = TopLeft.GetPositionX(); i < DistanceX; i++, X++)
	{
		for (int j = 0, Y = TopLeft.GetPositionY(); j < DistanceY; j++, Y++)
		{
			SetNewCorridorCell(MapCoordinate(X, Y), CorridorCells[i][j], (End == MapCoordinate(X, Y) ? true : false));
		}
	}
}

//
// SetNewCorridorCell()
// 
//
void Map::SetNewCorridorCell(MapCoordinate CellPosition, MapCell* NewCell, bool LastCell)
{
	int X = CellPosition.GetPositionX();
	int Y = CellPosition.GetPositionY();


	if ((X < m_Width) && (Y < m_Height) && (Y >= 0) && (X >= 0) && NewCell)
	{
		// If there is no cell here currently
		if (!m_Cells[X][Y])
		{
			// Then we add one
			m_Cells[X][Y] = NewCell;

			m_CorridorCells[X][Y] = m_Cells[X][Y];
			return;
		}
		// Otherwise, check if the cell if a Wall_Top type, as we will not
		// add a texture here
		MapCell* Cell = dynamic_cast<MapCell*>(m_Cells[X][Y]);

		if ((Cell->GetCellType() != Cell::Wall_Top && Cell->GetCellType() != Cell::Floor) || LastCell)
		{
			vector<string> Textures = *NewCell->ReturnRedTextures();

			// Add the texture to the list to draw with the other textures here
			for (size_t i = 0; i < Textures.size(); i++)
			{
				Cell->AddRedTexture(Textures[i]);
				if (NewCell->GetCellType() == Cell::Wall_Top)
				{
					Cell->SetCellType(Cell::Wall_Top);
				}
				else if (NewCell->GetCellType() == Cell::Floor)
				{
					Cell->SetCellType(Cell::Floor);
				}
			}
		}

		m_CorridorCells[X][Y] = m_Cells[X][Y];
	}
}

//
// SetNewCell()
//
//
void Map::SetNewCell(MapCoordinate CellPosition, MapCell* NewCell)
{
	int X = CellPosition.GetPositionX();
	int Y = CellPosition.GetPositionY();


	if ((X < m_Width) && (Y < m_Height) && (Y >= 0) && (X >= 0) && NewCell)
	{
		// If there is no cell here currently
		if (!m_Cells[X][Y])
		{
			// Then we add one
			m_Cells[X][Y] = NewCell;

			m_CorridorCells[X][Y] = m_Cells[X][Y];
			return;
		}
		// Otherwise, check if the cell if a Wall_Top type, as we will not
		// add a texture here
		MapCell* Cell = dynamic_cast<MapCell*>(m_Cells[X][Y]);

		if (Cell->GetCellType() != Cell::Wall_Top && Cell->GetCellType() != Cell::Floor)
		{
			vector<string> Textures = *NewCell->ReturnRedTextures();

			// Add the texture to the list to draw with the other textures here
			for (size_t i = 0; i < Textures.size(); i++)
			{
				Cell->AddRedTexture(Textures[i]);
				if (NewCell->GetCellType() == Cell::Wall_Top)
				{
					Cell->SetCellType(Cell::Wall_Top);
				}
				else if (NewCell->GetCellType() == Cell::Floor)
				{
					Cell->SetCellType(Cell::Floor);
				}
			}
		}
	}
}

//
// CheckCell()
// Checks the given cell and adds the texture to its location,
// allocating a new cell if needed
//
void Map::CheckCell(MapCoordinate CellPosition, std::vector<std::string> Textures,
	Cell CellType, std::vector<MapCoordinate>& TextureCoords, bool Right)
{
	int X = CellPosition.GetPositionX();
	int Y = CellPosition.GetPositionY();


	if ((X < m_Width) && (Y < m_Height) && (Y >= 0) && (X >= 0))
	{
		// If there is no cell here currently
		if (!m_Cells[X][Y])
		{
			// Then we add one
			m_Cells[X][Y] = new MapInactive(Textures, MapCoordinate(X, Y), CellType);
			// Set the corridor array
			if (Right) TextureCoords.push_back(MapCoordinate(X, Y));

			m_CorridorCells[X][Y] = m_Cells[X][Y];

			return;
		}
		// Otherwise, check if the cell if a Wall_Top type, as we will not
		// add a texture here
		MapCell* Cell = dynamic_cast<MapCell*>(m_Cells[X][Y]);

		if (Cell->GetCellType() != Cell::Wall_Top && Cell->GetCellType() != Cell::Floor)
		{

			// Add the texture to the list to draw with the other textures here
			for (size_t i = 0; i < Textures.size(); i++)
			{
				Cell->AddRedTexture(Textures[i]);
				if (CellType == Cell::Wall_Top)
				{
					Cell->SetCellType(Cell::Wall_Top);
				}
				else if (CellType == Cell::Floor)
				{
					Cell->SetCellType(Cell::Floor);
				}
			}
		}

		m_CorridorCells[X][Y] = m_Cells[X][Y];
	}
}

//
// GetColumn()
// Returns the indicate column of cells if it is within the bounds,
// otherwise it returns nullptr
//
MapObject** Map::GetColumn(int ZeroIndexedColumn)
{
	if (ZeroIndexedColumn < m_Width && ZeroIndexedColumn >= 0)
	{
		return m_Cells[ZeroIndexedColumn];
	}
	else return nullptr;
}

//
// GetRow()
// Gets the corresponding indexed row if its exists
//
MapObject** Map::GetRow(int ZeroIndexedRow)
{
	if (ZeroIndexedRow < m_Height && ZeroIndexedRow >= 0)
	{
		// Create an array
		MapObject** TempArray = new MapObject*[m_Width];
		// Get each of the cells in this column
		for (int i = 0; i < m_Width; i++)
		{
			TempArray[i] = m_Cells[i][ZeroIndexedRow];
		}
		return TempArray;
	}
	else return nullptr;
}

//
// GetRoomXFromColumnY()
// Gets the indicated room from this map if it exists, and
// sets its location of its topleft corner in the offset variables
// bool param is if the last room of the indicated column is requested
//
MapRoom* Map::GetRoomXFromColumnY(int RowX, int ColumnY, int& OffsetX, int& OffsetY, bool Last)
{
	// Checking bounds
	if ((size_t)ColumnY > m_Rooms.size() - 1)
	{
		return nullptr;
	}

	// Returning the last room of the given column
	if (Last)
	{
		int Index = m_Rooms[ColumnY].size() - 1;
		
		OffsetY = m_ColumnOffsetsY[ColumnY][Index];
		OffsetX = m_ColumnOffsetsX[ColumnY][Index] + (ColumnY * ColumnWidth5);
		return m_Rooms[ColumnY][Index];
	}

	if ((size_t)RowX > m_Rooms[ColumnY].size() - 1)
	{
		return nullptr;
	}

	OffsetY = m_ColumnOffsetsY[ColumnY][RowX];
	OffsetX = m_ColumnOffsetsX[ColumnY][RowX] + (ColumnY * ColumnWidth5);
	return m_Rooms[ColumnY][RowX];
}

//
// GetRoomOffsetsFromLastRow()
//
//
MapCoordinate* Map::GetRoomOffsetsFromLastRow(int X)
{
	int Index = m_ColumnOffsetsX.size() - 1;
	if ((size_t)X > m_ColumnOffsetsX[Index].size() - 1)
		return nullptr;

	return new MapCoordinate(m_ColumnOffsetsX[Index][X] + ColumnWidth5 * Index, m_ColumnOffsetsY[Index][X]);
}

//
// GetRoomsFromColumnX()
//
//
vector<MapRoom*> Map::GetRoomsFromColumnX(int X, bool Last)
{
	int Index = m_Rooms.size() - 1;
	if (X > Index && !Last)
		return vector<MapRoom*>();
	
	if (Last)
		return m_Rooms[Index];
	else
		return m_Rooms[X];
}

//
// SetCell()
// Sets the corresponding map cell
//
void Map::SetCell(int X, int Y, MapObject *Cell)
{
	if (X < m_Width && Y < m_Height)
		m_Cells[X][Y] = Cell;
}

//
// SetCorridorCell()
// Sets the corresponding corridor cell
//
void Map::SetCorridorCell(int X, int Y, MapObject* Cell)
{
	if (X < m_Width && Y < m_Height)
		m_CorridorCells[X][Y] = Cell;
}