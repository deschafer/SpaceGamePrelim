#include "MapRoom.h"
#include "MapWall.h"
#include "MapManager.h"
#include "MapInactive.h"
#include "GenRoomComp.h"
#include "MapAssetManager.h"

#include <vector>
#include <map>
#include <iostream>
#include <chrono>

//#define NO_REFLECT

using namespace std;
using namespace std::chrono;

// Helper functions for the MapRoom::Generate()
vector<string> FindCorrectTile(Side CurrentSide, Direction CurrentDirection, bool LastBlock, 
	Direction NextDirection, Cell &CellType);
void SetFloorTiles(MapObject*** &Cells, int StartX, int StartY, int XMax, int YMax);
void MarkFloorTile(MapObject*** &Cells, int X, int Y, int XMax, int YMax);
void Reflect(MapObject*** &Cells, int Width, int Height);
void ReflectCandidates( int Width, int Height,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &TopFacingCandiates,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &RightFacingCandiates,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &BottomFacingCandiates,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &LeftFacingCandiates);
vector<string> ReflectTile(MapObject* Tile);

const static string WallCornerRight = "Wall_Corner_Right";
const static string WallCornerLeft = "Wall_Corner_Left";
const static string WallSideRight = "Wall_Side_Right";
const static string WallSideLeft = "Wall_Side_Left";
const static string WallBottom = "Wall_Bottom";
const static string Default = "Wall";
const static string WallTopGroup = "Wall_Top";
const static string FloorGroup = "Floors";
const static MapCoordinate ErrorCoord = MapCoordinate(-1, -1);

const static int MinCandidateSideLength = 2;
const static int ReflectionChance = 2;


MapRoom::MapRoom()
{}
MapRoom::~MapRoom()
{
	//TODO: Implement proper destruction here
}

//
// MapRoom()
// CTOR that should be used to create a MapRoom with a known type
//
MapRoom::MapRoom(std::string RoomType, int Width, int Height, Map* ParentMap) :
	m_CellWidth(MapManager::Instance()->GetCellWidth()),
	m_CellHeight(MapManager::Instance()->GetCellHeight()),
	m_CellSpawnRate(100),
	m_CountCells(0),
	m_CellSpawnGroupSize(10),		// An asset can spawn every tens cells if the chance is selected
	m_BorderingRoom(false),
	m_ParentMap(ParentMap)
{
	// Getting the properties assoc with this room type
	m_Properties = RoomManager::Instance()->GetTypeDefinition(RoomType);
	if (m_Properties == nullptr) // Then some error occurred
	{
		cout << "Error -- RoomProperties received from GetTypeDef in MapRoom ctor was nullptr\n";
		abort();
	}

	m_Width = Width;
	m_Height = Height;
	m_RoomType = RoomType;

	// TODO: Need to check with the minimum sizes of the maproom here

	// Generating the array for this room
	m_Cells = new MapObject**[m_Width];
	m_Assets = new MapAsset**[m_Width];
	m_Doorways = new bool**[m_Width];
	for (int i = 0; i < m_Width; i++)
	{
		m_Cells[i] = new MapObject*[m_Height];
		m_Assets[i] = new MapAsset*[m_Height];
		m_Doorways[i] = new bool*[m_Height];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_Cells[i][j] = nullptr;
			m_Assets[i][j] = nullptr;
			m_Doorways[i][j] = nullptr;
		}
	} 
}

//
// MapRoom()
//
//
MapRoom::MapRoom(std::string RoomType, int Width, int Height, Map* ParentMap, std::vector<unsigned> AssetLists) :
	MapRoom(RoomType, Width, Height, ParentMap)
{
	for (size_t i = 0; i < AssetLists.size(); i++)
	{
		m_AssetListIDs.push_back(AssetLists[i]);
	}
}

//
// MapRoom()
// CTOR that should be used to create a MapRoom with an unknown type
// NOTE: May not be staying in final version due to min room size complications
//
MapRoom::MapRoom(int Width, int Height)
{
	// Getting the properties assoc with this room type
	m_Properties = RoomManager::Instance()->GetRandomTypeDefinition(m_RoomType);
	if (m_Properties == nullptr) // Then some error occurred
	{
		cout << "Error -- RoomProperties received from GetTypeDef in MapRoom ctor was nullptr\n";
		abort();
	}

#ifdef _DEBUG
	cout << "Map " << m_RoomType << " Loaded\n";
#endif // _DEBUG

	m_Width = Width;
	m_Height = Height;

	// Need to check with the minimum sizes of the maproom here

	// Generating the array for this room
	m_Cells = new MapObject**[m_Width];
	for (int i = 0; i < m_Width; i++)
	{
		m_Cells[i] = new MapObject*[m_Height];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_Cells[i][j] = nullptr;
		}
	}
}

//
// GetCell()
// Gets the cell from m_Cells specefied by position
// X,Y. If not within the bounds of m_Cells,
// it will return nullptr, so check the return.
//
MapObject* MapRoom::GetCell(int X, int Y)
{
	if (X < m_Width && Y < m_Height)
	{
		return m_Cells[X][Y];
	}
	else
	{
		return nullptr;
	}
}

MapAsset * MapRoom::GetAsset(int X, int Y)
{
	if (X < m_Width && Y < m_Height)
	{
		return m_Assets[X][Y];
	}
	else
	{
		return nullptr;
	}
}

