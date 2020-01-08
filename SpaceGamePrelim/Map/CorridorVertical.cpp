
#include "CorridorVertical.h"
#include "../TextureCode/TextureDefinitions.h"
#include "MapAssetManager.h"
#include "MapManager.h"

using namespace std;

CorridorVertical::CorridorVertical(int Width, int Height, MapRoom* RoomOne, MapRoom* RoomTwo) :
	Corridor(Width, Height, RoomOne, RoomTwo)
{
}

CorridorVertical::~CorridorVertical()
{
}

// 
// AddBeginningCell()
//
//
void CorridorVertical::AddBeginningCell(MapCoordinate Pos)
{
	// Loading a floor texture
	vector<string> Textures;
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));

	// Setting the beggining cell set
	vector<string> TexturesT;
	TexturesT.push_back(WallSideRight);
	AddCell(MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			TexturesT,
			MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Right));

	TexturesT.clear();
	TexturesT.push_back(WallSideLeft);
	AddCell(MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY()),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			TexturesT,
			MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Left));

	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()),
		new MapInactive(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Floor));

	// then we also add a door at this position
	MapAsset* Door = MapAssetManager::Instance()->CreateAsset(MapAssetManager::Instance()->StringToAssetID("Door"));
	Door->SetParentRoom(m_RoomAboveOrRight);
	Door->SetParentMap(m_RoomAboveOrRight->GetParentMap());
	AddAsset(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()), Door);
}

//
// NormalMajorAxisMovement()
// 
//
void CorridorVertical::NormalMajorAxisMovement(MapCoordinate Pos)
{
	vector<string> Textures;
	Textures.push_back(WallSideRight);
	AddCell(MapCoordinate(Pos.GetPositionX() - 1, Pos.GetPositionY()),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Right));

	Textures.clear();
	Textures.push_back(WallSideLeft);
	AddCell(MapCoordinate(Pos.GetPositionX() + 1, Pos.GetPositionY()),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Left));
}

//
// MinorAxisMovementBeg()
//
//
void CorridorVertical::MinorAxisMovementBeg(MapCoordinate Pos, Movement Direction)
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
	AddCell(MapCoordinate(Pos.GetPositionX() + LessChange, Pos.GetPositionY() + 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX() + LessChange, Pos.GetPositionY() + 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Bottom));

	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Bottom));

	Textures.clear();
	(Direction == Movement::LEFT) ? Textures.push_back(WallCornerRight) : Textures.push_back(WallCornerLeft);
	AddCell(MapCoordinate(Pos.GetPositionX() + Change, Pos.GetPositionY() + 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX() + Change, Pos.GetPositionY() + 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			(Direction == Movement::LEFT) ?
			Cell::Wall_Corner_Right_Bottom :
			Cell::Wall_Corner_Left_Bottom));

	Textures.clear();
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Top));
}

//
// MinorAxisMovementEnd()
//
//
void CorridorVertical::MinorAxisMovementEnd(MapCoordinate Pos, Movement Direction)
{
	int Change = 0;

	if (Direction == Movement::LEFT) Change = -1;
	else Change = 1;

	vector<string> Textures;
	(Change > 0) ? Textures.push_back(WallSideLeft) : Textures.push_back(WallSideRight);
	AddCell(MapCoordinate(Pos.GetPositionX() + Change, Pos.GetPositionY() - 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX() + Change, Pos.GetPositionY() - 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			(Change > 0) ? Cell::Wall_Left : Cell::Wall_Right));

	AddCell(MapCoordinate(Pos.GetPositionX() + Change, Pos.GetPositionY()),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX() + Change, Pos.GetPositionY()),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			(Change > 0) ? Cell::Wall_Left : Cell::Wall_Right));

	Textures.clear();
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Top));
}

//
// NormalMinorAxisMovement()
//
//
void CorridorVertical::NormalMinorAxisMovement(MapCoordinate Pos)
{
	vector<string> Textures;
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() - 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Top));

	Textures.clear();
	Textures.push_back(WallBottom);
	AddCell(MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
		new MapWall(Rect(0, 0, m_CellWidth, m_CellHeight),
			nullptr, 
			Textures,
			MapCoordinate(Pos.GetPositionX(), Pos.GetPositionY() + 1),
			Rect(0, 0, m_CellWidth, m_CellHeight),
			Cell::Wall_Bottom));
}

