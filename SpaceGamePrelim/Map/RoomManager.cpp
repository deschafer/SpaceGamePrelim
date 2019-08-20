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

	if (Properties->m_MinWidth <= (int)Columns::WIDTH1)
	{
		m_WidthGroups[Columns::WIDTH1].push_back(std::pair<RoomProperties*, std::string>(Properties, RoomID));

	}
	else if (Properties->m_MinWidth >= (int)Columns::WIDTH2 &&
		Properties->m_MinWidth < (int)Columns::WIDTH3)
	{
		m_WidthGroups[Columns::WIDTH2].push_back(std::pair<RoomProperties*, std::string>(Properties, RoomID));


	}
	else if (Properties->m_MinWidth >= (int)Columns::WIDTH3 &&
		Properties->m_MinWidth < (int)Columns::WIDTH4)
	{
		m_WidthGroups[Columns::WIDTH3].push_back(std::pair<RoomProperties*, std::string>(Properties, RoomID));


	}
	else if (Properties->m_MinWidth >= (int)Columns::WIDTH4 &&
		Properties->m_MinWidth < (int)Columns::WIDTH5)
	{
		m_WidthGroups[Columns::WIDTH4].push_back(std::pair<RoomProperties*, std::string>(Properties, RoomID));
	}
	else
	{
		m_WidthGroups[Columns::WIDTH5].push_back(std::pair<RoomProperties*, std::string>(Properties, RoomID));
	}

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

//
// GetRandomTypeThatFits()
// Returns a random room with the sizes that fits the params
//
RoomProperties* RoomManager::GetRandomTypeThatFits(std::string  &roomType, int MaxWidth, int MaxHeight)
{
	std::vector<Columns> Set;
	Set.push_back(Columns::WIDTH1);
	Set.push_back(Columns::WIDTH2);
	Set.push_back(Columns::WIDTH3);
	Set.push_back(Columns::WIDTH4);
	Set.push_back(Columns::WIDTH5);

	int SetMax = 0;


	if (MaxWidth <= (int)Columns::WIDTH1) SetMax = 1;
	else if (MaxWidth >= (int)Columns::WIDTH2 &&
		MaxWidth < (int)Columns::WIDTH3) SetMax = 2;
	else if (MaxWidth >= (int)Columns::WIDTH3 &&
		MaxWidth < (int)Columns::WIDTH4) SetMax = 3;
	else if (MaxWidth >= (int)Columns::WIDTH4 &&
		MaxWidth < (int)Columns::WIDTH5) SetMax = 4;
	else SetMax = 5;

	int SetIndex = rand() % SetMax;
	Columns CurrentCol = Set[SetIndex];

	for (size_t SetNumber = 0; SetNumber < (size_t)SetMax; SetNumber++)
	{
		if (!m_WidthGroups[CurrentCol].empty())
		{
			size_t Index = rand() % m_WidthGroups[CurrentCol].size();
			for (size_t Mag = 0; Mag < m_WidthGroups[CurrentCol].size(); Mag++, Index++)
			{
				if (Index >= m_WidthGroups[CurrentCol].size()) Index = 0;

				if (m_WidthGroups[CurrentCol][Index].first->m_MinWidth <= MaxWidth &&
					m_WidthGroups[CurrentCol][Index].first->m_MinHeight <= MaxHeight)
				{
					roomType = m_WidthGroups[CurrentCol][Index].second;
					return m_WidthGroups[CurrentCol][Index].first;
				}
			}
		}
		CurrentCol = Set[++SetIndex %= SetMax];
	}


	return nullptr;
}

//
// RegisterFallbackRoom()
//
//
void RoomManager::RegisterFallbackRoom(std::string RoomID)
{

	// Convert from the string to a properties object
	RoomProperties* Properties = m_RegisteredTypes[RoomID];
	if (Properties)
	{
		m_FallbackRooms.push_back(Properties);
		m_QueuedFallbackRooms.push_back(std::pair<RoomProperties*, std::string>(Properties, RoomID));
	}

}

//
// GetRandomFallbackRoomThatFits()
//
//
RoomProperties* RoomManager::GetRandomFallbackRoomThatFits(std::string &RoomType, int RoomWidth, int RoomHeight)
{
	// Need mutual exclusion here to verify no entry

	static bool Entry = false;

	while (Entry);

	Entry = true;
	size_t ListSize = m_QueuedFallbackRooms.size();
	RoomProperties* Properties = nullptr;
	int Random = rand() % ListSize;
	std::pair<RoomProperties*, std::string> CurrentSet;

	// Randomize the list
	for (size_t i = 0; i < Random; i++)
	{
		// pop the current room from the beginning
		CurrentSet = m_QueuedFallbackRooms.front();
		m_QueuedFallbackRooms.pop_front();

		// Before we look at it, return it to the back of the list
		m_QueuedFallbackRooms.push_back(CurrentSet);
	}

	for (size_t i = 0; i < ListSize; i++)
	{
		// pop the current room from the beginning
		CurrentSet = m_QueuedFallbackRooms.front();
		m_QueuedFallbackRooms.pop_front();
		Properties = CurrentSet.first;

		// Before we look at it, return it to the back of the list
		m_QueuedFallbackRooms.push_back(CurrentSet);

		// Inspect it and see if it will fit the space available
		if (Properties->m_MinHeight <= RoomHeight &&
			Properties->m_MinWidth <= RoomWidth)
		{
			Entry = false;
			RoomType = CurrentSet.second;
			return Properties;
		}
	}
	Entry = false;
	return nullptr;
}
