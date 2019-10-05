#pragma once

#include <string>

//
// This class is responsible for loading in all room definitions from an xml file
//

class RoomDefinitionParser
{
private:

	static RoomDefinitionParser* m_Instance;

	RoomDefinitionParser();

public:

	static RoomDefinitionParser* Instance()
	{
		if (!m_Instance)
		{
			return m_Instance = new RoomDefinitionParser;
		}
		else
		{
			return m_Instance;
		}
	}

	bool LoadRoomDefinitions(std::string File);
	virtual ~RoomDefinitionParser();
};

