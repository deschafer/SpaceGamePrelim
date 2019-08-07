#include "MapFactory.h"
#include "MapAssetManager.h"

const static std::string MapStr = "MAP";
const static std::string RoomStr = "ROOM";
const static std::string MapTypeStr = "MapType";
const static std::string GlobalStr = "GlobalAssetList";
const static std::string AssetListStr = "AssetList";
const static std::string RoomIDStr = "RoomID";

MapFactory* MapFactory::m_Instance = nullptr;

MapFactory::MapFactory()
{
}

MapFactory::~MapFactory()
{
}

//
// GetMapType()
//
//
MapTypeProperties* MapFactory::GetMapType(unsigned MapID)
{
	if(MapID >= 0 && MapID < m_MapNames.size())
		return m_Maps[MapID];
	else return nullptr;
}

//
// LoadFile()
//
//
void MapFactory::LoadFile(std::string Filename)
{

	// Parse the given file
	TiXmlDocument Document;

	// Load the input file
	if (!Document.LoadFile(Filename.c_str()))
	{
		std::cout << Document.ErrorDesc() << std::endl;
		return;
	}

	TiXmlElement* Node = Document.RootElement();

	for (Node = Node->FirstChildElement(); Node != nullptr; Node = Node->NextSiblingElement())
	{
		// Determine the kind of Node that we have

		// If we have the start of a map def
		if (Node->ValueStr() == MapStr)
		{
			ParseMap(Node);
		}
	}

}

void MapFactory::ParseMap(TiXmlElement* Node)
{
	std::string MapType;
	std::string GlobalAssets;
	std::string RoomID;
	std::string RoomAssets;
	TiXmlElement* Child = Node; // Set a child pointer to the next child node
	MapTypeProperties* Properties;
	std::vector<std::string> PossibleRooms;
	std::vector<unsigned> PossibleAssetLists;
	std::vector<unsigned> GlobalAssetListIDs;

	// Save the information about the map node itself
	MapType = CheckAndCopy(Node, MapTypeStr);
	GlobalAssets = CheckAndCopy(Node, GlobalStr);

	// Then add the map name itself to the data structure
	m_MapNames.push_back(MapType);

	GlobalAssetListIDs.push_back(MapAssetManager::Instance()->StringToListID(GlobalAssets));

	for (Child = Child->FirstChildElement(); Child != nullptr; Child = Child->NextSiblingElement())
	{
		// Determine the kind of Node that we have

		// If we have the start of a room description
		if (Child->ValueStr() == RoomStr)
		{
			RoomID = CheckAndCopy(Child, RoomIDStr);			// Get the ID of the room
			RoomAssets = CheckAndCopy(Child, AssetListStr);	// Then get the list of the room

			// Save the data to the data structures
			PossibleRooms.push_back(RoomID);
			PossibleAssetLists.push_back(MapAssetManager::Instance()->StringToListID(RoomAssets));
		}
	}

	Properties = new MapTypeProperties(
		MapType,
		PossibleRooms,
		PossibleAssetLists,
		GlobalAssetListIDs
	);

	m_Maps.push_back(Properties);
}
