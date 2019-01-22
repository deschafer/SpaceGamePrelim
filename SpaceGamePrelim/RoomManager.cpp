#include "RoomManager.h"

#include <iostream>
#include <string>

RoomManager* RoomManager::s_pInstance = nullptr;

RoomManager::RoomManager()
{
}

RoomManager::~RoomManager()
{
}

void RoomManager::RegisterRoomType(RoomProperties *Properties, std::string RoomID)
{

	m_RegisteredTypes[RoomID] = Properties;



#ifdef Debug
	std::cout << "New Room Type Registered " << RoomID << std::endl;
#endif // DEBUG
}

RoomProperties* RoomManager::GetTypeDefinition(std::string RoomID)
{
	std::map<std::string, RoomProperties*>::iterator it = m_RegisteredTypes.find(RoomID);
	if (it != m_RegisteredTypes.end())
	{
		// Found the room definition
		return m_RegisteredTypes[RoomID];
	}
	else
	{
		std::cout << "Requested RoomID " << RoomID << " Not Found\n";
		return nullptr;
	}

}