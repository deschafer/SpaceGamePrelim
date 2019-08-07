#pragma once

#include <vector>

#include "..\Parsing\TinyXML\tinyxml.h"

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
		m_GlobalAssetListIDs(GlobalAssetListIDs)
	{
	}
	~MapTypeProperties()
	{
	}

	std::vector<std::string> GetRooms() { return m_PossibleRooms; }
	std::vector<unsigned> GetAssetIDs() { return m_PossibleAssetLists; }
	std::vector<unsigned> GetGlobalAssetIDs() { return m_GlobalAssetListIDs; }

private:

	std::vector<std::string> m_PossibleRooms;
	std::vector<unsigned> m_PossibleAssetLists;
	std::vector<unsigned> m_GlobalAssetListIDs;
	std::string m_MapTypeName;
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

