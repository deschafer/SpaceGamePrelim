#include "RoomDefinitionParser.h"
#include "TinyXML\tinyxml.h"
#include "..\Map\MapAssetManager.h"
#include "..\Map\RoomManager.h"

#include <vector>
#include <iostream>

RoomDefinitionParser* RoomDefinitionParser::m_Instance = nullptr;

const static std::string RoomStr = "ROOM";
const static std::string RoomIDStr = "RoomID";

RoomDefinitionParser::RoomDefinitionParser()
{
}

RoomDefinitionParser::~RoomDefinitionParser()
{
}

bool RoomDefinitionParser::LoadRoomDefinitions(std::string File)
{
	const float DefaultVariance = 0.5;

	std::vector<bool> ParsedGSides;
	std::vector<char> ParsedTurns;
	std::vector<int> ParsedStaticSides;
	std::vector<unsigned> ListIDs;

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
	std::string MinWidth;
	std::string MinHeight;
	std::string VarianceStr;
	std::string DynStr;
	std::string InnerXStr;
	std::string InnerYStr;
	std::string ListID;

	float InnerY;
	float InnerX;
	float Variance;
	bool DynamicSides;

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

			MinWidth.clear();
			MinHeight.clear();
			VarianceStr.clear();
			DynStr.clear();
			InnerXStr.clear();
			InnerYStr.clear();
			ListIDs.clear();

			// Getting the definition name
			DefName = Current->Attribute("name");
			if (Current->Attribute("minwidth"))
			{
				MinWidth = Current->Attribute("minwidth");
			}
			if (Current->Attribute("minheight"))
			{
				MinHeight = Current->Attribute("minheight");
			}
			if (Current->Attribute("variance"))
			{
				VarianceStr = Current->Attribute("variance");
			}
			if (Current->Attribute("dynamic"))
			{
				DynStr = Current->Attribute("dynamic");
			}
			if (Current->Attribute("innerX"))
			{
				InnerXStr = Current->Attribute("innerX");
			}
			if (Current->Attribute("innerY"))
			{
				InnerYStr = Current->Attribute("innerY");
			}

			// Getting the static side definition if it exists
			Current = Current->FirstChildElement();
			while (Current != nullptr &&
				(temp = Current->Value()) == "Asset_List")
			{
				ListID = Current->Attribute("ListID");
				Current = Current->NextSiblingElement();
				ListIDs.push_back(MapAssetManager::Instance()->StringToListID(ListID));
			}

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

			// Getting the variance
			if (VarianceStr.empty())
			{
				Variance = DefaultVariance;
			}
			else
			{
				Variance = std::stof(VarianceStr);
			}
			// Getting if dynamic sides should be used
			if (DynStr.empty())
			{
				DynamicSides = true;
			}
			else
			{
				DynamicSides = std::stoi(DynStr);
			}
			// Checking the status of innerX and innerY params
			if (InnerXStr.empty())
			{
				InnerX = RoomManager::Instance()->GetDefaultInnerX();
			}
			else
			{
				InnerX = std::stof(InnerXStr);
			}
			if (InnerYStr.empty())
			{
				InnerY = RoomManager::Instance()->GetDefaultInnerY();
			}
			else
			{
				InnerY = std::stof(InnerYStr);
			}

			// If no static sides
			if (ParsedStaticSides.empty())
			{
				// If min sizes are present
				if (MinHeight.empty() || MinWidth.empty())
				{
					RoomManager::Instance()->RegisterRoomType(
						new RoomProperties(
							ParsedGSides,
							ParsedTurns,
							Variance,
							InnerX,
							InnerY,
							DynamicSides,
							ListIDs
						),
						DefName);
				}
				else
				{
					RoomManager::Instance()->RegisterRoomType(
						new RoomProperties(
							ParsedGSides,
							ParsedTurns,
							Variance,
							InnerX,
							InnerY,
							DynamicSides,
							ListIDs,
							std::pair<int, int>(
								std::stoi(MinWidth),
								std::stoi(MinHeight))
						),
						DefName);
				}
			}
			// if static sides
			else
			{
				// If min sizes are present
				if (MinHeight.empty() || MinWidth.empty())
				{
					RoomManager::Instance()->RegisterRoomType(
						new RoomProperties(
							ParsedGSides,
							ParsedTurns,
							ParsedStaticSides,
							Variance,
							InnerX,
							InnerY,
							ListIDs
						),
						DefName);
				}
				else
				{
					RoomManager::Instance()->RegisterRoomType(
						new RoomProperties(
							ParsedGSides,
							ParsedTurns,
							ParsedStaticSides,
							Variance,
							InnerX,
							InnerY,
							ListIDs,
							std::pair<int, int>(
								std::stoi(MinWidth),
								std::stoi(MinHeight))
						),
						DefName);
				}

			}

			// Clearing the parsed contents from this definition
			ParsedGSides.clear();
			ParsedTurns.clear();
			ParsedStaticSides.clear();

		}
	}

	return true;
}