//
// GenerateCorridor()
//
//
Corridor* CorridorVertical::GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix, MapCoordinate Begin, MapCoordinate End, int DistanceBetween)
{
	// Local data
	float Result = (float)DistanceBetween;
	int CurrentX = Begin.GetPositionX();										// Current X location as we traverse the matrix
	int CurrentY = Begin.GetPositionY();										// Current Y location as we traverse the matrix
	int DistanceX = abs(Begin.GetPositionX() - End.GetPositionX());				// Total distance X-wise that needs to be covered
	int DistanceY = abs(Begin.GetPositionY() - End.GetPositionY());				// Total distance Y-wise that needs to be covered
	int MidPointY = (CurrentY + (int)((Result = ceil(Result / 2)) ? Result : 1));	// The calc. midpoint where we attempt to change axis movement
	bool Try = false;																					
	bool MovementY = false;					// Type of movement for cell creation
	bool MovementX = false;					// ..
	bool BeginX = false;					// First X cell, for cell creation
	bool EndX = false;						// Last X cell
	Movement Direction;						// Direction of minor axis movement
	vector<string> Textures;
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));

	// First check if the ending point can be reached
	int X = End.GetPositionX();
	int Y = End.GetPositionY();
	if (Y - 1 >= 0 && BoundsMatrix[X][Y - 1] && !CorridorLocaleMartrix[X][Y - 1])
	{
		return nullptr;
	}
	
	// Determine the direction of minor axis movement
	if (End.GetPositionX() > Begin.GetPositionX()) Direction = Movement::RIGHT;
	else Direction = Movement::LEFT;

	// Adjusting the x transition 
	// so that no conflicts occur
	while (BoundsMatrix[CurrentX][MidPointY] && !CorridorLocaleMartrix[CurrentX][MidPointY] && MidPointY > 0)
	{
		MidPointY--;
	}

	// Sets the beginning cells
	AddBeginningCell(MapCoordinate(Begin.GetPositionX(), Begin.GetPositionY()));

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
			for (size_t i = 0, Pos = CurrentX; i < (size_t)DistanceX; i++)
			{
				if (Direction == Movement::LEFT) Pos--;
				else Pos++;
				// If this cell is not occupied by an external cell
				// and it is not one of our drawn texture cells
				if (BoundsMatrix[Pos][CurrentY] && !CorridorLocaleMartrix[Pos][CurrentY])
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
			new MapInactive(Rect(0, 0, m_CellWidth, m_CellHeight),
				nullptr, 
				Textures,
				MapCoordinate(CurrentX, CurrentY),
				Rect(0, 0, m_CellWidth, m_CellHeight), Cell::Floor);

		// If there was a y movement,  then we add to the 
		// cells to the left and right to add the walls to
		// encompass the corridor
		if (MovementY)
		{
			NormalMajorAxisMovement(MapCoordinate(CurrentX, CurrentY));

			MovementY = false;
		}
		// Otherwise, if a x movement occurred
		else if (MovementX)
		{
			// The beg of the x-movement
			// DIRECTION MATTERS
			if (BeginX)
			{
				MinorAxisMovementBeg(MapCoordinate(CurrentX, CurrentY), Direction);
			}
			// The end of the x-movement
			// DIRECTION MATTERS
			if (EndX)
			{
				MinorAxisMovementEnd(MapCoordinate(CurrentX, CurrentY), Direction);
			}
			// Adding to the cells above and below this current cell
			// Does not matter with the direction of the x corridor movement
			else if (!BeginX && !EndX)
			{
				NormalMinorAxisMovement(MapCoordinate(CurrentX, CurrentY));
			}
			EndX = false;
			BeginX = false;
			MovementX = false;
		}
	}


	if (CurrentX != End.GetPositionX() || CurrentY != End.GetPositionY())
	{
		cout << "Potential Error: CorridorVertical was unable to find an ending point" << endl;
		/*
		vector<string> Textures;
		Textures.push_back("Test");

		// Adding the corridor to the array
		m_Cells[Begin.GetPositionX()][Begin.GetPositionY()] =
			new MapInactive(Textures, MapCoordinate(CurrentX, CurrentY), Cell::Floor);

		Textures.clear();
		Textures.push_back("Explosion");

		// Adding the corridor to the array
		m_Cells[End.GetPositionX()][End.GetPositionY()] =
			new MapInactive(Textures, MapCoordinate(CurrentX, CurrentY), Cell::Floor);
		*/

		return nullptr;
	}
	else 
	{
		// place a door at the ending position
		MapAsset* Door = MapAssetManager::Instance()->CreateAsset(MapAssetManager::Instance()->StringToAssetID("Door"));
		Door->SetParentMap(m_RoomAboveOrRight->GetParentMap());
		Door->SetParentRoom(m_RoomAboveOrRight);
		AddAsset(MapCoordinate(CurrentX, CurrentY), Door);
	}

	return this;
}