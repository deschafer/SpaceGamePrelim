
#include "Map.h"
#include "MapRoom.h"
#include "MapWall.h"
#include "MapInactive.h"
//#include "GenRoomComp.h"

#include "..\Frame\MainApplication.h"

#include <chrono>

//#define DEBUG_CORRIDOR_VERTICAL
//#define DEBUG_CORRIDOR_HORIZ

enum class Side { TOP, RIGHT, BOTTOM, LEFT, COMPL };


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
	m_MapCoordinates(Coords)
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
	for (size_t i = OffsetY, Count = 0; i < m_Height && Count < MaxRoomHeight; i++, Count++)
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

	SetUpCorridor(ColNumber, OffsetX, OffsetY, xOffset, Room);
	SetUpHorizCorridor(ColNumber - 1, OffsetX, OffsetY, xOffset, Room);


	// Generating the next room below this one
	GenerateRoom(OffsetX, OffsetY + RoomHeight + rand() % ColumnSeparatorMax + ColumnSeparatorMin, MaxWidth, ColNumber);
}

//
// SetUpCorridor()
// Finds suitable starting cells for the corridor to be created.
//
void Map::SetUpCorridor(int ColumnNumber, int ColumnOffsetX, int OffsetY, int RoomOffsetX, MapRoom* BottomRoom)
{
	int Size = 0;

	// Generate a path to above room if possible
	if ((Size = m_Rooms[ColumnNumber].size() - 2) >= 0)
	{
		// There is a room above us, so we need to find a suitable location for a corridor.
		MapRoom* RoomAbove = m_Rooms[ColumnNumber][Size];
		MapCoordinate CurrLocation;
		MapCoordinate AboveLocation;
		pair<MapCoordinate, MapCoordinate>* GetLocationBeg;
		pair<MapCoordinate, MapCoordinate>* GetLocationEnd;

		vector<string> temp;
		int AboveWidth = RoomAbove->GetWidth();
		int AboveOffsetX = m_ColumnOffsetsX[ColumnNumber][Size];
		int CurrentOffsetX = RoomOffsetX;
		int CenterPointY = m_ColumnOffsetsY[ColumnNumber][Size] + RoomAbove->GetHeight();
		int CenterPointX = AboveOffsetX + AboveWidth / 2 + ColumnOffsetX;
		int ThisRoomOffsetY = OffsetY;
		int AboveDifference = 0;
		int CurrDifference = 0;
		int ShorterXLoc = 0;
		bool CurrentIsShorter = false;

		// Ask the room above for a suitable side to set as a beginning tile for the end point
		// for this corridor.
		// Get a wall from the indicated side
		GetLocationBeg = RoomAbove->GetFacingFromSide(Side::BOTTOM);
		// Do the same for the current room on the top side
		GetLocationEnd = BottomRoom->GetFacingFromSide(Side::TOP);

		// Next find the smallest side
		// If this wall with min size exists
		if (GetLocationBeg)
		{
			AboveDifference = (GetLocationBeg->first.GetPositionX() - GetLocationBeg->second.GetPositionX());
		}
		else return;
		if (GetLocationEnd)
		{
			CurrDifference = (GetLocationEnd->first.GetPositionX() - GetLocationEnd->second.GetPositionX());
		}
		else return;

		// Handle the shorter side first
		if (abs(CurrDifference) > abs(AboveDifference))
		{
			AboveLocation = MapCoordinate(GetLocationBeg->first.GetPositionX() - AboveDifference / 2 + AboveOffsetX, GetLocationBeg->first.GetPositionY());
			ShorterXLoc = AboveLocation.GetPositionX(); // Saving this x pos with respect to column offset
		}
		else 
		{
			CurrLocation = MapCoordinate(GetLocationEnd->first.GetPositionX() - CurrDifference / 2 + CurrentOffsetX, GetLocationEnd->first.GetPositionY());
			ShorterXLoc = CurrLocation.GetPositionX(); // Saving this x pos with respect to column offset
			CurrentIsShorter = true;
		}

		// Then we need to process the room above
		if (CurrentIsShorter)
		{
			// Standardize the coordinates so we can compare them
			int XPos1 = GetLocationBeg->first.GetPositionX() + AboveOffsetX;
			int XPos2 = GetLocationBeg->second.GetPositionX() + AboveOffsetX;


			// If the x coord of the shorter is within the range of this side
			// if so, that is the new point
			if ((XPos1 < ShorterXLoc && ShorterXLoc < XPos2) ||
				(XPos1 > ShorterXLoc && ShorterXLoc > XPos2))
			{
				AboveLocation = MapCoordinate(ShorterXLoc, GetLocationBeg->first.GetPositionY());
			}
			else AboveLocation = MapCoordinate(GetLocationBeg->first.GetPositionX() - AboveDifference / 2 + AboveOffsetX, GetLocationBeg->first.GetPositionY());
			
		}
		// Then we need to process the room below
		else
		{
			int XPos1 = GetLocationEnd->first.GetPositionX() + CurrentOffsetX;
			int XPos2 = GetLocationEnd->second.GetPositionX() + CurrentOffsetX;

			// If the x coord of the shorter is within the range of this side
			// if so, that is the new point
			if ((XPos1 < ShorterXLoc && ShorterXLoc < XPos2) ||
				(XPos1 > ShorterXLoc && ShorterXLoc > XPos2))
			{
				CurrLocation = MapCoordinate(ShorterXLoc, GetLocationEnd->first.GetPositionY());
			}
			else CurrLocation = MapCoordinate(GetLocationEnd->first.GetPositionX() - CurrDifference / 2 + CurrentOffsetX, GetLocationEnd->first.GetPositionY());
		}

		GenerateCorridorBetween(
			MapCoordinate(
				AboveLocation.GetPositionX() + ColumnOffsetX, 
				AboveLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size]), 
			MapCoordinate(
				CurrLocation.GetPositionX() + ColumnOffsetX, 
				CurrLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size + 1]
			),
			ThisRoomOffsetY - CenterPointY,
			false
		);

