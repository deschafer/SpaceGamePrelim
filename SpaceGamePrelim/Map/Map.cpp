
#include "Map.h"
#include "MapRoom.h"
#include "MapWall.h"
#include "MapInactive.h"
//#include "GenRoomComp.h"

#include "..\Frame\MainApplication.h"

#include <chrono>

enum class Side { TOP, RIGHT, BOTTOM, LEFT, COMPL };
enum class Movement { LEFT, RIGHT };


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
static const int ColumnSeparatorMax = 2;
static const int ColumnSeparatorMin = 1;

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
	for (int i = 0; i < m_Height; i++)
	{
		m_Cells[i] = new MapObject*[m_Width];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Height; i++)
	{
		for (int j = 0; j < m_Width; j++)
		{
			m_Cells[i][j] = nullptr;
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

	

	int Size = 0;
	

	// Add new rooms to the storage vector
	m_Rooms[ColNumber].push_back(Room);
	// Adding this rooms column offset to simplify corridor gen later on
	m_ColumnOffsetsX[ColNumber].push_back(xOffset);
	m_ColumnOffsetsY[ColNumber].push_back(OffsetY);

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
			m_Cells[IndexX][IndexY] = Room->GetCell(MagX, MagY);
		}
	}

	SetUpCorridor(ColNumber, OffsetX, xOffset);

	// Generating the next room below this one
	GenerateRoom(OffsetX, OffsetY + RoomHeight + rand() % ColumnSeparatorMax + ColumnSeparatorMin, MaxWidth, ColNumber);
}

//
// SetUpCorridor()
// Finds suitable starting cells for the corridor to be created.
//
void Map::SetUpCorridor(int ColumnNumber, int ColumnOffsetX, int RoomOffsetX)
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
		int CurrentOffsetX = m_ColumnOffsetsX[ColumnNumber][Size + 1];
		int CenterPointY = m_ColumnOffsetsY[ColumnNumber][Size] + RoomAbove->GetHeight();
		int AxisOffsetY = 0;
		int CenterPointX = AboveOffsetX + AboveWidth / 2 + ColumnOffsetX;
		int ThisRoomOffsetY = 0;
		int AboveDifference = 0;
		int CurrDifference = 0;
		int ShorterXLoc = 0;
		bool CurrentIsShorter = false;

		// Adding a test texture to obeserve the location
		temp.push_back("Test");

		// Ask the room above for a suitable side to set as a beginning tile for the end point
		// for this corridor.
		// Get a wall from the indicated side
		GetLocationBeg = RoomAbove->GetFacingFromSide(Side::BOTTOM);
		// Do the same for the current room on the top side
		GetLocationEnd = m_Rooms[ColumnNumber][Size + 1]->GetFacingFromSide(Side::TOP);

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

		// Finally adding to the cell array so we can see these tiles
		m_Cells[AboveLocation.GetPositionX() + ColumnOffsetX][AboveLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size]] =
			new MapWall(temp, MapCoordinate(AboveLocation.GetPositionX() + ColumnOffsetX, AboveLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size]), Cell::Floor);


		// Adding a different test texture
		temp.clear();
		temp.push_back("Test2");

		m_Cells[CurrLocation.GetPositionX() + ColumnOffsetX][CurrLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size + 1]] =
			new MapWall(temp, MapCoordinate(CurrLocation.GetPositionX() + ColumnOffsetX, CurrLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size + 1]), Cell::Floor);

		GenerateCorridorBetween(
			MapCoordinate(
				AboveLocation.GetPositionX() + ColumnOffsetX, 
				AboveLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size]), 
			MapCoordinate(
				CurrLocation.GetPositionX() + ColumnOffsetX, 
				CurrLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size + 1]
			)
		);
	}
}

//
// GenerateCorridorBetween()
// Attempts to draw a corridor between the two points given
//
bool Map::GenerateCorridorBetween(MapCoordinate Begin, MapCoordinate End)
{

	int CurrentX = Begin.GetPositionX();
	int CurrentY = Begin.GetPositionY();
	int DistanceX = abs(Begin.GetPositionX() - End.GetPositionX());
	int DistanceY = abs(Begin.GetPositionY() - End.GetPositionY());
	int MidPointY = CurrentY + DistanceY / 2;
	Movement Direction;
	bool Try = false;
	bool MovementY = false;
	bool MovementX = false;
	bool BeginX = false;
	bool EndX = false;
	vector <MapCoordinate> TexturedCoords;

	if (End.GetPositionX() > Begin.GetPositionX()) Direction = Movement::RIGHT;
	else Direction = Movement::LEFT;


	// Adjusting the x transition 
	// so that not conflicts occur
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

				BeginX = false;
			}
			// The end of the x-movement
			// DIRECTION MATTERS
			else if (EndX)
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

				EndX = false;
			}
			// Adding to the cells above and below this current cell
			// Does not matter with the direction of the x corridor movement
			else
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
			MovementX = false;
		}
	}
	return false;
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
			if (Right) TextureCoords.push_back(MapCoordinate(X, Y));

			return;
		}
		// Otherwise, check if the cell if a Wall_Top type, as we will not
		// add a texture here
		MapCell* Cell = dynamic_cast<MapCell*>(m_Cells[X][Y]);

		if (Cell->GetCellType() != Cell::Wall_Top)
		{
			// Add the texture to the list to draw with the other textures here
			for (size_t i = 0; i < Textures.size(); i++)
			{
				Cell->AddRedTexture(Textures[i]);
			}
		}
	}
}