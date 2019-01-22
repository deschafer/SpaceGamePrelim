
#include "MapManager.h"
#include "MainApplication.h"

#include <iostream>
#include <ctime>

#include <map>

#define RANDOM 71245

using namespace std;

enum class Direction{EAST, SOUTH, WEST, NORTH};
enum class Event{NONE, CHANGE_DIR, SWITCH};


Direction TurnRight(Direction Dir);
Direction TurnLeft(Direction Dir);
Direction Turn(Direction Dir, char code);

Direction TurnRight(Direction Dir)
{
	int num = static_cast<int>(Dir);
	num += 1;
	if (num == 4) num = 0;

	return static_cast<Direction>(num);
}

Direction TurnLeft(Direction Dir)
{
	int num = static_cast<int>(Dir);
	num -= 1;
	if (num == -1) num = 3;

	return static_cast<Direction>(num);
}

Direction Turn(Direction Dir, char code)
{
	

	if (code == 'R')
	{
		return TurnRight(Dir);
	}
	else if (code == 'L')
	{
		return TurnLeft(Dir);
	}
	else
		abort();

}


MapManager* MapManager::m_Instance = nullptr;

MapManager::MapManager() :
	m_ActiveWndHeight(0),
	m_ActiveWndWidth(0),
	m_Init(true)
{
	
	m_RoomManager = RoomManager::Instance();
}


MapManager::~MapManager()
{
}

void MapManager::DrawGrid()
{
	int CellWidth = 32;
	int CellHeight = 32;
	int OldR, OldG, OldB, OldA;


	SDL_Renderer* renderer = MainApplication::Instance()->GetRenderer();

	m_ActiveWndHeight = MainApplication::Instance()->GetWndHeight();
	m_ActiveWndWidth = MainApplication::Instance()->GetWndWidth();
	
	m_Rows = m_ActiveWndHeight / CellHeight; // Get a count of columns
	m_Columns = m_ActiveWndWidth / CellWidth;		// Get a count of rows

	// Definetely have to do something else here
	if (MainApplication::Instance()->ResolutionChanged())
	{
		
		m_VisibleObjectArray = new MapObject**[m_Columns];
		for (int i = 0; i < m_Columns; i++)
		{
			m_VisibleObjectArray[i] = new MapObject*[m_Rows];
		}
	}

	if (m_Init)
	{
		for (int i = 0; i < m_Columns; i++)
		{
			for (int j = 0; j < m_Rows; j++)
			{
				m_VisibleObjectArray[i][j] = nullptr;
			}
		}

		m_Init = false;
	}

	

	SDL_GetRenderDrawColor(renderer, (Uint8*)&OldR, (Uint8*)&OldG, (Uint8*)&OldB, (Uint8*)&OldA);
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, SDL_ALPHA_OPAQUE);



	// Drawing horizontal lines
	for (int i = 0; i < m_Rows; i++)
	{
	
		SDL_RenderDrawLine(renderer, 0, (i + 1) * CellHeight, m_ActiveWndWidth, (i + 1) * CellHeight);

	}

	// Drawing vertical lines
	for (int i = 0; i < m_Columns; i++)
	{

		SDL_RenderDrawLine(renderer, (i + 1) * CellWidth, 0, (i + 1) * CellWidth, m_ActiveWndHeight);

	}

	SDL_SetRenderDrawColor(renderer, OldR, OldG, OldB, OldA);


	//DrawDefaultMapObjects();
	DrawDefinedRoom();
	//DrawDefaultRoom();


	DrawVisibleCells();
}

void MapManager::DrawVisibleCells()
{

	for (int i = 0; i < m_Columns; i++)
	{
		for (int j = 0; j < m_Rows; j++)
		{
			if (m_VisibleObjectArray[i][j] != nullptr)
			{
				m_VisibleObjectArray[i][j]->Draw();
			}
		}
	}


}

//
// This is a TEST function to play with random room generation
//
//
void MapManager::DrawDefaultRoom()
{

}


