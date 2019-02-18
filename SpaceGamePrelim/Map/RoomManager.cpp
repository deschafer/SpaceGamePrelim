#include "RoomManager.h"

#include <iostream>
#include <string>
#include <iterator>

static const float DefaultInnerX = 0.25;
static const float DefaultInnerY = 0.25;

RoomManager* RoomManager::s_pInstance = nullptr;

RoomManager::RoomManager() :
	m_DefaultInnerX(DefaultInnerX),
	m_DefaultInnerY(DefaultInnerY)
{
}

RoomManager::~RoomManager()
{
}

void RoomManager::RegisterRoomType(RoomProperties *Properties, std::string RoomID)
{

	m_RegisteredTypes[RoomID] = Properties;



#ifdef _DEBUG
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
#ifndef _DEBUG
		std::cout << "Requested RoomID " << RoomID << " Not Found\n";
#endif // DEBUG
		return nullptr;
	}

}

RoomProperties* RoomManager::GetRandomTypeDefinition(std::string &RoomType)
{

	size_t Max = m_RegisteredTypes.size();

	std::map<std::string, RoomProperties*>::iterator Iterator = m_RegisteredTypes.begin();

	std::advance(Iterator, (rand() % m_RegisteredTypes.size()));

	RoomType = Iterator->first;

	return Iterator->second;
	
}
