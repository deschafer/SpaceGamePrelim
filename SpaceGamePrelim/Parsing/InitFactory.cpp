
#include "InitFactory.h"
#include "..\Parsing\TinyXML\tinyxml.h"
#include "..\Map\RoomManager.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Frame\MainApplication.h"
#include "..\Map\MapAssetManager.h"

#include <iostream>
#include <vector>

#include <string.h>


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

//
// LoadTextures()
// Loads in both reduced textures and textures from a given xml file
//
bool InitFactory::LoadTextures(std::string File)
{

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
	std::string ReducedTextureName;
	std::string TextureName;
	std::string TexturePath;
	Rect SourceDimensions;

	// Parsing the file
	for (TiXmlElement* Current = Root->FirstChildElement();
		Current != nullptr;
		Current = NextChildNode->NextSiblingElement())
	{
		// Looking for the start of a texture definition
		if ((Current != nullptr &&
			(temp = Current->Value()) == "TEXTURE"))
		{
			NextChildNode = Current;

			// Getting the definition name
			TextureName = Current->Attribute("name");
			TexturePath = Current->Attribute("path");

			// Add the new texture
			TextureManager::Instance()->SetTexture(
				TexturePath, 
				TextureName, 
				MainApplication::Instance()->GetRenderer());

		}
		// Looking for the start of a reduced texture definition
		else if ((Current != nullptr &&
			(temp = Current->Value()) == "RED_TEXTURE"))
		{
			NextChildNode = Current;

			// Getting the definition name
			ReducedTextureName = Current->Attribute("name");
			TextureName = Current->Attribute("assoc_name");

			// Getting the dimensions
			temp = Current->Attribute("x");
			int X = std::stoi(temp);
			temp = Current->Attribute("y");
			int Y = std::stoi(temp);
			temp = Current->Attribute("width");
			int Width = std::stoi(temp);
			temp = Current->Attribute("height");
			int Height = std::stoi(temp);

			SourceDimensions = Rect(X, Y, Width, Height);

			// Adds the new reduced texture
			TextureManager::Instance()->SetReducedTexture(
				ReducedTextureName, 
				new TextureProperties(SourceDimensions, TextureName, 0, 0));
		}
		// Looking for the start of a texture group
		else if ((Current != nullptr &&
			(temp = Current->Value()) == "TEXTURE_GROUP"))
		{

			NextChildNode = Current;

			std::vector<std::string> ParsedGroup;		// Storage for the textures of this group
			std::string GroupName = Current->Attribute("name");

			// Getting each group name
			for (TiXmlElement* NextNode = Current->FirstChildElement();
				NextNode != nullptr;
				NextNode = NextNode->NextSiblingElement())
			{
				if (NextNode != nullptr)
				{
					// Getting the name of this texture
					std::string Name = NextNode->Attribute("name");
					if (!Name.empty())
					{
						ParsedGroup.push_back(Name);
					}
				}
			}
			// Finally adding to the manager
			TextureManager::Instance()->AddTextureGroup(GroupName, ParsedGroup);
		}
	}

	return true;
}