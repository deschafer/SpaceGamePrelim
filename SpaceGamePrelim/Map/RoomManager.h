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



struct RoomProperties
{
	//std::vector<int> m_Sides; // # of sides must equal # of turns
	std::vector<char> m_Turns; // # of turns must equal # of sides
	std::vector<bool> m_GreaterSideDefinition;
	
	int m_Variation;		// Parameter denoting the level of variation in the 
						// Generated rooms
	int m_Extremism;	// Higher number forces more extreme versions of rooms

	RoomProperties(std::vector<bool> GreaterSides, std::vector<char> Turns,
		int Variation, int Extreme) :
		m_Variation(Variation),
		m_Extremism(Extreme)
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

