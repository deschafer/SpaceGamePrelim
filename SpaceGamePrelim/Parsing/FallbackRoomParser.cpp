#include "FallbackRoomParser.h"
#include "TinyXML\tinyxml.h"
#include "..\Map\MapAssetManager.h"
#include "..\Map\RoomManager.h"

FallbackRoomParser* FallbackRoomParser::m_Instance = nullptr;

const static std::string RoomStr = "ROOM";
const static std::string RoomIDStr = "RoomID";

FallbackRoomParser::FallbackRoomParser()
{
}

FallbackRoomParser::~FallbackRoomParser()
{
}

bool FallbackRoomParser::LoadFallbackRooms(std::string File)
{
	// Parse the given file
	TiXmlDocument Document;

	// Load the input file
	if (!Document.LoadFile(File.c_str()))
	{
		std::cout << Document.ErrorDesc() << std::endl;
		return false;
	}

	TiXmlElement* Node = Document.RootElement();

	for (Node = Node->FirstChildElement(); Node != nullptr; Node = Node->NextSiblingElement())
	{
		// Determine the kind of Node that we have

		// If we have the start of a map def
		if (Node->ValueStr() == RoomStr)
		{
			std::string ID = CheckAndCopy(Node, RoomIDStr);

			// Then we register a new fallback room
			RoomManager::Instance()->RegisterFallbackRoom(ID);
		}
	}
	return true;
}