#ifdef DEBUG_CORRIDOR_VERTICAL
		vector<string> Textures;
		Textures.push_back("Test");

		m_Cells[CurrLocation.GetPositionX() + ColumnOffsetX][CurrLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size + 1]] = 
			new MapWall(Textures, MapCoordinate(0, 0), Cell::Floor);
		m_Cells[AboveLocation.GetPositionX() + ColumnOffsetX][AboveLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size]] = 
			new MapWall(Textures, MapCoordinate(0, 0), Cell::Floor);
#endif // DEBUG_CORRIDOR_VERTICAL

	}
}

//
// GenerateCorridorBetween()
// Attempts to draw a corridor between the two points given
//
bool Map::GenerateCorridorBetween(MapCoordinate Begin, MapCoordinate End, int DistanceBetween, bool Horizontal)
{
	enum class Movement { LEFT, RIGHT, UP, DOWN };

	float Result = DistanceBetween;
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
		int MidPointX = (CurrentX + ((Result = ceil(Result / 2)) ? Result : 1));

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

	// Find room candidates to the left using the offsets given
	// Add these to a list

	vector<string> Textures;

	static int Counter = 0;

	switch (Counter)
	{
	case 0:
		Counter++;
		Textures.push_back("Test");
		break;
	case 1:
		Textures.push_back("Test2");
		Counter = 0;
		break;
	default:
		break;
	}

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

	for (size_t i = 0; i < (CurrRoomBeg = m_ColumnOffsetsY[ColumnNumber].size()); i++)
	{
		CurrRoomBeg = m_ColumnOffsetsY[ColumnNumber][i];
		CurrRoom = m_Rooms[ColumnNumber][i];
		CurrRoomEnd = CurrRoom->GetHeight() + CurrRoomBeg;

		// If this room is within the bounds of the room next to it,
		// we add it as a candidate for corridor generation
		if(CurrRoomBeg <= RoomEnd && CurrRoomEnd > OffsetY)
		{
			CandidateRooms.push_back(pair<MapRoom*, MapCoordinate>(m_Rooms[ColumnNumber][i], MapCoordinate(m_ColumnOffsetsX[ColumnNumber][i], CurrRoomBeg)));
			CandidateIndices.push_back(i);
		}
		// If we have passed the margin, stop looking
		else if (CurrRoomBeg > RoomEnd) break;

	}

	// If there are no rooms, then we do not connect
	// Only happens rarely
	if (CandidateRooms.empty()) return;


	// Pick one of these rooms that preferable does not already have a link
	// Pick to one closed to the middle of this room

	// If there is only one room present, then we will connect to the room regardless
	if (CandidateRooms.size() == 1)
	{
		FindCandidateSidePositions(
			Room, 
			OffsetX + RoomOffsetX, 
			OffsetY, 
			CandidateRooms[0].first, 
			m_ColumnOffsetsX[ColumnNumber][CandidateIndices[0]], 
			m_ColumnOffsetsY[ColumnNumber][CandidateIndices[0]], 
			ColumnNumber);
	}
	// If more than one room available, the anwser is more difficult
	else
	{
		vector<bool> Connections;
		int ConnectionlessCount = 0;
		int ConnectionlessIndex = -1;	// Holds the index of a connectionless room -- intended for one connectionless room only
		MapRoom* SelectedRoom = nullptr;

		for (size_t i = 0; i < CandidateRooms.size(); i++)
		{
			// Adding all of the connections for the candiate rooms
			if (CandidateRooms[i].first->ConnectedToRoom(Side::RIGHT))
			{
				Connections.push_back(true);
			}
			else
			{
				Connections.push_back(false);
				ConnectionlessCount++;
				ConnectionlessIndex = i;
			}
		}

		// If no rooms are not connected to
		if (ConnectionlessCount == 0)
		{
			return;
		}

		// If any of the rooms given does not have a connection, connect to one of those
		// If one connectionless room, connect to that one
		else if (ConnectionlessCount == 1)
		{
			FindCandidateSidePositions(
				Room,
				OffsetX + RoomOffsetX,
				OffsetY,
				CandidateRooms[ConnectionlessIndex].first,
				m_ColumnOffsetsX[ColumnNumber][CandidateIndices[ConnectionlessIndex]],
				m_ColumnOffsetsY[ColumnNumber][CandidateIndices[ConnectionlessIndex]],
				ColumnNumber);
		}
		// More than one possible room
		else if (ConnectionlessCount)
		{
			int Index = -1;
			// Find the connectionless room that is above
			for (size_t i = 0; i < Connections.size(); i++)
			{
				// Due to how the rooms are found, from up to down, we can
				// always assume that the first connectionless index is the highest room
				if (!Connections[i])
				{
					Index = i;
					break;
				}
			}

			if (Index == -1) cout << "Error?: Incorred index" << endl;
			else
			{
				FindCandidateSidePositions(
					Room,
					OffsetX + RoomOffsetX,
					OffsetY,
					CandidateRooms[Index].first,
					m_ColumnOffsetsX[ColumnNumber][CandidateIndices[Index]],
					CandidateRooms[Index].second.GetPositionY(),
					ColumnNumber);
			}

		}
		// If more than one, connect to the one above



		// If all rooms do not have a connection, connect to the upper room
		// If all have links, that connect to the closest room


	}
}

