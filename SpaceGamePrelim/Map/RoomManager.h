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
	float m_Variation;		// Parameter denoting the level of variation in the Generated rooms (0 - 1)
	bool m_StaticSidesFlag;	// Indicates that static sides are a part of this def
	bool m_DynamicRandomFlag;	// Indicates that the static sides are to be generated programatically

	float m_InnerSizeY;		// Indicates the maximum size for movement inside the shape on the indicated axis
	float m_InnerSizeX;		// ..

	// CTOR of RoomProperties w/o static sides
	RoomProperties(std::vector<bool> GreaterSides, std::vector<char> Turns,
		float Variation, float InnerX, float InnerY, bool DynamicRandomFlag,
		std::pair<int, int> WidthHeigth = std::pair<int, int>(8, 8)) :
		m_Variation(Variation),
		m_StaticSidesFlag(false),
		m_MinWidth(WidthHeigth.first),
		m_MinHeight(WidthHeigth.second),
		m_DynamicRandomFlag(DynamicRandomFlag),
		m_InnerSizeX(InnerX),
		m_InnerSizeY(InnerY)
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
		std::vector<int> StaticSides, float Variation, float InnerX, float InnerY,
		std::pair<int, int> WidthHeigth = std::pair<int, int>(8, 8)) :
		m_Variation(Variation),
		m_StaticSidesFlag(true),
		m_MinWidth(WidthHeigth.first),
		m_MinHeight(WidthHeigth.second),
		m_DynamicRandomFlag(false),
		m_InnerSizeX(InnerX),
		m_InnerSizeY(InnerY)
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
	#ifdef _DEBUG
		std::cout << "StaticSides Defintion Present\n";
	#endif // _DEBUG

	}

};


// Singleton class structure
class RoomManager
{
private:
	RoomManager();

	std::map<std::string, RoomProperties*> m_RegisteredTypes;	// Contains the registered room definitions

	static RoomManager* s_pInstance;

	// Default params for some rooms
	const float m_DefaultInnerX;
	const float m_DefaultInnerY;

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

	float GetDefaultInnerX() { return m_DefaultInnerX; }
	float GetDefaultInnerY() { return m_DefaultInnerY; }


	void RegisterRoomType(RoomProperties *Properties, std::string RoomID);
	RoomProperties* GetTypeDefinition(std::string RoomID);
	RoomProperties* GetRandomTypeDefinition(std::string  &roomType);
	
	~RoomManager();
};

