#pragma once

#include <vector>
#include <list>
#include <iostream>
#include <mutex>

#include "..\Parsing\TinyXML\tinyxml.h"
#include "RoomManager.h"

// This class is responsible for communicating with the room manager
// and loading in the map "biome" definitions that define each named map
// This will be used later in the future to control automation of spawing assets
// and eventually entities as well.

class MapTypeProperties
{
public:
	MapTypeProperties(
		std::string MapTypeName,
		std::vector<std::string> RoomTypes,
		std::vector<unsigned> AssetListIDs,
		std::vector<unsigned> GlobalAssetListIDs) :
		m_MapTypeName(MapTypeName),
		m_PossibleRooms(RoomTypes),
		m_PossibleAssetLists(AssetListIDs),
		m_GlobalAssetListIDs(GlobalAssetListIDs),
		m_MinRoomHeight(INT32_MAX),
		m_MinRoomWidth(INT32_MAX)
	{
		RoomProperties* Properties;

		for (size_t i = 0; i < m_PossibleRooms.size(); i++)
		{
			Properties = RoomManager::Instance()->GetTypeDefinition(m_PossibleRooms[i]);
			if (Properties)
			{
				m_QueuedPossibleRooms.push_back(std::pair<RoomProperties*, std::string>(Properties, m_PossibleRooms[i]));
				m_QueuedAssetListIDs.push_back(AssetListIDs[i]);

				if (Properties->m_MinWidth < m_MinRoomWidth) 
					m_MinRoomWidth = Properties->m_MinWidth;
				if (Properties->m_MinHeight < m_MinRoomHeight)
					m_MinRoomHeight = Properties->m_MinHeight;
			}
		}
	}
	~MapTypeProperties()
	{
	}

	int GetMinWidth() { return m_MinRoomWidth; }
	int GetMinHeight() { return m_MinRoomHeight; }
	std::vector<std::string> GetRooms() { return m_PossibleRooms; }
	std::vector<unsigned> GetAssetIDs() { return m_PossibleAssetLists; }
	std::vector<unsigned> GetGlobalAssetIDs() { return m_GlobalAssetListIDs; }
	RoomProperties* GetRandomRoomThatFits(
		std::string &RoomType, 
		int RoomWidth, 
		int RoomHeight, 
		std::vector<unsigned> &GlobalAssets, 
		unsigned &LocalAssets)
	{
		m_Mutex.lock();

		RoomProperties* Properties = nullptr;
		int Random = rand() % (m_QueuedPossibleRooms.size());
		std::pair<RoomProperties*, std::string> CurrentSet;	
		unsigned CurrentAssetList;

		// Randomize the list
		for (int i = 0; i < Random; i++)
		{
			// pop the current room from the beginning
			CurrentSet = m_QueuedPossibleRooms.front();
			m_QueuedPossibleRooms.pop_front();

			// Before we look at it, return it to the back of the list
			m_QueuedPossibleRooms.push_back(CurrentSet);

			// Randomize the assets as well
			CurrentAssetList = m_QueuedAssetListIDs.front();
			m_QueuedAssetListIDs.pop_front();

			m_QueuedAssetListIDs.push_back(CurrentAssetList);
		}

		for (size_t i = 0; i < m_QueuedPossibleRooms.size(); i++)
		{
			// pop the current room from the beginning
			CurrentSet = m_QueuedPossibleRooms.front();
			m_QueuedPossibleRooms.pop_front();
			Properties = CurrentSet.first;

			// Do the same for the assets
			CurrentAssetList = m_QueuedAssetListIDs.front();
			m_QueuedAssetListIDs.pop_front();

			// Before we look at it, return it to the back of the list
			m_QueuedPossibleRooms.push_back(CurrentSet);
			m_QueuedAssetListIDs.push_back(CurrentAssetList);

			// Inspect it and see if it will fit the space available
			if (Properties->m_MinHeight <= RoomHeight &&
				Properties->m_MinWidth <= RoomWidth)
			{
				RoomType = CurrentSet.second;
				GlobalAssets = m_GlobalAssetListIDs;
				LocalAssets = CurrentAssetList;
			}
			else Properties = nullptr;
		}

		m_Mutex.unlock();
		return Properties;
	}

private:

	std::vector<std::string> m_PossibleRooms;
	std::list<std::pair<RoomProperties*, std::string>> m_QueuedPossibleRooms;
	std::vector<unsigned> m_PossibleAssetLists;
	std::vector<unsigned> m_GlobalAssetListIDs;
	std::list<unsigned> m_QueuedAssetListIDs;		// local assets only

	std::string m_MapTypeName;

	std::mutex m_Mutex;

	int m_MinRoomWidth;
	int m_MinRoomHeight;
};

class MapFactory
{
private:

	static MapFactory* m_Instance;

	MapFactory();

	// Data structures that store information about the maps
	// Parallel vectors
	std::vector<MapTypeProperties*> m_Maps;
	std::vector<std::string> m_MapNames;

	void ParseMap(TiXmlElement* Node);

public:

	static MapFactory* Instance()
	{
		if (!m_Instance)
		{
			return m_Instance = new MapFactory();
		}
		else return m_Instance;
	}

	MapTypeProperties* GetMapType(unsigned MapID);	// just mod the ID with the max size
	unsigned MapTypeQuantity() { return m_MapNames.size(); }
	void LoadFile(std::string Filename); // load in maps in the listed file

	virtual ~MapFactory();
};