//
//
//
//
void Map::FindCandidateSidePositions(MapRoom* RightRoom, int RightOffsetX, int RightOffsetY, MapRoom* LeftRoom, int LeftOffsetX, int LeftOffsetY, int ColumnNumber)
{
	vector<string> Textures;
	static int Counter = 0;

	switch (Counter)
	{
	case 0:
		Counter++;
		Textures.push_back("Test");
		break;
	case 1:
		Textures.push_back("Test2");
		Counter = 0;
		break;
	default:
		break;
	}

	int YStartPos = 0;
	int Difference = 0;
	pair<MapCoordinate, MapCoordinate>* GetLocationBegSide;
	pair<MapCoordinate, MapCoordinate>* TempSide = nullptr;
	pair<MapCoordinate, MapCoordinate>* GetLocationEndSide;

	// Set a start position within +/- 2 blocks of the center side (if Possible)
	// Save the pos. of this side
	// Get a side that is within +/- 2 blocks of the first selected side
	// Add new mapcells with text texutres at both sides
	// Eventually, use a horiz corridor creator to create the connection

	// First get a minor side from the major left side for the beginning room
	GetLocationBegSide = RightRoom->GetFacingFromSide(Side::LEFT);

	// Then select a YPos in here for the actual start.

	Difference = GetLocationBegSide->first.GetPositionY() - GetLocationBegSide->second.GetPositionY();

	// This will a part of this side with variation
	YStartPos = rand() % (Difference - 1) + GetLocationBegSide->second.GetPositionY() + 1;

	// Now get the closest end position to here
	// First, find a ending side, if it exists, otherwise get the closest

	int LeftPositionBeg = 0;
	int RightPositionBeg = YStartPos - ComparisonSideWidth / 2 + RightOffsetY;
	int LeftPositionEnd = 0;
	int RightPositionEnd = YStartPos + ComparisonSideWidth / 2 + RightOffsetY;

	//int RightPositionBaseBeg = GetLocationBegSide->second.GetPositionY() + RightOffsetY;
	//int RightPositionBaseEnd = GetLocationBegSide->first.GetPositionY() + RightOffsetY;
	int RightPositionBaseBeg = RightOffsetY;
	int RightPositionBaseEnd = RightRoom->GetHeight() + RightOffsetY;

	int LeftRoomOffsetX = ColumnNumber * ColumnWidth5;
	int LeftRoomColumnOffsetX = LeftOffsetX;
	int LeftRoomOffsetY = LeftOffsetY;

	vector<pair<MapCoordinate, MapCoordinate>> WideCandidateSides;
	vector<pair<MapCoordinate, MapCoordinate>> CandidateSides;
	bool PerfectFit = false;

	// TODO: Take all the declarations outside of the loop below

	for (size_t Index = 0; ; Index++)
	{
		TempSide = LeftRoom->GetFacingFromSideIndexed(Side::RIGHT, Index);

		if (!TempSide) break;

		LeftPositionBeg = TempSide->first.GetPositionY() + LeftRoomOffsetY;
		LeftPositionEnd = TempSide->second.GetPositionY() + LeftRoomOffsetY;

		// Determine if this side is within the desired position
		// First case - side is within a set param, like 2 blocks, of the starting corridor position
		if (LeftPositionBeg <= RightPositionEnd && LeftPositionEnd > RightPositionBeg)
		{
			CandidateSides.push_back(*TempSide);
		}
		// Second case - side is within the bounds of the entire side of where the starting corridor pos. is
		else if (LeftPositionBeg <= RightPositionBaseEnd && LeftPositionEnd >= RightPositionBaseBeg)
		{
			WideCandidateSides.push_back(*TempSide);
		}
		// Third case - If the exact Y position of the starting corridor position is located within this side
		if (LeftPositionBeg <= (YStartPos + RightOffsetY) && LeftPositionEnd >= (YStartPos + RightOffsetY))
		{
			PerfectFit = true;
			// We have a fitting side, so leave
			break;
		}
	}

	// Setting a formal MapCoord for the starting room pos.
	MapCoordinate LeftRoomPoint(
		RightOffsetX + GetLocationBegSide->first.GetPositionX(),
		RightOffsetY + YStartPos);
	MapCoordinate RightRoomPoint(0, 0);

	if (PerfectFit)
	{
		// Good to go, we can set this ypos as the ending pos.

		// Need to verify we are not at the absolute end of this side
		// Since we are on the Right side of the left room,
		// Verify that this point is not equal to a point in the room. 
		// If it is, incr or decr appropriately.

		int Pos = YStartPos + RightOffsetY;

		// Upper part of this side section
		// Determining if this point is at the endge of this side, which
		// usually is not a valid place for a corridor
		if (Pos == TempSide->first.GetPositionY() + LeftRoomOffsetY)
		{
			// Move downwards one, increment by 1
			Pos++;
		}
		else if (Pos == TempSide->second.GetPositionY() + LeftRoomOffsetY)
		{
			// Move upwards one, increment by 1
			Pos--;
		}

		// Save this position
		RightRoomPoint = MapCoordinate(
			LeftRoomOffsetX + LeftRoomColumnOffsetX + TempSide->first.GetPositionX(),
			Pos);
	}
	// If there are several sides that work
	else if (!CandidateSides.empty() || !WideCandidateSides.empty())
	{
		// Select a random side from those available
		if (!CandidateSides.empty())
		{
			int SideIndex = rand() % CandidateSides.size();
			TempSide = &CandidateSides[SideIndex];
		}
		else if (!WideCandidateSides.empty())
		{
			int SideIndex = rand() % WideCandidateSides.size();
			TempSide = &WideCandidateSides[SideIndex];
		}

		// Get a magnitude of the difference
		int SideDifference = TempSide->second.GetPositionY() - TempSide->first.GetPositionY();

		// Start at the middle of this side
		int Pos = TempSide->first.GetPositionY() + SideDifference / 2 + LeftRoomOffsetY;
		int Goal = YStartPos + RightOffsetY;
		int EndLegalSideTop = TempSide->first.GetPositionY() + LeftRoomOffsetY + 1;
		int StartLegalSideBottom = TempSide->second.GetPositionY() + LeftRoomOffsetY - 1;

		// Move to a pos as close to the other point as possible
		while (true)
		{
			if (Pos > Goal)
			{
				if (Pos - 1 > EndLegalSideTop) Pos--;
				else break;
			}
			else
			{
				if (Pos + 1 < StartLegalSideBottom) Pos++;
				else break;
			}
		}

		// Save this new position
		RightRoomPoint = MapCoordinate(
			LeftRoomOffsetX + LeftRoomColumnOffsetX + TempSide->first.GetPositionX(),
			Pos);
	}
	// If there was an ending point found
	if (TempSide != nullptr)
	{
		int RightRoomXPos = RightOffsetX;
		int LeftRoomXPos = LeftRoomOffsetX + LeftRoomColumnOffsetX + LeftRoom->GetWidth();

		// Adds the link to the left room
		LeftRoom->AddLinkedRoom(Side::RIGHT, RightRoom);
		// Adds the link to the right room
		RightRoom->AddLinkedRoom(Side::LEFT, LeftRoom);

		GenerateCorridorBetween(
			MapCoordinate(RightRoomPoint),
			MapCoordinate(LeftRoomPoint),
			abs(RightRoomXPos - LeftRoomXPos),
			true
		);

	#ifdef DEBUG_CORRIDOR_HORIZ
		//cout << "Room -- " << RightRoom->GetRoomType() << " Connected to " << LeftRoom->GetRoomType() << " with distance between " << abs(RightRoomXPos - LeftRoomXPos) << endl;
		m_Cells[RightRoomPoint.GetPositionX()][RightRoomPoint.GetPositionY()] = new MapWall(Textures, MapCoordinate(RightRoomPoint), Cell::Floor);
	#endif // DEBUG_CORRIDOR_HORIZ
	}
	else
	{
	#ifdef _DEBUG
		cout << "Corridor Horiz Gen Error: No ending point found for " << RightRoom->GetRoomType() << " to " << LeftRoom->GetRoomType() << endl;
	#endif // _DEBUG
	}
	#ifdef DEBUG_CORRIDOR_HORIZ
	m_Cells[LeftRoomPoint.GetPositionX()][LeftRoomPoint.GetPositionY()] = new MapWall(Textures, MapCoordinate(LeftRoomPoint), Cell::Floor);
	#endif // DEBUG_CORRIDOR_HORIZ
}

