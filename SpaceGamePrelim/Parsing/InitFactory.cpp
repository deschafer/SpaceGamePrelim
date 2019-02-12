#include "InitFactory.h"
#include "..\Parsing\TinyXML\tinyxml.h"
#include "..\Map\RoomManager.h"

#include <iostream>
#include <vector>



InitFactory* InitFactory::m_Instance = nullptr;


InitFactory::InitFactory()
{
}


InitFactory::~InitFactory()
{
}

//
// LoadRoomDefinitions()
// Loads in all the room definitions from the xml file that contains them.
// Each definition describes its turns and greater sides, and it may or may 
// not have static sides.
//
bool InitFactory::LoadRoomDefinitions(std::string File)
{
	std::vector<bool> ParsedGSides;
	std::vector<char> ParsedTurns;
	std::vector<int> ParsedStaticSides;

	TiXmlDocument Document;

	// Load the input file
	if (!Document.LoadFile(File.c_str()))
	{
		std::cout << Document.ErrorDesc() << std::endl;
		return false;
	}

	// Gets the root element
	TiXmlElement* Root = Document.RootElement();

	if (Root == nullptr)
	{
		std::cout << "Root nullptr" << std::endl;
		return false;
	}

	// declare the states root node
	TiXmlElement* NextChildNode = nullptr;
	std::string temp;
	std::string DefName;

	// Parsing the file
	for (TiXmlElement* Current = Root->FirstChildElement();
		Current != nullptr;
		Current = NextChildNode->NextSiblingElement())
	{
		// Looking for the start of a definition
		if ((Current != nullptr &&
			(temp = Current->Value()) == "DEF"))
		{
			NextChildNode = Current;

			// Getting the definition name
			DefName = Current->Attribute("name");

			// Getting the definitions turns
			Current = Current->FirstChildElement();

			if (Current != nullptr &&
				(temp = Current->Value()) == "Turns")
			{
				for (TiXmlElement* CurrentTurn = Current->FirstChildElement();
					CurrentTurn != nullptr;
					CurrentTurn = CurrentTurn->NextSiblingElement())
				{
					std::string Turn = CurrentTurn->Attribute("t");
					if (!Turn.empty())
					{
						ParsedTurns.push_back(Turn[0]);
					}
				}
			}
			else
			{
				// Not in correct format
				std::cout << "No Turns definition for " << File << std::endl;
				return false;
			}

			// Getting the greater side def
			Current = Current->NextSiblingElement();

			if (Current != nullptr &&
				(temp = Current->Value()) == "GSideDef")
			{
				for (TiXmlElement* CurrentTurn = Current->FirstChildElement();
					CurrentTurn != nullptr;
					CurrentTurn = CurrentTurn->NextSiblingElement())
				{
					std::string GSide = CurrentTurn->Attribute("g");
					if (!GSide.empty())
					{
						ParsedGSides.push_back(std::stoi(GSide));
					}
				}
			}
			else
			{
				// Not in correct format
				std::cout << "No GSide definition for " << File << std::endl;
				return false;
			}

			// Getting the static side definition if it exists
			Current = Current->NextSiblingElement();
			if (Current != nullptr &&
				(temp = Current->Value()) == "StaticSides")
			{
				for (TiXmlElement* CurrentTurn = Current->FirstChildElement();
					CurrentTurn != nullptr;
					CurrentTurn = CurrentTurn->NextSiblingElement())
				{
					std::string StaticSide = CurrentTurn->Attribute("s");
					if (!StaticSide.empty())
					{
						ParsedStaticSides.push_back(std::stoi(StaticSide));
					}
				}
			}

			// If no static sides
			if (ParsedStaticSides.empty())
			{
				RoomManager::Instance()->RegisterRoomType(new RoomProperties(ParsedGSides, ParsedTurns, 0, 0), DefName);
			}
			// if static sides
			else
			{
				RoomManager::Instance()->RegisterRoomType(new RoomProperties(ParsedGSides, ParsedTurns, ParsedStaticSides, 0, 0), DefName);
			}

			// Clearing the parsed contents from this definition
			ParsedGSides.clear();
			ParsedTurns.clear();
			ParsedStaticSides.clear();

		}
	}

	return true;
}

//
//
//
//
bool InitFactory::LoadTextures(std::string File)
{

	return true;
}