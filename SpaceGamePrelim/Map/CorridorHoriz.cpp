#include "CorridorHoriz.h"
#include "..\TextureCode\TextureDefinitions.h"
#include "..\Map\MapAssetManager.h";


using namespace std;

CorridorHoriz::CorridorHoriz(int Width, int Height, MapRoom* LeftRoom, MapRoom* RightRoom) :
	Corridor(Width, Height, LeftRoom, RightRoom)
{
}

CorridorHoriz::~CorridorHoriz()
{
}

//
// AddBeginningCell()
//
//
void CorridorHoriz::AddBeginningCell(MapCoordinate Pos)
{
	vector<string> Textures;
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Top));

	Textures.clear();
	Textures.push_back(WallBottom);
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Bottom));

	Textures.clear();
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()),
		new MapInactive(Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Floor));

	// then we also add a door at this position
	MapAsset* Door = MapAssetManager::Instance()->CreateAsset(MapAssetManager::Instance()->StringToAssetID("Door"));
	Door->SetParentRoom(m_RoomAboveOrRight);
	AddAsset(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()), Door);
}

//
// NormalMajorAxisMovement()
//
//
void CorridorHoriz::NormalMajorAxisMovement(MapCoordinate Pos)
{
	vector<string> Textures;
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Top));

	Textures.clear();
	Textures.push_back(WallBottom);
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Bottom));
}

//
// MinorAxisMovementBeg()
//
//
void CorridorHoriz::MinorAxisMovementBeg(MapCoordinate Pos, Movement Direction)
{
	vector<string> Textures;
	if (Direction != Movement::UP)
	{
		Textures.push_back(WallSideRight);
		AddCell(MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
			new MapWall(Textures,
				MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
				Rect(0, 0, m_CellWidth, m_CellHeight),
				Cell::Wall_Right));
		Textures.clear();
	}

	Textures.push_back(WallSideLeft);
	AddCell(MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY()),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Left));
	AddCell(MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY() + ((Direction == Movement::UP) ? 1 : -1)),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY() + ((Direction == Movement::UP) ? 1 : -1)),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Left));

	Textures.clear();
	Direction == Movement::UP ? Textures.push_back(WallCornerRight) : Textures.push_back(WallSideLeft);
	AddCell(MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY() + ((Direction == Movement::UP) ? 2 : -2)),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY() + ((Direction == Movement::UP) ? 2 : -2)),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Direction == Movement::UP ? Cell::Wall_Corner_Right : Cell::Wall_Left));
}

//
// MinorAxisMovementEnd()
//
//
void CorridorHoriz::MinorAxisMovementEnd(MapCoordinate Pos, Movement Direction)
{
	vector<string> Textures;
	(Direction == Movement::UP) ?
		Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup)) :
		Textures.push_back(WallBottom);

	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + ((Direction == Movement::UP) ? -1 : 1)),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + ((Direction == Movement::UP) ? -1 : 1)),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			(Direction == Movement::UP) ? Cell::Wall_Top : Cell::Wall_Bottom));

	Textures.clear();
	Textures.push_back(WallSideRight);
	AddCell(MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Right));

	if (Direction == Movement::UP)
	{
		AddCell(MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY() - 1),
			new MapWall(Textures,
				MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY() - 1),
				Rect(0, 0, m_CellWidth, m_CellHeight),
				Cell::Wall_Right));
	}
	else
	{
		Textures.clear();
		Textures.push_back(WallCornerLeft);
		AddCell(MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY() + 1),
			new MapWall(Textures,
				MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY() + 1),
				Rect(0, 0, m_CellWidth, m_CellHeight),
				Cell::Wall_Corner_Left));
	}
}

//
// NormalMinorAxisMovement()
//
//
void CorridorHoriz::NormalMinorAxisMovement(MapCoordinate Pos)
{
	vector<string> Textures;
	Textures.push_back(WallSideRight);
	AddCell(MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Right));

	Textures.clear();
	Textures.push_back(WallSideLeft);
	AddCell(MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY()),
		new MapWall(Textures,
			MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Left));
}

