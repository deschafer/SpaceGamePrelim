#pragma once

//
// RoomManager
//
// Responsible for maintaining a database (std::map) of named map definitions. 
// These defenitions are loaded into this object during init/load phase, then
// when creating a random room, these defenitions can be used to create the room.
//

#include <vector>
#include <map>
#include <iostream>

//
// RoomProperties
// NOTES:
// Default size of Min height and width is 5
// By Default, no static sides are used
// GreaterSideDef.size() must equal Turns.Size() for a complete definition
//

//
// This struct defines a room for MapRoom::Generate().
//
struct RoomProperties
{
	std::vector<char> m_Turns;		// # of turns must equal # of sides
	std::vector<bool> m_GreaterSideDefinition;
	std::vector<int> m_StaticSides;	// Set certain sides as a static length
	
	int m_MinWidth;			// Minimum User-set recommended width for this room
	int m_MinHeight;		// Minimum User-set recommended height for this room
	int m_Variation;		// Parameter denoting the level of variation in the Generated rooms
	int m_Extremism;		// Higher number forces more extreme versions of rooms
	bool m_StaticSidesFlag;	// Indicates that static sides are a part of this def

	// CTOR of RoomProperties w/o static sides
	RoomProperties(std::vector<bool> GreaterSides, std::vector<char> Turns,
		int Variation, int Extreme, 
		std::pair<int, int> WidthHeigth = std::pair<int,int>(5,5)) :
		m_Variation(Variation),
		m_Extremism(Extreme),
		m_StaticSidesFlag(false)
	{
		if (GreaterSides.size() != Turns.size())
		{
			throw "Sizes of vectors do not match!";
			abort();
		}
		for (size_t i = 0; i < GreaterSides.size(); i++)
		{
			m_GreaterSideDefinition.push_back(GreaterSides[i]);
			m_Turns.push_back(Turns[i]);
		}
	}

	// CTOR of RoomProperties w/ static sides
	RoomProperties(std::vector<bool> GreaterSides, std::vector<char> Turns, 
		std::vector<int> StaticSides, int Variation, int Extreme,
		std::pair<int, int> WidthHeigth = std::pair<int, int>(5, 5)) :
		m_Variation(Variation),
		m_Extremism(Extreme),
		m_StaticSidesFlag(true)
	{
		if (GreaterSides.size() != Turns.size() || GreaterSides.size() != StaticSides.size())
		{
			throw "Sizes of vectors do not match!";
			abort();
		}
		for (size_t i = 0; i < GreaterSides.size(); i++)
		{
			m_GreaterSideDefinition.push_back(GreaterSides[i]);
			m_Turns.push_back(Turns[i]);
			m_StaticSides.push_back(StaticSides[i]);
		}
#ifndef _DEBUG
		std::cout << "StaticSides Defintion Present\n";
#endif // !_DEBUG
	}

};

// Singleton class structure
class RoomManager
{
private:
	RoomManager();

	std::map<std::string, RoomProperties*> m_RegisteredTypes;	// Contains the registered room definitions

	static RoomManager* s_pInstance;

public:

	static RoomManager* Instance()
	{
		if (s_pInstance == nullptr)
		{
			s_pInstance = new RoomManager;
			return s_pInstance;
		}
		else return s_pInstance;
	}

	void RegisterRoomType(RoomProperties *Properties, std::string RoomID);
	RoomProperties* GetTypeDefinition(std::string RoomID);
	RoomProperties* GetRandomTypeDefinition(std::string * roomType);
	
	~RoomManager();
};