//
// Generate()
// Room generation function, generates the room based on the width and height 
// parameters based off of the room def. of RoomType stored in RoomManager.
// Stores the cells of this room in its own array.
// 
// The algorithm starts at the top, works to the east, folowing the definition, and.
// goes cell by cell, constructing the room. It goes east(Top)->south(Right)->west(Bottom)->north(Left). 
// Each of these greater sides have smaller, specefic sides that are defined by the room definition.
//
void MapRoom::Generate()
{
	RoomProperties* Properties = m_Properties;

	// Check if the definition was found prior to generating
	if (Properties == nullptr)
	{
		assert("No room def found");
		abort();
	}

	// These indicate how extreme a room's length of inner sides will appear,
	// definition dependent
	float ParamInnerSizeY = Properties->m_InnerSizeX;
	float ParamInnerSizeX = Properties->m_InnerSizeY;
	float Variance;

	// Numerics
	int CellWidth = MapManager::Instance()->GetCellWidth();
	int CellHeight = MapManager::Instance()->GetCellHeight();
	int StartX = 0;							// Starting Offset
	int StartY = 0;							// Starting Offset
	int EffectHeight = m_Height - 1;		// Actual height used in the alg
	int EffectWidth = m_Width - 1;			// Actual width used in the alg
	int MaxHeightX = StartX;				// Max values
	int MaxHeightY = StartY + m_Height;		// ..
	int MaxWidthX = StartX + m_Width;		// ..
	int MaxWidthY = StartY;					// ..
	int DrawCount = 0;						// A count of drawn/created cells in the second stage
	int Count = 0;							// A genertic count to keep track of pos. in SideDef
	int CountRecord = 0;					// A record of the count at the top of the cycle
	int TempX = StartX;						// Used to contain current XPos of drawn cell
	int TempY = StartY;						// Used to contain current YPos of drawn cell
	int CurrentLengthQuota = 0;				// # of cells needed to be drawn
	int HorizontalDeficit = 0;				// Offset left over from last side, it decreases the effective dimensions
	int VerticalDeficit = 0;				// Offset left over from last side, it decreases the effective dimensions

	const int MinRandSize = 2;

	// Flags
	bool Test = 0;				
	bool SizingComplete = false;
	bool LastSide = false;					
	bool StaticSideFlag = false;			// Indicates usage of static sides
	bool DynamicRandomFlag = false;
	bool StaticRandomHorizontal = false;
	bool StaticRandomVertical = false;
	bool complete = false;					// generic, used througout

	// Generating-stage helper variables
	Direction CurrentDirection = Direction::EAST;	// Current direction used in stage 1 and stage 2
	Direction CurrentDirectionTemp;					// Used to store the correspong direction for a side, stage 1
	Side CurrentSide = Side::TOP;					// Current side, set at the start, the top
	Cell CellType;

	vector<string> tempStr;	// Vector used to tempoarily store string sofr texture layers of cells

	// Room Def. Vectors
	vector<int> StaticSides;// Used for static side definitions
	vector<int> Sides;		// size = number of sides, each side has a length.
	vector<bool> SideDef;	// stores the Greater Side Definition from the room def
	vector<char> Turns;		// Stores the turns from the room def

	// Room gen. Vectors
	vector<int> TempSidesEast;	// Temporary vectors, used in the first stage
	vector<int> TempSidesSouth;
	vector<int> TempSidesWest;
	vector<int> TempSidesNorth;

	// Static Side Vectors
	vector<int> TempStaticEast;	// Temporary vectors, used in the first stage for static sides
	vector<int> TempStaticSouth;
	vector<int> TempStaticWest;
	vector<int> TempStaticNorth;

	pair<int, int> CurrentPair;	// used for adding coordinates

	// Set Static Side flag
	if (Properties->m_StaticSidesFlag)
	{
		StaticSideFlag = true;
	}
	else if (Properties->m_DynamicRandomFlag)
	{
		DynamicRandomFlag = true;
		// Copying static sides
		for (size_t i = 0; i < Properties->m_GreaterSideDefinition.size(); i++)
		{
			StaticSides.push_back(0);
		}
	}
	// Copying GreaterSide and Turn vectors (same size by def)
	for (size_t i = 0; i < Properties->m_GreaterSideDefinition.size(); i++)
	{
		SideDef.push_back(Properties->m_GreaterSideDefinition[i]);
		Turns.push_back(Properties->m_Turns[i]);
	}
	// Copying static sides
	for (size_t i = 0; i < Properties->m_StaticSides.size(); i++)
	{
		StaticSides.push_back(Properties->m_StaticSides[i]);
	}

	Variance = Properties->m_Variation;

	// Determining the side lengths for each of the 
	// sides in this defined room.
	// Start STAGE 1
	while (!SizingComplete)
	{
		CurrentDirectionTemp = CorrespondingDirection(CurrentSide);
		CountRecord = Count;

		// Adding empty sides to the record, these serve as a place holder that
		// will be dealt with later.
		do
		{
			// Testing if this side is significant
			Test = SideDef[Count];
			if (CurrentDirectionTemp == Direction::EAST && CurrentSide != Side::BOTTOM)
			{
				TempSidesEast.push_back(0 - (TempSidesSouth.size() + TempSidesNorth.size())); // placeholder for a side to be set later
			}
			else if (CurrentDirectionTemp == Direction::SOUTH && CurrentSide != Side::LEFT)
			{
				TempSidesSouth.push_back(0 - (TempSidesWest.size() + TempSidesEast.size())); // placeholder for a side to be set later
			}
			else if (CurrentDirectionTemp == Direction::WEST && CurrentSide != Side::TOP)
			{
				TempSidesWest.push_back(0 - (TempSidesSouth.size() + TempSidesNorth.size())); // placeholder for a side to be set later
			}
			else if (CurrentDirectionTemp == Direction::NORTH && CurrentSide != Side::RIGHT)
			{
				TempSidesNorth.push_back(0 - (TempSidesWest.size() + TempSidesEast.size())); // placeholder for a side to be set later
			}

			// Get the new direction
			CurrentDirectionTemp = Turn(CurrentDirectionTemp, Turns[Count]);
			// Increment a count
			Count++;
		} while (Test != true);


		// Deal with static sides here
		// A mag of static sides
		// and count for each direction
		if (StaticSideFlag)
		{
			Direction TempDirection = CorrespondingDirection(CurrentSide);
			int TempCount = CountRecord;
			int Index = TempCount;
			int StaticWestMag = 0;
			int StaticEastMag = 0;
			int StaticNorthMag = 0;
			int StaticSouthMag = 0;
			do
			{
				Test = SideDef[TempCount];
				// Setting the east temp queue for the static sides
				if (TempDirection == Direction::EAST && CurrentSide != Side::BOTTOM)
				{
					TempStaticEast.push_back(StaticSides[Index]);
					StaticEastMag += StaticSides[Index];
				}
				// Setting the south temp queue for the static sides
				else if (TempDirection == Direction::SOUTH && CurrentSide != Side::LEFT)
				{
					if (StaticSides[Index] != 0)
					{
						TempStaticSouth.push_back(StaticSides[Index]);
						StaticSouthMag += StaticSides[Index];
					}
				}
				// Setting the west temp queue for the static sides
				else if (TempDirection == Direction::WEST && CurrentSide != Side::TOP)
				{
					if (StaticSides[Index] != 0)
					{
						TempStaticWest.push_back(StaticSides[Index]);
						StaticWestMag += StaticSides[Index];
					}
				}
				// Setting the north temp queue for the static sides
				else if (TempDirection == Direction::NORTH && CurrentSide != Side::RIGHT)
				{
					if (StaticSides[Index] != 0)
					{
						TempStaticNorth.push_back(StaticSides[Index]);
						StaticNorthMag += StaticSides[Index];
					}
				}

				// Get the new direction
				TempDirection = Turn(TempDirection, Turns[TempCount]);
				// Increment a count
				TempCount++;
				Index++;
			} while (Test != true);
		}
		// Start of dynamic generation for sides
		else if (DynamicRandomFlag)
		{

			Direction TempDirection = CorrespondingDirection(CurrentSide);	
			int TempCount = CountRecord;	// Beginning count for looking at the temp vectors
			int Index = TempCount;			// ..

			int Space = ((int)(EffectWidth * Variance) == 0) ? 1 : (int)(EffectWidth * Variance);

			int HorizontalMagnitude = (TempSidesEast.size() + TempSidesWest.size()) - 1;
			int VerticalMagnitude = (TempSidesNorth.size() + TempSidesSouth.size()) - 1;

			// If the current side is a horizontal side, then the following will add variety to its up/down 
			// side lengths
			if ((HorizontalMagnitude > 0) &&
				(SideVertical(CurrentSide)))
			{
				int InwardMovement = 0; // Keep track of movement so we stay within the bounds

				do
				{
					Test = SideDef[TempCount];	// Get the next GSideDef

					// Setting the east temp queue for the static sides
					if (TempDirection == Direction::EAST && CurrentSide != Side::BOTTOM)
					{
						// Calc a random integer
						int random = (rand() % ((Space == 0) ? 1 : Space) / 2) + 1;
						
						// If this direction moves inwards,
						// make note of this
						if (CurrentSide == Side::LEFT)
						{
							StaticSides[Index] += random;
							TempStaticEast.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;
						}
						else
						{
							// Otherwise generate based on movement
							random = (rand() % InwardMovement) + 1;

							StaticSides[Index] += random;
							TempStaticEast.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;
						}
					}
					// Setting the west temp queue for the static sides
					else if (TempDirection == Direction::WEST && CurrentSide != Side::TOP)
					{

						int random = (rand() % ((Space == 0) ? 1 : Space) / 2) + 1;

						// If this direction moves inwards,
						// make note of this
						if (CurrentSide == Side::RIGHT)
						{
							StaticSides[Index] += random;
							TempStaticWest.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;

						}
						else
						{
							random = (rand() % InwardMovement) + 1;
							StaticSides[Index] += random;
							TempStaticWest.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;

						}
					}
					// Get the new direction
					TempDirection = Turn(TempDirection, Turns[TempCount]);
					// Increment a count
					TempCount++;
					Index++;

				} while (Test != true);
				// We have made static sides, so set it true
				StaticSideFlag = true;
			}
			// If the current side is a verticle side, then the following will add variety to its left/right 
			// side lengths
			else if ((VerticalMagnitude > 0) &&
				(SideHorizontal(CurrentSide)))
			{
				int InwardMovement = 0;	// Keeps track of movement inwards to keep within bounds

				do
				{
					Test = SideDef[TempCount];

					// Setting the east temp queue for the static sides
					if (TempDirection == Direction::NORTH && CurrentSide != Side::RIGHT)
					{
						int random = (rand() % ((Space == 0) ? 1 : Space) / 2) + 1;

						// If this movement is inwards
						if (CurrentSide == Side::BOTTOM)
						{
							StaticSides[Index] += random;
							TempStaticNorth.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;
						}
						else
						{
							random = (rand() % InwardMovement) + 1;

							StaticSides[Index] += random;
							TempStaticNorth.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;
						}
					}
					// Setting the west temp queue for the static sides
					else if (TempDirection == Direction::SOUTH && CurrentSide != Side::LEFT)
					{

						int random = (rand() % ((Space == 0) ? 1: Space) / 2) + 1;

						// If this movement is inwards
						if (CurrentSide == Side::TOP)
						{
							StaticSides[Index] += random;
							TempStaticSouth.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;

						}
						else
						{
							random = (rand() % InwardMovement) + 1;
							StaticSides[Index] += random;
							TempStaticSouth.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;
						}
					}
					// Get the new direction
					TempDirection = Turn(TempDirection, Turns[TempCount]);
					// Increment a count
					TempCount++;
					Index++;
				} while (Test != true);

				StaticSideFlag = true;
			}
		}
		
		// Then we deal with the empty, placeholder sides
		bool TempComplete = false;
		int X_Size = 0;
		int Y_Size = 0;

		// Determining an actual size for these sides
		while (!TempComplete)
		{
			X_Size = TempSidesEast.size();

			// first deal with x positions
			for (int i = 0; i < X_Size; i++)
			{
				if (SideVertical(CurrentSide))
				{
					// calculating integer division
					TempSidesEast[i] = static_cast<int>((EffectWidth * ParamInnerSizeX) / (X_Size)); // all set to equal size for now
				}
				else
				{
					TempSidesEast[i] = (EffectWidth) / X_Size - HorizontalDeficit; // all set to equal size for now
				}
			}

			// Resetting sizes
			if (SideHorizontal(CurrentSide))
			{
				int EastMag = 0;
				// Getting a magnitude for the total length of the sides generated.
				// Due to integer division and truncation, the sides may have to be resized so 
				// that they are guaranteed to fit the defining rectangle.
				for (size_t i = 0; i < TempSidesEast.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticEast.size() && TempStaticEast[i] != 0)
					{
						EastMag += TempStaticEast[i];
					}
					else
					{
						EastMag += TempSidesEast[i];
					}
				}
				// Adding a horizontal defecit if it exists
				EastMag += HorizontalDeficit;
				// If the magnitude is less than the total width
				// NOTE: This is most common 
				if (EastMag < EffectWidth)
				{
					int difference = EffectWidth - EastMag;
					size_t size = TempSidesEast.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticEast.size() && TempStaticEast[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesEast[i]++;
							difference--;
						}
						// If this round completed, yet a difference still exists, reset
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
				// If the magnitude is larger
				// NOTE: This is most likely caused by a static side definition
				else if (EastMag > EffectWidth)
				{
					int difference = EastMag - EffectWidth;
					size_t size = TempSidesEast.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticEast.size() && TempStaticEast[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesEast[i]--;
							difference--;
						}
						// If this round completed, yet a difference still exists, reset
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}

			X_Size = TempSidesWest.size();

			// first deal with x positions
			for (int i = 0; i < X_Size; i++)
			{
				if (SideVertical(CurrentSide))
				{
					TempSidesWest[i] = static_cast<int>((EffectWidth * ParamInnerSizeX) / (X_Size)); // all set to equal size for now
				}
				else
				{
					TempSidesWest[i] = EffectWidth / X_Size - HorizontalDeficit; // all set to equal size for now
				}
			}

			// Resetting sizes
			if (SideHorizontal(CurrentSide))
			{
				int WestMag = 0;
				for (size_t i = 0; i < TempSidesWest.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticWest.size() && TempStaticWest[i] != 0)
					{
						WestMag += TempStaticWest[i];
					}
					else
					{
						WestMag += TempSidesWest[i];
					}
				}
				WestMag += HorizontalDeficit;
				if (WestMag < EffectWidth)
				{
					int difference = EffectWidth - WestMag;
					size_t size = TempSidesWest.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticWest.size() && TempStaticWest[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesWest[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
				else if (WestMag > EffectWidth)
				{
					int difference = WestMag - EffectWidth;
					size_t size = TempSidesWest.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticWest.size() && TempStaticWest[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesWest[i]--;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}


			Y_Size = TempSidesSouth.size();

			for (int i = 0; i < Y_Size; i++)
			{
				if (SideHorizontal(CurrentSide))
				{
					TempSidesSouth[i] = static_cast<int>((EffectHeight * ParamInnerSizeY) / (Y_Size)); // all set to equal size for now
				}
				else
				{
					TempSidesSouth[i] = EffectHeight / Y_Size - VerticalDeficit; // all set to equal size for now
				}
			}


			// Reset sizes
			// first get a magnitude
			if (SideVertical(CurrentSide))
			{
				int SouthMag = 0;
				for (size_t i = 0; i < TempSidesSouth.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticSouth.size() && TempStaticSouth[i] != 0)
					{
						SouthMag += TempStaticSouth[i];
					}
					else
					{
						SouthMag += TempSidesSouth[i];
					}
				}
				SouthMag += VerticalDeficit;
				if (SouthMag < EffectHeight)
				{
					int difference = EffectHeight - SouthMag;
					size_t size = TempSidesSouth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticSouth.size() && TempStaticSouth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesSouth[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = -1;
					}
				}
				else if (SouthMag > EffectHeight)
				{
					int difference = SouthMag - EffectHeight;
					size_t size = TempSidesSouth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticSouth.size() && TempStaticSouth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesSouth[i]--;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}



			Y_Size = TempSidesNorth.size();

			for (int i = 0; i < Y_Size; i++)
			{
				if (SideHorizontal(CurrentSide))
				{
					TempSidesNorth[i] = static_cast<int>((EffectHeight * ParamInnerSizeY) / (Y_Size)); // all set to equal size for now
				}
				else
				{
					TempSidesNorth[i] = EffectHeight / Y_Size - VerticalDeficit; // all set to equal size for now
				}
			}

			// Resetting sizes
			if (SideVertical(CurrentSide))
			{
				int NorthMag = 0;
				for (size_t i = 0; i < TempSidesNorth.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticNorth.size() && TempStaticNorth[i] != 0)
					{
						NorthMag += TempStaticNorth[i];
					}
					else
					{
						NorthMag += TempSidesNorth[i];
					}
				}
				NorthMag += VerticalDeficit;
				if (NorthMag < EffectHeight)
				{
					int difference = EffectHeight - NorthMag;
					size_t size = TempSidesNorth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticNorth.size() && TempStaticNorth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesNorth[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
				else if (NorthMag > EffectHeight)
				{
					int difference = NorthMag - EffectHeight;
					size_t size = TempSidesNorth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticNorth.size() && TempStaticNorth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesNorth[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}

			TempComplete = true;
		}

		// Resetting defecits from last cycle
		VerticalDeficit = 0;
		HorizontalDeficit = 0;
		// Calculating side-specefic defecits

		// Horizontal defecit, used in the verticle sides ONLY
		if ((TempSidesEast.size() > TempSidesWest.size() ||
			TempSidesEast.size() < TempSidesWest.size()) &&
			(SideVertical(CurrentSide) && !StaticSideFlag) ||
			(SideVertical(CurrentSide) && StaticSideFlag))
		{
			int EastMagnitude = 0;
			int WestMagnitude = 0;

			for (size_t i = 0; i < TempSidesWest.size(); i++)
			{
				if (StaticSideFlag && (i < TempStaticWest.size()) && TempStaticWest[i] != 0)
				{
					WestMagnitude += TempStaticWest[i];
				}
				else
				{
					WestMagnitude += TempSidesWest[i];
				}
			}

			for (size_t i = 0; i < TempSidesEast.size(); i++)
			{
				// If a static side is present, use that size over the dynamic side
				if (StaticSideFlag && (i < TempStaticEast.size()) && TempStaticEast[i] != 0)
				{
					EastMagnitude += TempStaticEast[i];
				}
				else
				{
					EastMagnitude += TempSidesEast[i];
				}
			}
			HorizontalDeficit = abs(static_cast<int>(EastMagnitude - WestMagnitude));
		}

		// If the an overall movement in the y axis was produced in 
		// either the top or bottom sides, the next side will need to know 
		// about this.
		if ((TempSidesNorth.size() > TempSidesSouth.size() ||
			TempSidesNorth.size() < TempSidesSouth.size()) &&
			(SideHorizontal(CurrentSide) && !StaticSideFlag) ||
			(SideHorizontal(CurrentSide) && StaticSideFlag))
		{

			int NorthMagnitude = 0;
			int SouthMagnitude = 0;

			for (size_t i = 0; i < TempSidesNorth.size(); i++)
			{
				
				if (StaticSideFlag && (i < TempStaticNorth.size()) && TempStaticNorth[i] != 0)
				{
					NorthMagnitude += TempStaticNorth[i];
				}
				else
				{
					NorthMagnitude += TempSidesNorth[i];
				}
			}

			for (size_t i = 0; i < TempSidesSouth.size(); i++)
			{
				// If a static side is present, use that size over the dynamic side
				if (StaticSideFlag && (i < TempStaticSouth.size()) && TempStaticSouth[i] != 0)
				{
					SouthMagnitude += TempStaticSouth[i];
				}
				else
				{
					SouthMagnitude += TempSidesSouth[i];
				}
			}

			VerticalDeficit = abs(static_cast<int>(NorthMagnitude - SouthMagnitude));
		}

		int CountWest = 0;
		int CountEast = 0;
		int CountNorth = 0;
		int CountSouth = 0;

		CurrentDirectionTemp = CorrespondingDirection(CurrentSide);

		// Adding the sides to the vector to be drawn in a the same
		// order that they were initially set at the beginning
		Count = CountRecord;
		do
		{
			// Test if side is significant
			Test = SideDef[Count];
			if (CurrentDirectionTemp == Direction::EAST && CurrentSide != Side::BOTTOM)
			{
				Sides.push_back(TempSidesEast[CountEast]);
				CountEast++;
			}
			else if (CurrentDirectionTemp == Direction::SOUTH && CurrentSide != Side::LEFT)
			{
				Sides.push_back(TempSidesSouth[CountSouth]); // placeholder for a side to be set later
				CountSouth++;
			}
			else if (CurrentDirectionTemp == Direction::WEST && CurrentSide != Side::TOP)
			{
				Sides.push_back(TempSidesWest[CountWest]); // placeholder for a side to be set later
				CountWest++;
			}
			else if (CurrentDirectionTemp == Direction::NORTH && CurrentSide != Side::RIGHT)
			{
				Sides.push_back(TempSidesNorth[CountNorth]); // placeholder for a side to be set later
				CountNorth++;
			}

			// Gets new direction, increments
			CurrentDirectionTemp = Turn(CurrentDirectionTemp, Turns[Count]);
			Count++;

		} while (Test != true);

		// Testing if the next side is the complete side
		if ((CurrentSide = NextSide(CurrentSide)) == Side::COMPL)
		{
			SizingComplete = true;
		}

		// Clear all temporary storage for cells
		TempSidesEast.clear();
		TempSidesSouth.clear();
		TempSidesWest.clear();
		TempSidesNorth.clear();
		TempStaticEast.clear();
		TempStaticSouth.clear();
		TempStaticWest.clear();
		TempStaticNorth.clear();
	}

	CurrentSide = Side::TOP;
	int SideCount = -1;
	// STAGE 1 END
	// ------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------
	// STAGE 2 START
	// Now the next step, drawing in the set sides. This goes through the side vector, and draws them 
	// with their desired length and in the direction indicated by the turns vector.

	MapCoordinate CandidateStart;
	MapCoordinate CandidateEnd;

	while (!complete)
	{
		// keeping a count of drawn cells
		DrawCount++;

		// Get a new start point 
		if (CurrentLengthQuota == 0)
		{
			CandidateStart = MapCoordinate(TempX, TempY);
		}

		// Getting the appropriate side for this definition
		// If static sides, and not finished, and the done drawing this spec side
		if (StaticSideFlag && !Sides.empty() && CurrentLengthQuota <= 0)
		{
			// Checking that the definition is correct first
			if (StaticSides.empty() && Sides.size() > 1)
			{
			#ifdef _DEBUG
				cout << "Room defined incorrectly -- static sides count incorrect\n";
			#endif // _DEBUG
				abort(); // Cannot proceed
			}

			// If the static sides is empty, get from the other sides
			if ((CurrentLengthQuota = StaticSides.front()) == 0)
			{
				CurrentLengthQuota = Sides.front();
			}
			// Still have to remove from other queue
			StaticSides.erase(StaticSides.begin());
			Sides.erase(Sides.begin());
			// If last, set approp.
			if (Sides.empty()) LastSide = true;
		}
		// If no static sides, then get the next side when ready
		else if (!Sides.empty() && CurrentLengthQuota <= 0)
		{
			CurrentLengthQuota = Sides.front();
			Sides.erase(Sides.begin());
			if (Sides.empty()) LastSide = true;
		}
		// Complete case
		else if (Sides.empty() && CurrentLengthQuota <= 0)
		{
			// If the ending cell is not at the beginning X Location
			if (TempX > 0)
			{
				// Set the new start point
				StartX = TempX;
				int Start = 0;
				int Distance = 0;
				// Iterate through all the x cells in the first row until we 
				// reach where the last printed cell is located
				for (MapObject* Current; Start < (TempX); Start++, Distance++)
				{
					Current = m_Cells[Start][0];
					
					if(Current != nullptr) delete Current;
					m_Cells[Start][0] = nullptr;
				}
				
				for (size_t i = 0; i < m_TopFacingCandiates.size(); i++)
				{
					if (m_TopFacingCandiates[i].first.GetPositionX() < Distance)
					{
						m_TopFacingCandiates[i].first = MapCoordinate(Distance, m_TopFacingCandiates[i].first.GetPositionY());
					}
				}

				// Finally, finish by setting the appropriate texture here
				tempStr.push_back(WallSideRight);

				m_Cells[Start][0] = new MapWall(Rect(0,0,m_CellWidth, m_CellHeight), 
					MapManager::Instance()->GetParentScene(), 
					tempStr,
					MapCoordinate(TempX, TempY), 
					Rect(0, 0, m_CellWidth, m_CellHeight), 
					Cell::Wall_Right);

				// Then update the cell counter
				m_CountCells++;
			}

			complete = true;
			break;
		}

		// If the current point is the starting point
		if ((CurrentPair = make_pair(TempX, TempY)) == make_pair(StartX, StartY) && Sides.empty())
		{
			complete = true;
			break;
		}

		// Determines the movement based on direction
		if (CurrentDirection == Direction::EAST)
		{
			TempX++;
		}
		else if (CurrentDirection == Direction::SOUTH)
		{
			TempY++;
		}
		else if (CurrentDirection == Direction::WEST)
		{
			TempX--;
		}
		else if (CurrentDirection == Direction::NORTH)
		{
			TempY--;
		}

		// Adding the current coordinates to the record
		CurrentPair.swap(make_pair(TempX, TempY));

		// Adding to the array
		if (TempX >= 0 && TempY >= 0 && TempX < m_Width && TempY < m_Height)
		{
			// Getting the correct texture
			tempStr = FindCorrectTile(
				CurrentSide, 
				CurrentDirection,
				(CurrentLengthQuota == 1),
				Turn(CurrentDirection, Turns.front()),
				CellType);

			// Finally, creating a new map cell representing the outer walls of this room
			m_Cells[TempX][TempY] = new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
				MapManager::Instance()->GetParentScene(), 
				tempStr, 
				MapCoordinate(TempX, TempY), 
				Rect(0, 0, m_CellWidth, m_CellHeight), 
				CellType);

			// Update the cells counter
			m_CountCells++;
		}

		// Decrementing the length
		CurrentLengthQuota--;

		// Complete case
		if (CurrentLengthQuota == 0 && !Turns.empty())
		{
			
			CurrentDirection = Turn(CurrentDirection, Turns.front());
			Turns.erase(Turns.begin());

		}
		// Keeping track of the current side
		if (CurrentLengthQuota == 0)
		{

			// First adding this completed side to a candidate if it
			// is a facing side
			CandidateEnd = MapCoordinate(TempX, TempY);

			AddCandidate(CurrentSide, CandidateStart, CandidateEnd);

			SideCount++;
			// Greater Side Definition
			if ((SideDef.size() > (size_t)SideCount) && (SideDef[SideCount] == 1))
			{
				CurrentSide = NextSide(CurrentSide);
			}

		}
		// Clear the textures for the last cell
		tempStr.clear();
	}
	

#ifndef NO_REFLECT
	// Reflection of the current room
	if ((rand() % ReflectionChance) == 0)
	{
		// Reflects the room
		Reflect(m_Cells, m_Width, m_Height);

		// Setting a new start position
		for (int i = 0; i < m_Width; i++)
		{
			if (m_Cells[i][0] != nullptr)
			{
				StartX = i;
				break;
			}
		}

		ReflectCandidates(m_Width, m_Height, m_TopFacingCandiates, m_RightFacingCandiates, m_BottomFacingCandiates, m_LeftFacingCandiates);

		int number = 123;
	}
#endif // !NO_REFLECT

	// Sets the floor tiles, a recursive function
	SetFloorTiles(m_Cells, StartX, StartY, m_Width, m_Height);

	// Cleaning all memory
	StaticSides.clear();
	Sides.clear();	
	SideDef.clear();
	Turns.clear();
	tempStr.clear();
}

//
// FindCorrectTile()
// Finds the next correct tile that appropriately fits the context of the wall
//
vector<string> FindCorrectTile(Side CurrentSide, Direction CurrentDirection, bool LastBlock, 
	Direction NextDirection, Cell& CellType)
{
	vector<string> Strings; // Temp vector for the textures for this cell

	// If this is the last block for this specefic side, then 
	// we need to prepare for the transition
	if (LastBlock)
	{
		if (CurrentDirection == Direction::EAST && NextDirection == Direction::SOUTH)
		{
			CellType = Cell::Wall_Left;
			Strings.push_back(WallSideLeft);
		}
		else if (CurrentDirection == Direction::SOUTH && NextDirection == Direction::WEST)
		{
			CellType = Cell::Wall_Corner_Right;
			Strings.push_back(WallCornerRight);
		}
		else if (CurrentDirection == Direction::WEST && NextDirection == Direction::NORTH)
		{
			CellType = Cell::Wall_Corner_Left;
			Strings.push_back(WallCornerLeft);
		}
		else if (CurrentDirection == Direction::WEST && NextDirection == Direction::SOUTH)
		{
			CellType = Cell::Wall_Corner_Right_Bottom;
			Strings.push_back(WallBottom);
			Strings.push_back(WallSideLeft);
			Strings.push_back(WallCornerRight);
		}
		else if (CurrentDirection == Direction::NORTH && NextDirection == Direction::EAST)
		{
			CellType = Cell::Wall_Right;
			Strings.push_back(WallSideRight);
		}
		else if (CurrentDirection == Direction::NORTH && NextDirection == Direction::WEST)
		{
			CellType = Cell::Wall_Corner_Left_Bottom;
			Strings.push_back(WallBottom);
			Strings.push_back(WallSideRight);
			Strings.push_back(WallCornerLeft);
		}
		else
		{
			CellType = Cell::Wall_Top;
			Strings.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
		}
	}
	// For a normal block in a side
	else
	{
		if (CurrentDirection == Direction::SOUTH)
		{
			CellType = Cell::Wall_Left;
			Strings.push_back(WallSideLeft);
		}
		else if (CurrentDirection == Direction::NORTH)
		{
			CellType = Cell::Wall_Right;
			Strings.push_back(WallSideRight);
		}
		else if (CurrentDirection == Direction::WEST)
		{
			CellType = Cell::Wall_Bottom;
			Strings.push_back(WallBottom);
		}
		else
		{
			CellType = Cell::Wall_Top;
			Strings.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
		}
	}

	// Returns textures
	return Strings;
}

//
// SetFloorTiles()
// Wrapper around recursive alg to mark all floor tiles
//
void SetFloorTiles(MapObject*** &Cells, int StartX, int StartY, int XMax, int YMax)
{
	MarkFloorTile(Cells, StartX + 1, StartY + 1, XMax, YMax);
}

//
// MarkFloorTile()
// A simple recursive search that marks all cells within the walls as floors
//
void MarkFloorTile(MapObject*** &Cells, int X, int Y, int XMax, int YMax)
{
	// Only need the height and width once
	static int Width = MapManager::Instance()->GetCellWidth();
	static int Height = MapManager::Instance()->GetCellHeight();

	vector<string> Strings;

	// IF current is nullptr, and not already made a wall
	if (Cells[X][Y] == nullptr)
	{
		Strings.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));
		Cells[X][Y] = new MapInactive(Rect(0, 0, MapManager::ActiveCellsWidth, MapManager::ActiveCellsHeight),
			MapManager::Instance()->GetParentScene(), Strings, MapCoordinate(X * Width, Y * Height), Rect(0, 0, Width, Height), Cell::Floor);
	}
	// One cell to the east
	if ((X + 1) < XMax &&
		Y < YMax &&
		Cells[X + 1][Y] == nullptr)
	{
		MarkFloorTile(Cells, X + 1, Y, XMax, YMax);
	}
	// One cell to the South
	if (X < XMax &&
		(Y + 1) < YMax &&
		Cells[X][Y + 1] == nullptr)
	{
		MarkFloorTile(Cells, X, Y + 1, XMax, YMax);
	}
	// One cell to the north
	if (X < XMax &&
		(Y - 1) > 0 &&
		Cells[X][Y - 1] == nullptr)
	{
		MarkFloorTile(Cells, X, Y - 1, XMax, YMax);
	}
	// One cell to the west
	if ((X - 1) >= 0 &&
		(Y) < YMax &&
		Cells[X - 1][Y] == nullptr)
	{
		MarkFloorTile(Cells, X - 1, Y, XMax, YMax);
	}
}

//
// Reflect()
//
//
void Reflect(MapObject*** &Cells, int Width, int Height)
{
	MapObject* temp;
	
	MapObject** Temporary = new MapObject*[Width];

	for (int y = 0; y < Height; y++)
	{

		for (int i = 0; i < Width; i++)
		{
			Temporary[i] = Cells[i][y];
		}

		for (int xBeg = 0, xEnd = (Width - 1); (xBeg < Width) && (xEnd >= 0); xBeg++, xEnd--)
		{
			temp = Cells[xBeg][y];
			Cells[xBeg][y] = Temporary[xEnd];

			if (Cells[xBeg][y] != nullptr)
			{
				vector<string> NewTextures = ReflectTile(Cells[xBeg][y]);
			}
		}
	}
}

//
// ReflectTile()
// Responsible for reflecting the textures of this MapCellobject
//
vector<string> ReflectTile(MapObject* Tile)
{

	MapCell* CellTile = dynamic_cast<MapCell*>(Tile);

	vector<string> *Textures = CellTile->ReturnRedTextures();
	Cell CellType = CellTile->GetCellType();
	
	switch (CellType)
	{
	case Cell::Floor:
		break;
	case Cell::Wall_Top:
		break;
	case Cell::Wall_Bottom:
		break;
	case Cell::Wall_Left:
		Textures->clear();
		Textures->push_back(WallSideRight);
		CellType = Cell::Wall_Right;
		break;
	case Cell::Wall_Right:
		Textures->clear();
		Textures->push_back(WallSideLeft);
		CellType = Cell::Wall_Left;
		break;
	case Cell::Wall_Corner_Left:
		Textures->clear();
		Textures->push_back(WallCornerRight);
		CellType = Cell::Wall_Corner_Right;
		break;
	case Cell::Wall_Corner_Right:
		Textures->clear();
		Textures->push_back(WallCornerLeft);
		CellType = Cell::Wall_Corner_Left;
		break;
	case Cell::Wall_Corner_Left_Bottom:
		Textures->clear();
		Textures->push_back(WallBottom);
		Textures->push_back(WallSideLeft);
		Textures->push_back(WallCornerRight);
		CellType = Cell::Wall_Corner_Right_Bottom;
		break;
	case Cell::Wall_Corner_Right_Bottom:
		Textures->clear();
		Textures->push_back(WallBottom);
		Textures->push_back(WallSideRight);
		Textures->push_back(WallCornerLeft);
		CellType = Cell::Wall_Corner_Left_Bottom;
		break;
	case Cell::Default:
		break;
	default:
		break;
	}

	CellTile->ChangeRedTextures(*Textures);

	return* Textures;
}

//
// ReflectCandidates()
// Reflects the candidate sides used for corridor creation
//
void ReflectCandidates(
	int Width, int Height,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &TopFacingCandiates,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &RightFacingCandiates,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &BottomFacingCandiates,
	std::vector<std::pair<MapCoordinate, MapCoordinate>> &LeftFacingCandiates)
{
	int DistanceFirst = 0;
	int DistanceSecond = 0;
	int YPosition = 0;
	int XPosition = 0;
	Width--;
	Height--;

	// Left and right need to swap sides
	vector<pair<MapCoordinate, MapCoordinate>> TempFacingCandiates;
	for (size_t i = 0; i < LeftFacingCandiates.size(); i++)
	{
		TempFacingCandiates.push_back(LeftFacingCandiates[i]);
	}
	LeftFacingCandiates.clear();
	for (size_t i = 0; i < RightFacingCandiates.size(); i++)
	{
		LeftFacingCandiates.push_back(RightFacingCandiates[i]);
	}
	RightFacingCandiates.clear();
	for (size_t i = 0; i < TempFacingCandiates.size(); i++)
	{
		RightFacingCandiates.push_back(TempFacingCandiates[i]);
	}

	pair<MapCoordinate, MapCoordinate> *NewPair;

	for (size_t i = 0; i < TopFacingCandiates.size(); i++)
	{

		DistanceFirst = TopFacingCandiates[i].first.GetPositionX();
		DistanceSecond = TopFacingCandiates[i].second.GetPositionX();
		YPosition = TopFacingCandiates[i].first.GetPositionY();

		NewPair = new std::pair<MapCoordinate, MapCoordinate>(
			MapCoordinate(Width - DistanceFirst, YPosition), 
			MapCoordinate(Width - DistanceSecond, YPosition));

		TopFacingCandiates[i] = *NewPair;

	}

	for (size_t i = 0; i < RightFacingCandiates.size(); i++)
	{

		DistanceFirst = RightFacingCandiates[i].first.GetPositionX();
		DistanceSecond = RightFacingCandiates[i].second.GetPositionX();
		YPosition = RightFacingCandiates[i].first.GetPositionY();
		int YPosition2 = RightFacingCandiates[i].second.GetPositionY();

		NewPair = new std::pair<MapCoordinate, MapCoordinate>(
			MapCoordinate(Width - DistanceSecond, YPosition2),
			MapCoordinate(Width - DistanceFirst, YPosition));

		RightFacingCandiates[i] = *NewPair;

	}

	for (size_t i = 0; i < BottomFacingCandiates.size(); i++)
	{
		DistanceFirst = BottomFacingCandiates[i].first.GetPositionX();
		DistanceSecond = BottomFacingCandiates[i].second.GetPositionX();
		YPosition = BottomFacingCandiates[i].first.GetPositionY();

		NewPair = new std::pair<MapCoordinate, MapCoordinate>(
			MapCoordinate(Width - DistanceFirst, YPosition),
			MapCoordinate(Width - DistanceSecond, YPosition));

		BottomFacingCandiates[i] = *NewPair;
	}

	for (size_t i = 0; i < LeftFacingCandiates.size(); i++)
	{
		DistanceFirst = LeftFacingCandiates[i].first.GetPositionX();
		DistanceSecond = LeftFacingCandiates[i].second.GetPositionX();
		YPosition = LeftFacingCandiates[i].first.GetPositionY();
		int YPosition2 = LeftFacingCandiates[i].second.GetPositionY();

		NewPair = new std::pair<MapCoordinate, MapCoordinate>(
			MapCoordinate(Width - DistanceSecond, YPosition2),
			MapCoordinate(Width - DistanceFirst, YPosition));
		LeftFacingCandiates[i] = *NewPair;
	}
}

//
// AddCandidate()
// This function checks if the side defined by the points given is of
// a certain length, and if so adds it to its corresponding side
//
void MapRoom::AddCandidate(Side CurrentSide, MapCoordinate Start, MapCoordinate End)
{
	switch (CurrentSide)
	{
	case Side::TOP:

		// If this side is of a certain length
		if (End.GetPositionX() - Start.GetPositionX() >= MinCandidateSideLength)
		{
			m_TopFacingCandiates.push_back(pair<MapCoordinate, MapCoordinate>(Start, End));
		}

		break;
	case Side::RIGHT:

		// If this side is of a certain length
		if (End.GetPositionY() - Start.GetPositionY() >= MinCandidateSideLength)
		{
			m_RightFacingCandiates.push_back(pair<MapCoordinate, MapCoordinate>(Start, End));
		}

		break;
	case Side::BOTTOM:

		// If this side is of a certain length
		if (Start.GetPositionX() - End.GetPositionX() >= MinCandidateSideLength)
		{
			m_BottomFacingCandiates.push_back(pair<MapCoordinate, MapCoordinate>(Start, End));
		}

		break;
	case Side::LEFT:

		// If this side is of a certain length
		if (Start.GetPositionY() - End.GetPositionY() >= MinCandidateSideLength)
		{
			m_LeftFacingCandiates.push_back(pair<MapCoordinate, MapCoordinate>(Start, End));
		}

		break;
	default:
		break;
	}
}

//
// GetFacingFromSide()
// Gets a side that is facing outwards of the given side, and
// is a candidate for a corridor connection
//
std::pair<MapCoordinate, MapCoordinate>* MapRoom::GetFacingFromSide(Side side)
{
	switch (side)
	{
	case Side::TOP:
		if(!m_TopFacingCandiates.empty())
			return &m_TopFacingCandiates[rand() % m_TopFacingCandiates.size()];
		break;
	case Side::RIGHT:
		if (!m_RightFacingCandiates.empty())
			return &m_RightFacingCandiates[rand() % m_RightFacingCandiates.size()];
		break;
	case Side::BOTTOM:
		if (!m_BottomFacingCandiates.empty())
			return &m_BottomFacingCandiates[rand() % m_BottomFacingCandiates.size()];
		break;
	case Side::LEFT:
		if (!m_LeftFacingCandiates.empty())
			return &m_LeftFacingCandiates[rand() % m_LeftFacingCandiates.size()];
		break;
	default:
		break;
	}

	return nullptr;
}

//
// GetFacingFromSideIndexed()
// Returns an indexed side from the corresponding given side and index
// if it exists
//
pair<MapCoordinate, MapCoordinate>* MapRoom::GetFacingFromSideIndexed(Side side, int IndexI)
{
	size_t Index = (size_t)IndexI;

	switch (side)
	{
	case Side::TOP:
		if (!m_TopFacingCandiates.empty() && Index < m_TopFacingCandiates.size())
			return &m_TopFacingCandiates[Index];
		break;
	case Side::RIGHT:
		if (!m_RightFacingCandiates.empty() && Index < m_RightFacingCandiates.size())
			return &m_RightFacingCandiates[Index];
		break;
	case Side::BOTTOM:
		if (!m_BottomFacingCandiates.empty() && Index < m_BottomFacingCandiates.size())
			return &m_BottomFacingCandiates[Index];
		break;
	case Side::LEFT:
		if (!m_LeftFacingCandiates.empty() && Index < m_LeftFacingCandiates.size())
			return &m_LeftFacingCandiates[Index];
		break;
	default:
		break;
	}

	return nullptr;
}

//
// AddLinkedRoom()
// Adds LinkedRoom param to the approp. side given by side param
//
void MapRoom::AddLinkedRoom(Side side, MapRoom* LinkedRoom)
{
	switch (side)
	{
	case Side::TOP:
		m_TopLinkedRooms.push_back(LinkedRoom);
		return;
	case Side::RIGHT:
		m_RightLinkedRooms.push_back(LinkedRoom);
		return;
	case Side::BOTTOM:
		m_BottomLinkedRooms.push_back(LinkedRoom);
		return;
	case Side::LEFT:
		m_LeftLinkedRooms.push_back(LinkedRoom);
		return;
	default:
		break;
	}
}

//
//
//
//
bool MapRoom::ConnectedToRoom(Side side)
{
	switch (side)
	{
	case Side::TOP:
		return m_TopLinkedRooms.size() ? true : false;
		break;
	case Side::RIGHT:
		return m_RightLinkedRooms.size() ? true : false;
		break;
	case Side::BOTTOM:
		return m_BottomLinkedRooms.size() ? true : false;
		break;
	case Side::LEFT:
		return m_LeftLinkedRooms.size() ? true : false;
		break;
	default:
		break;
	}
	return false;
}

//
// PlaceAssets()
//
//
std::vector<MapAsset*> MapRoom::PlaceAssets()
{
	vector<MapAsset*> Assets;
	MapAsset* CurrentAsset = nullptr;

	// Generate by chance the asset that will be generated
		// larger rooms have more potential for assets
		// create a function/equ for this.

	// Then create this asset
	// Call this asset to place itself
		// pass in map cells, asset positions, and doorway positions
		// pass in boundary room status

	// If it suceeded, then we save its placement,
	// and save the asset so we can return it later
		// save the asset to this room's asset position array
		// and save it to the general asset array


	// If we cannot place the asset, then try again with 2 
	// additional and different assets
		// then stop and void this chance

	int MaxNumberAssets = (int)round(m_CountCells / m_CellSpawnGroupSize);	// the maximum number of assets that can be placed


	for (int i = 0; i < MaxNumberAssets; i++)
	{
		// For each of the potential assets, check by chance if it will be placed
		bool Chance = (rand() % 100) <= m_CellSpawnRate;

		// If we need to place an asset
		if (Chance)
		{
			CurrentAsset = PlaceAsset();
			if (CurrentAsset) 
			{
				Assets.push_back(CurrentAsset);
			}
			else
			{
				cout << "Potential Error: MapRoom ultimately failed placing an asset." << endl;
			}
		}
	}

	return Assets;
}

//
// PlaceAsset()
// Creates and attempts to place a single asset
// somewhere in this room
//
MapAsset* MapRoom::PlaceAsset()
{
	if (!m_AssetListIDs.size()) 
		return nullptr;

	// First select a list
	int Selection = rand() % m_AssetListIDs.size();
	MapCoordinate SelPosition;
	MapAsset* NewAsset = nullptr;
	int NumberTries = 3;
	int Try = 0;
	bool Retry = false;

	while (NewAsset == nullptr && Try < NumberTries)
	{
		// Choose an asset from this selected list
		if (NewAsset = MapAssetManager::Instance()->CreateAssetFromList(m_AssetListIDs[Selection]))
		{
			NewAsset->SetParentRoom(this);
			NewAsset->SetParentMap(m_ParentMap);

			// Is this a boundary room or not
			if (m_BorderingRoom)
			{
				SelPosition = NewAsset->PlaceAssetBorderingRoom((MapCell***)m_Cells, m_Assets);
			}
			else
			{
				SelPosition = NewAsset->PlaceAsset(m_Cells, m_Assets, m_Doorways);
			}

			// Check if the asset succeeded
			if (SelPosition == ErrorCoord)
			{
				// Then the placement failed, delete the asset we wanted to place
				// and set it as nullptr
				delete NewAsset;
				NewAsset = nullptr;
			}
			else
			{
				// Save the position of the asset by modifying the m_Assets array;

				for (int i = SelPosition.GetPositionX(), AssetWidth = NewAsset->GetIntegerWidth();
					i < m_Width && i < AssetWidth;
					i++)
				{
					for (int j = SelPosition.GetPositionY(), AssetHeight = NewAsset->GetIntegerHeight();
						j < m_Height && j < AssetHeight;
						j++)
					{
						// First check if there already is an asset here
						if (m_Assets[i][j])
						{
							cout << "MapAsset Error: Selected placement for map asset was already occupied" << endl;
						}
						else
						{
							m_Assets[i][j] = NewAsset;	// Save the location of this asset
						}
					}
				}
			}

			Try++;	// We have tried once to place an asset with this iteration
		}
	}
	return NewAsset;
}

//
// FindAssetPlacement()
//
//
MapCoordinate MapRoom::FindAssetPlacement(MapCoordinate Coord, MapAsset* Asset, int IntegerWidth, int IntegerHeight)
{

	MapCell* CurrentCell;

	// Check if the current cell is a floor
	// Also check the boundaries of the map

	bool Transition = (Coord.GetPositionX() + 1) == m_Width;
	MapCoordinate NextCell = MapCoordinate
	((Coord.GetPositionX() + 1) % m_Width,
		Coord.GetPositionY() + static_cast<int>(Transition));

	if (Coord.GetPositionY() >= m_Height) return ErrorCoord;

	if (Coord.GetPositionX() < m_Width &&
		Coord.GetPositionY() < m_Height &&
		!(CurrentCell = static_cast<MapCell*>(m_Cells[Coord.GetPositionX()][Coord.GetPositionY()]))->IsCollidableType())
	{
		// Then the current cell itself is not collidable

		// Now we can check if the base size of the asset is available

		for (int X = 0, Y = 0;
			(Coord.GetPositionX() + X < m_Width) &&
			(Coord.GetPositionY() + Y < m_Height) &&
			(X != IntegerWidth && Y != IntegerHeight);)
		{
			// Check the current cell
			if (static_cast<MapCell*>(m_Cells[Coord.GetPositionX() + X][Coord.GetPositionY() + Y])->IsCollidableType())
			{
				// Then we cannot place a cell here, and move to the next available position
				FindAssetPlacement(NextCell, Asset, IntegerWidth, IntegerHeight);
			}

			// Otherwise we keep looking
			// Move to the next cell
			X++;

			// Check if we need to move down a row
			// Check this as the end so we can guarantee we are still within 
			// the selected region
			if ((X %= m_Width) == 0)
			{
				X = 0;
				Y++;
			}
		}

		// The selected region is free and able to be taken
		// Return the topleft corner of this rectangle
		return Coord;
	}

	return FindAssetPlacement(NextCell, Asset, IntegerWidth, IntegerHeight);
}

//
// AddDoorWayPosition()
//
//
void MapRoom::AddDoorWayPosition(MapCoordinate Coord)
{
	m_Doorways[Coord.GetPositionX()][Coord.GetPositionY()] = new bool(true);
}