//
// GenerateCorridor()
//
//
Corridor* CorridorHoriz::GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix, MapCoordinate Begin, MapCoordinate End, int DistanceBetween)
{

	MapCoordinate Swap = End;
	End = Begin;
	Begin = Swap;

	// Local data
	float Result = (float)DistanceBetween;
	int CurrentX = Begin.GetPositionX();											// Current X location as we traverse the matrix
	int CurrentY = Begin.GetPositionY();											// Current Y location as we traverse the matrix
	int DistanceX = abs(Begin.GetPositionX() - End.GetPositionX());					// Total distance X-wise that needs to be covered
	int DistanceY = abs(Begin.GetPositionY() - End.GetPositionY());					// Total distance Y-wise that needs to be covered
	int MidPointX = (CurrentX + ((int)(Result = (Result / 2)) ? (int)Result : 1));	// The calc. midpoint where we attempt to change axis movement
	bool Try = false;
	bool MovementY = false;					// Type of movement for cell creation
	bool MovementX = false;					// ..
	bool BeginY = false;					// First X cell, for cell creation
	bool EndY = false;						// Last X cell
	Movement Direction;						// Direction of minor axis movement
	vector<string> Textures;
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));

	// First check if the ending point can be reached
	int X = End.GetPositionX();
	int Y = End.GetPositionY();
	if (X - 1 >= 0 && BoundsMatrix[X - 1][Y] && !CorridorLocaleMartrix[X - 1][Y])
	{
		cout << "Bounded error" << endl;
		return nullptr;
	}

	// Get direction of minor axis movement
	if (End.GetPositionY() < Begin.GetPositionY()) Direction = Movement::UP;
	else Direction = Movement::DOWN;

	// Testing if the start is within a cell
	if (BoundsMatrix[CurrentX + 1][CurrentY] && !CorridorLocaleMartrix[CurrentX + 1][CurrentY])
	{
		cout << "Start is within a cell" << endl;
		return nullptr;
	}
	else if (CorridorLocaleMartrix[CurrentX + 1][CurrentY])
	{
		cout << "Corridor Horiz: Anomoly detected, a Double Corridor has been generated." << endl;
	}

	// Adjusting the y transition 
	// so that no conflicts occur
	while (BoundsMatrix[MidPointX][CurrentY] && !CorridorLocaleMartrix[MidPointX][CurrentY])
	{
		MidPointX--; // Move backwards
		if (MidPointX < 0)
		{
			cout << "No valid path found" << endl;
			return false;
		}
	}

	AddBeginningCell(Begin);

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
			for (int i = 0, Pos = CurrentY; i < DistanceY; i++)
			{
				if (Direction == Movement::UP) Pos--;
				else Pos++;
				// If this cell is not occupied by an external cell
				// and it is not one of our drawn texture cells
				if (BoundsMatrix[CurrentX][Pos] && !CorridorLocaleMartrix[CurrentX][Pos])
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
			new MapInactive(Textures, MapCoordinate(CurrentX, CurrentY), Rect(0, 0, m_CellWidth, m_CellHeight), Cell::Floor);

		// Determining correct tile types for the corresponding cells
		// Normal X movement
		if (MovementX)
		{
			NormalMajorAxisMovement(MapCoordinate(CurrentX, CurrentY));
			MovementX = false;
		}
		else if (MovementY)
		{
			// Beginning cell for y movement
			if (BeginY)
			{	
				MinorAxisMovementBeg(MapCoordinate(CurrentX, CurrentY), Direction);
			}
			// Ending cell for y movement
			if (EndY)
			{
				MinorAxisMovementEnd(MapCoordinate(CurrentX, CurrentY), Direction);
			}
			// Normal y movement cell
			if (!BeginY && !EndY)
			{
				NormalMinorAxisMovement(MapCoordinate(CurrentX, CurrentY));
			}

			BeginY = false;
			EndY = false;
			MovementY = false;
		}
	}

	if (CurrentX != End.GetPositionX() || CurrentY != End.GetPositionY())
	{
		cout << "Potential Error: CorridorVertical was unable to find an ending point" << endl;
		return nullptr;
	}
	else 
	{
		// place a door at the ending position
		MapAsset* Door = MapAssetManager::Instance()->CreateAsset(MapAssetManager::Instance()->StringToAssetID("Door"));
		Door->SetParentRoom(m_RoomAboveOrRight);
		AddAsset(MapCoordinate(CurrentX, CurrentY), Door);
	}

	return this;
}
