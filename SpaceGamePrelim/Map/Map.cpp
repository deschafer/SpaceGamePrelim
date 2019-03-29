
#include "Map.h"
#include "MapRoom.h"
#include "MapWall.h"
#include "MapInactive.h"
#include "GenRoomComp.h"
#include "RoomConnector.h"

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
	m_Generated(false)
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
	for (size_t i = OffsetY, Count = 0; i < (m_Height - 1) && Count < MaxRoomHeight; i++, Count++)
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
		MagX < RoomWidth;
		MagX++, IndexX++)
	{
		for (size_t IndexY = OffsetY, MagY = 0;
			MagY < RoomHeight;
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
			// Translating back into map coordinates
			AboveLocation = MapCoordinate(
				Positions->BaseOffset.GetPositionX() + AboveLocation.GetPositionX(), 
				Positions->BaseOffset.GetPositionY() + AboveLocation.GetPositionY());
			CurrLocation = MapCoordinate(
				Positions->OtherPosOffset.GetPositionX() + CurrLocation.GetPositionX(),
				Positions->OtherPosOffset.GetPositionY() + CurrLocation.GetPositionY());
			
			GenerateCorridorBetween(
				AboveLocation,
				CurrLocation,
				abs(AboveLocation.GetPositionY() - CurrLocation.GetPositionY()),
				false
			);
		}
	}
}