void MapManager::DrawDefinedRoom()
{

	RoomProperties* Properties = m_RoomManager->GetTypeDefinition("Rectangle");

	int StartX = 10;
	int StartY = 6;

	static bool complete = false;
	int Width = 25;
	int Height = 6;
	int MaxHeightX = StartX;
	int MaxHeightY = StartY + Height;
	int MaxWidthX = StartX + Width;
	int MaxWidthY = StartY;

	int rng = 0; // 0 = south, 1 = west, 2 = north, 3 = east
	int count = 0;
	int TempX = StartX;
	int TempY = StartY;

	int CurrentLengthQuota = 0;

	bool Switch = false;
	bool SwitchStart = false;
	bool EventPossible = false;
	bool LastSide = false;

	Event EventType = Event::NONE;
	Direction CurrentDirection = Direction::EAST;

	map<pair<int, int>, bool> CurrentLocations;

	vector<int> Sides; // size = number of sides, each side has a length.
	vector<char> Turns;

	pair<int, int> CurrentPair;

	for (int i = 0; i < Properties->m_Sides.size(); i++)
	{
		Sides.push_back(Properties->m_Sides[i]);
		Turns.push_back(Properties->m_Turns[i]);
	}

	int LengthParam = rand() % 7;
	int HeightParam = rand() % 3;


	while (!complete)
	{
		count++;

		// Popping the front element
		if (!Sides.empty() && CurrentLengthQuota <= 0)
		{
			CurrentLengthQuota = Sides.front();
			Sides.erase(Sides.begin());
			if (Sides.empty()) LastSide = true;
		}
		else if(Sides.empty() && CurrentLengthQuota <= 0)
		{
			complete = true;
			break;
		}

		// If the current point is the starting point
		if ((CurrentPair = make_pair(TempX, TempY)) == make_pair(StartX, StartY) && Sides.empty())
		{
			complete = true;
			break;
		}
		else // and the end
		{
			// then we can move to the start pos.
		}

		
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
			if ((TempX - 1) == StartX)
			{
			}
			else
			{
				TempX--;

			}

		}
		else if (CurrentDirection == Direction::NORTH)
		{

			

			if (LastSide)
			{
				if (TempX > StartX)
				{
					TempX--;
					CurrentLengthQuota++; // Not counted in quota
				}
				else if (TempX < StartX)
				{
					TempX++;
					CurrentLengthQuota++; // Not counted in the quota
				}
				else if(TempY > StartY)
				{
					TempY--;
					CurrentLengthQuota++;
				}
				else
				{
					TempY--;
				}
			}
			else
			{
				TempY--;
			}

		}

		// Adding the current coordinates to the record
		CurrentPair.swap(make_pair(TempX, TempY));
		CurrentLocations[CurrentPair] = true;

		m_VisibleObjectArray[TempX][TempY] = new MapCell(new TextureProperties(Rect(0, 0, 32, 32), "Room", 1, 0, 0, 1), MapCoordinate(TempX * 32, TempY * 32));


		CurrentLengthQuota--;

		if (CurrentLengthQuota == 0 && !Turns.empty())
		{
			CurrentDirection = Turn(CurrentDirection, Turns.front());
			Turns.erase(Turns.begin());
		}

	}


}


//
//
// This will load the inview map objects
//
void MapManager::LoadMapObjects()
{

	

}

//
//
//
//
void MapManager::LoadDefaultMapObjects()
{


}


void MapManager::DrawDefaultMapObjects()
{

	for (int i = 0; i < m_Rows; i++)
	{
		for (int j = 0; j < m_Columns; j++)
		{

			TextureManager::Instance()->DrawCurrentFrame("DefaultMap",
				(j) * 32,
				(i) * 32,
				Rect(0, 0, 32, 32),
				SDL_FLIP_NONE,
				MainApplication::Instance()->GetRenderer(),
				1, 0);

		}
	}

}
