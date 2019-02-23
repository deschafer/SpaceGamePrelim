#include "RoomManager.h"

#include <iostream>
#include <string>
#include <iterator>

static const float DefaultInnerX = 0.25;
static const float DefaultInnerY = 0.25;

RoomManager* RoomManager::s_pInstance = nullptr;

RoomManager::RoomManager() :
	m_DefaultInnerX(DefaultInnerX),
	m_DefaultInnerY(DefaultInnerY),
	m_AbsMinH(0),
	m_AbsMinW(0)
{
}

RoomManager::~RoomManager()
{
}

void RoomManager::RegisterRoomType(RoomProperties *Properties, std::string RoomID)
{

	m_RegisteredTypes[RoomID] = Properties;
	if (Properties->m_MinWidth > m_AbsMinW)
	{
		m_AbsMinW = Properties->m_MinWidth;
	}
	if (Properties->m_MinHeight > m_AbsMinH)
	{
		m_AbsMinH = Properties->m_MinHeight;
	}

	m_RandomTypes.push_back(Properties);

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


RoomProperties* RoomManager::GetRandomTypeThatFits(std::string  &roomType, int MaxWidth, int MaxHeight)
{
	int InitIndex = rand() % m_RandomTypes.size();

	for(size_t i = InitIndex, mag = 0; mag < m_RandomTypes.size(); i++, mag++)
	{ 
	
		if (i >= m_RandomTypes.size()) i = 0;

		if (m_RandomTypes[i]->m_MinWidth <= MaxWidth &&
			m_RandomTypes[i]->m_MinHeight <= MaxHeight)
		{
			return m_RandomTypes[i];
		}


	}

	// No room fits inside the size
	return nullptr;
}