//
// GenerateCorridorBetween()
// Attempts to draw a corridor between the two points given
//
bool Map::GenerateCorridorBetween(MapCoordinate Begin, MapCoordinate End, int DistanceBetween, bool Horizontal)
{
	enum class Movement { LEFT, RIGHT, UP, DOWN };

	float Result = (int)DistanceBetween;
	int CurrentX = Begin.GetPositionX();
	int CurrentY = Begin.GetPositionY();
	int DistanceX = abs(Begin.GetPositionX() - End.GetPositionX());
	int DistanceY = abs(Begin.GetPositionY() - End.GetPositionY());
	bool Try = false;
	bool MovementY = false;
	bool MovementX = false;

	Movement Direction;
	vector <MapCoordinate> TexturedCoords;

	if (Horizontal)
	{
		int MidPointX = (CurrentX + ((int)(Result = ceil(Result / 2)) ? (int)Result : 1));

		bool BeginY = false;
		bool EndY = false;

		if (End.GetPositionY() < Begin.GetPositionY()) Direction = Movement::UP;
		else Direction = Movement::DOWN;

		// Testing if the start is within a cell
		if (m_Cells[CurrentX + 1][CurrentY] && !m_CorridorCells[CurrentX + 1][CurrentY])
		{
			cout << "Start is within a cell" << endl;
			return false;
		}
		else if (m_CorridorCells[CurrentX + 1][CurrentY])
		{
			cout << "Double Corridor" << endl;
		}

		// Adjusting the y transition 
		// so that no conflicts occur
		while (!m_Cells[MidPointX][CurrentY] && !m_CorridorCells[MidPointX][CurrentY])
		{
			MidPointX--; // Move backwards
			if (MidPointX < 0)
			{
				cout << "No valid path found" << endl;
				return false;
			}
		}

		vector<string> Textures;
		Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
		CheckCell(MapCoordinate(CurrentX, CurrentY - 1),
			Textures,
			Cell::Wall_Top,
			TexturedCoords,
			false);
		Textures.clear();
		Textures.push_back(WallBottom);
		CheckCell(MapCoordinate(CurrentX, CurrentY + 1),
			Textures,
			Cell::Wall_Bottom,
			TexturedCoords,
			false);

		// Loading a floor texture
		Textures.clear();
		Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));
		CheckCell(MapCoordinate(CurrentX, CurrentY),
			Textures,
			Cell::Floor,
			TexturedCoords,
			false);

		// Until we reach our end destination
		while (CurrentX != End.GetPositionX())
		{
			// If we need to move x-wise
			if ((CurrentX == MidPointX &&
				(Begin.GetPositionY() != End.GetPositionY()) &&
				DistanceY) || Try)
			{
				bool Taken = false;
				// We will try every turn until we compete the x-movement
				Try = true;

				// First search our texured sides to verify 
				// Verifying that the area is clear for the corridor
				for (size_t i = 0, Pos = CurrentY; i < DistanceY; i++)
				{
					if (Direction == Movement::UP) Pos--;
					else Pos++;
					// If this cell is not occupied by an external cell
					// and it is not one of our drawn texture cells
					if ((m_Cells[CurrentX][Pos] != nullptr && m_CorridorCells[CurrentX][Pos]) && 
						(TexturedCoords.empty() ||
						!(TexturedCoords[TexturedCoords.size() - 1].GetPositionX() == CurrentX &&
							TexturedCoords[TexturedCoords.size() - 1].GetPositionY() == Pos)))
					{
						// This path is taken
						Taken = true;
						break;
					}
				}
				// If the area is taken, then increment y and try next time
				if (Taken)
				{
					CurrentX++;
					MovementX = true;
				}
				// Otherwise move in correct x pos
				else if (Direction == Movement::DOWN)
				{
					if (CurrentY == Begin.GetPositionY()) BeginY = true;
					CurrentY++;
					if (CurrentY == End.GetPositionY()) EndY = true;
					DistanceY--;
					MovementY = true;
				}
				else
				{
					if (CurrentY == Begin.GetPositionY()) BeginY = true;
					CurrentY--;
					if (CurrentY == End.GetPositionY()) EndY = true;
					DistanceY--;
					MovementY = true;
				}
				// Try until we are done moving y-wise
				if (!DistanceY) Try = false;
			}
			// Increment X by default
			else
			{
				CurrentX++;
				MovementX = true;
			}

			// Adding the corridor to the array
			m_Cells[CurrentX][CurrentY] =
				new MapInactive(Textures, MapCoordinate(CurrentX, CurrentY), Cell::Floor);
			// Add to the corridor array
			m_CorridorCells[CurrentX][CurrentY] = m_Cells[CurrentX][CurrentY];

			// Determining correct tile types for the corresponding cells
			// Normal X movement
			if (MovementX)
			{
				vector<string> Textures;
				Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
				CheckCell(MapCoordinate(CurrentX, CurrentY - 1),
					Textures,
					Cell::Wall_Top,
					TexturedCoords,
					(Direction == Movement::UP) ? true : false);
				Textures.clear();
				Textures.push_back(WallBottom);
				CheckCell(MapCoordinate(CurrentX, CurrentY + 1),
					Textures,
					Cell::Wall_Bottom,
					TexturedCoords,
					(Direction == Movement::DOWN) ? true : false);

				MovementX = false;
			}
			else if (MovementY)
			{
				// Beginning cell for y movement
				if (BeginY)
				{
					vector<string> Textures;
					if (Direction != Movement::UP)
					{
						Textures.push_back(WallSideRight);
						CheckCell(MapCoordinate(CurrentX - 1, CurrentY),
							Textures,
							Cell::Wall_Right,
							TexturedCoords,
							false);
					}

					Textures.clear();
					Textures.push_back(WallSideLeft);
					CheckCell(MapCoordinate(CurrentX + 1, CurrentY),
						Textures,
						Cell::Wall_Left,
						TexturedCoords,
						false);
					CheckCell(MapCoordinate(CurrentX + 1, CurrentY + ((Direction == Movement::UP) ? 1 : -1)),
						Textures,
						Cell::Wall_Left,
						TexturedCoords,
						false);

					Textures.clear();

					Direction == Movement::UP ? Textures.push_back(WallCornerRight) : Textures.push_back(WallSideLeft);

					CheckCell(MapCoordinate(CurrentX + 1, CurrentY + ((Direction == Movement::UP) ? 2 : -2)),
						Textures,
						Direction == Movement::UP ? Cell::Wall_Corner_Right : Cell::Wall_Left,
						TexturedCoords,
						false);

				}
				// Ending cell for y movement
				if (EndY)
				{

					vector<string> Textures;
					(Direction == Movement::UP) ? 
						Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup)) : 
						Textures.push_back(WallBottom);
					CheckCell(MapCoordinate(CurrentX, CurrentY + ((Direction == Movement::UP) ? -1 : 1)),
						Textures,
						(Direction == Movement::UP) ? Cell::Wall_Top : Cell::Wall_Bottom,
						TexturedCoords,
						false);

					Textures.clear();
					Textures.push_back(WallSideRight);
					CheckCell(MapCoordinate(CurrentX - 1, CurrentY),
						Textures,
						Cell::Wall_Right,
						TexturedCoords,
						false);
					if (Direction == Movement::UP)
					{
						CheckCell(MapCoordinate(CurrentX - 1, CurrentY -1),
							Textures,
							Cell::Wall_Right,
							TexturedCoords,
							false);
					}
					else
					{
						Textures.clear();
						Textures.push_back(WallCornerLeft);
						CheckCell(MapCoordinate(CurrentX - 1, CurrentY + 1),
							Textures,
							Cell::Wall_Corner_Left,
							TexturedCoords,
							false);
					}
				}
				// Normal y movement cell
				if (!BeginY && !EndY)
				{
					vector<string> Textures;
					Textures.push_back(WallSideRight);
					CheckCell(MapCoordinate(CurrentX - 1, CurrentY),
						Textures,
						Cell::Wall_Right,
						TexturedCoords,
						false);

					Textures.clear();
					Textures.push_back(WallSideLeft);
					CheckCell(MapCoordinate(CurrentX + 1, CurrentY),
						Textures,
						Cell::Wall_Left,
						TexturedCoords,
						false);
				}

				BeginY = false;
				EndY = false;
				MovementY = false;
			}
		}
		if (CurrentX != End.GetPositionX() || CurrentY != End.GetPositionY())
		{
			cout << "Did not find the end." << endl;
		}
	}
	else
	{
		int MidPointY = (CurrentY + ((Result = ceil(Result / 2)) ? Result : 1));

		bool BeginX = false;
		bool EndX = false;

		if (End.GetPositionX() > Begin.GetPositionX()) Direction = Movement::RIGHT;
		else Direction = Movement::LEFT;

		// Adjusting the x transition 
		// so that no conflicts occur
		while (m_Cells[CurrentX][MidPointY] != nullptr)
		{
			MidPointY--;
		}

		// Loading a floor texture
		vector<string> Textures;
		Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));

		// Setting the begging cell
		vector<string> TexturesT;
		TexturesT.push_back(WallSideRight);
		CheckCell(MapCoordinate(CurrentX - 1, CurrentY),
			TexturesT,
			Cell::Wall_Right,
			TexturedCoords,
			(Direction == Movement::LEFT));

		TexturesT.clear();
		TexturesT.push_back(WallSideLeft);
		CheckCell(MapCoordinate(CurrentX + 1, CurrentY),
			TexturesT,
			Cell::Wall_Left,
			TexturedCoords,
			(Direction == Movement::RIGHT));

		CheckCell(MapCoordinate(CurrentX, CurrentY),
			Textures,
			Cell::Floor,
			TexturedCoords,
			false);

		// Until we reach our end destination
		while (CurrentY != End.GetPositionY())
		{
			// If we need to move x-wise
			if ((CurrentY == MidPointY &&
				(Begin.GetPositionX() != End.GetPositionX()) &&
				DistanceX) || Try)
			{
				bool Taken = false;
				// We will try every turn until we compete the x-movement
				Try = true;

				// First search our texured sides to verify 
				// Verifying that the area is clear for the corridor
				for (size_t i = 0, Pos = CurrentX; i < DistanceX; i++)
				{
					if (Direction == Movement::LEFT) Pos--;
					else Pos++;
					// If this cell is not occupied by an external cell
					// and it is not one of our drawn texture cells
					if (m_Cells[Pos][CurrentY] != nullptr && (TexturedCoords.empty() ||
						!(TexturedCoords[TexturedCoords.size() - 1].GetPositionY() == CurrentY &&
							TexturedCoords[TexturedCoords.size() - 1].GetPositionX() == Pos)))
					{
						// This path is taken
						Taken = true;
						break;
					}
				}

				// If the area is taken, then increment y and try next time
				if (Taken)
				{
					CurrentY++;
					MovementY = true;
				}
				// Otherwise move in correct x pos
				else if (Direction == Movement::RIGHT)
				{
					if (CurrentX == Begin.GetPositionX()) BeginX = true;
					CurrentX++;
					if (CurrentX == End.GetPositionX()) EndX = true;
					DistanceX--;
					MovementX = true;
				}
				else
				{
					if (CurrentX == Begin.GetPositionX()) BeginX = true;
					CurrentX--;
					if (CurrentX == End.GetPositionX()) EndX = true;
					DistanceX--;
					MovementX = true;
				}
				// Try until we are done moving x-wise
				if (!DistanceX) Try = false;
			}
			// Increment Y be default
			else
			{
				CurrentY++;
				MovementY = true;
			}

			// Adding the corridor to the array
			m_Cells[CurrentX][CurrentY] =
				new MapInactive(Textures, MapCoordinate(CurrentX, CurrentY), Cell::Floor);
			// Add to the corridor array
			m_CorridorCells[CurrentX][CurrentY] = m_Cells[CurrentX][CurrentY];

			// set as floor cell

			// If there was a y movement,  then we add to the 
			// cells to the left and right to add the walls to
			// encompass the corridor
			if (MovementY)
			{
				vector<string> Textures;
				Textures.push_back(WallSideRight);
				CheckCell(MapCoordinate(CurrentX - 1, CurrentY),
					Textures,
					Cell::Wall_Right,
					TexturedCoords,
					(Direction == Movement::LEFT));

				Textures.clear();
				Textures.push_back(WallSideLeft);
				CheckCell(MapCoordinate(CurrentX + 1, CurrentY),
					Textures,
					Cell::Wall_Left,
					TexturedCoords,
					(Direction == Movement::RIGHT));

				MovementY = false;
			}
			// Otherwise, if a x movement occurred
			else if (MovementX)
			{
				// The beg of the x-movement
				// DIRECTION MATTERS
				if (BeginX)
				{
					int Change = 0;
					int LessChange = 0;

					if (Direction == Movement::LEFT)
					{
						Change += 2;
						LessChange++;
					}
					else
					{
						Change -= 2;
						LessChange--;
					}
					vector<string> Textures;
					Textures.push_back(WallBottom);
					CheckCell(MapCoordinate(CurrentX + LessChange, CurrentY + 1),
						Textures,
						Cell::Wall_Bottom,
						TexturedCoords,
						false);

					CheckCell(MapCoordinate(CurrentX, CurrentY + 1),
						Textures,
						Cell::Wall_Bottom,
						TexturedCoords,
						false);

					Textures.clear();
					(Direction == Movement::LEFT) ? Textures.push_back(WallCornerRight) : Textures.push_back(WallCornerLeft);
					CheckCell(
						MapCoordinate(CurrentX + Change, CurrentY + 1),
						Textures, (Direction == Movement::LEFT) ?
						Cell::Wall_Corner_Right_Bottom :
						Cell::Wall_Corner_Left_Bottom,
						TexturedCoords,
						false);

					Textures.clear();
					Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
					CheckCell(MapCoordinate(CurrentX, CurrentY - 1),
						Textures,
						Cell::Wall_Top,
						TexturedCoords,
						false);

				}
				// The end of the x-movement
				// DIRECTION MATTERS
				if (EndX)
				{
					int Change = 0;

					if (Direction == Movement::LEFT) Change = -1;
					else Change = 1;

					vector<string> Textures;
					(Change > 0) ? Textures.push_back(WallSideLeft) : Textures.push_back(WallSideRight);
					CheckCell(MapCoordinate(CurrentX + Change, CurrentY - 1),
						Textures,
						(Change > 0) ? Cell::Wall_Left : Cell::Wall_Right,
						TexturedCoords,
						false);
					CheckCell(MapCoordinate(CurrentX + Change, CurrentY),
						Textures,
						(Change > 0) ? Cell::Wall_Left : Cell::Wall_Right,
						TexturedCoords,
						false);
					Textures.clear();
					Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
					CheckCell(MapCoordinate(CurrentX, CurrentY - 1),
						Textures,
						Cell::Wall_Top,
						TexturedCoords,
						false);

				}
				// Adding to the cells above and below this current cell
				// Does not matter with the direction of the x corridor movement
				else if (!BeginX && !EndX)
				{
					vector<string> Textures;
					Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
					CheckCell(MapCoordinate(CurrentX, CurrentY - 1),
						Textures,
						Cell::Wall_Top,
						TexturedCoords,
						false);

					Textures.clear();
					Textures.push_back(WallBottom);
					CheckCell(MapCoordinate(CurrentX, CurrentY + 1),
						Textures,
						Cell::Wall_Bottom,
						TexturedCoords,
						false);
				}
				EndX = false;
				BeginX = false;
				MovementX = false;
			}
		}
	}

	return true;
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
	pair<MapCoordinate, MapCoordinate>* GetLocationBegSide;
	pair<MapCoordinate, MapCoordinate>* TempSide = nullptr;
	pair<MapCoordinate, MapCoordinate>* GetLocationEndSide;

	MapCoordinate Beginning;
	MapCoordinate Ending;

	vector<MapCoordinate> RoomOffsets;

	for (size_t i = 0; i < m_ColumnOffsetsX[ColumnNumber].size(); i++)
		RoomOffsets.push_back(MapCoordinate(m_ColumnOffsetsX[ColumnNumber][i] + ColumnNumber * ColumnWidth5, m_ColumnOffsetsY[ColumnNumber][i]));
	

	RoomConnector HorizConnector(Room, MapCoordinate(OffsetX + RoomOffsetX, OffsetY), m_Rooms[ColumnNumber], RoomOffsets);

	if (HorizConnector.FindRoomToConnectToHoriz())
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

			// Have to convert back into map coordinates
			GenerateCorridorBetween(
				Ending,
				Beginning,
				abs(Beginning.GetPositionX() - Ending.GetPositionX()),
				true
			);
		}
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
		for (size_t i = 0; i < m_Width; i++)
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
	if (ColumnY > m_Rooms.size() - 1)
	{
#ifdef _DEBUG
		cout << "GetRoomXFromColumnY: Requested column is out of bounds" << endl;
#endif // _DEBUG
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

	if (RowX > m_Rooms[ColumnY].size() - 1)
	{
#ifdef _DEBUG
		cout << "GetRoomXFromColumnY: Requested row is out of bounds" << endl;
#endif // _DEBUG
		return nullptr;
	}

	OffsetY = m_ColumnOffsetsY[ColumnY][RowX];
	OffsetX = m_ColumnOffsetsX[ColumnY][RowX] + (ColumnY * ColumnWidth5);
	return m_Rooms[ColumnY][RowX];
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