#include "MapAssetManager.h"

#include <iostream>

// Attributes definitions as how they will appear in the xml file
const static std::string AssetStr = "ASSET";
const static std::string AssetListStr = "ASSET_LIST";
const static std::string AssetIDStr = "AssetID";
const static std::string ListIDStr = "ListID";
const static std::string DestWidthStr = "dstRectW";
const static std::string DestHeightStr = "dstRectH";
const static std::string TextureStr= "textureID";
const static std::string AnimationStr = "animSpeed";
const static std::string AnimationBoolStr = "anim";
const static std::string NumberFramesStr = "numFrames";
const static std::string TypeIDStr = "typeID";

std::string CheckAndCopy(TiXmlElement* Node, std::string Comparison)
{
	const std::string* GetAttribute;

	if (GetAttribute = Node->Attribute(Comparison))
	{
		return *GetAttribute;
	}
	return std::string("Empty");
}

MapAssetManager* MapAssetManager::m_Instance = nullptr;

MapAssetManager::MapAssetManager()
{
}

MapAssetManager::~MapAssetManager()
{
}

// 
// CreateAsset()
// Creates a copy of the desired asset and returns it.
//
MapAsset* MapAssetManager::CreateAsset(unsigned AssetID)
{

	if (AssetID > m_Assets.size())
		return nullptr;

	// Return a copy of the desired asset
	return m_Assets[AssetID]->Copy();
}

//
// RegisterAsset()
// Registers the undefined base type into the main data structure.
// It must still be parsed from a source file.
//
void MapAssetManager::RegisterAsset(std::string AssetName, MapAsset* Asset)
{

	m_Assets.push_back(Asset);
	m_AssetNames.push_back(AssetName);

}

//
// AddAssetSourceFile()
// Adds a source file to be parsed on startup
//
void MapAssetManager::AddAssetSourceFile(std::string Filename)
{

	m_ParsingFile = Filename;
	ParseFile();

}

//
// ParseFile()
// Parses in the information to define the base type for object creation,
// and also parses in lists of objects.
//
void MapAssetManager::ParseFile()
{

	// Parse the given file
	TiXmlDocument Document;

	// Load the input file
	if (!Document.LoadFile(m_ParsingFile.c_str()))
	{
		std::cout << Document.ErrorDesc() << std::endl;
		return;
	}

	TiXmlElement* Node = Document.RootElement();

	for (Node = Node->FirstChildElement(); Node != nullptr; Node = Node->NextSiblingElement())
	{
		// Determine the kind of Node that we have

		// If we have a single asset definition
		if (Node->ValueStr() == AssetStr)
		{
			ParseAsset(Node);
		}
		// If we have an asset list definition
		else if (Node->ValueStr() == AssetListStr)
		{
			ParseList(Node);
		}
	}
}

//
// ParseAsset()
// Parses a single asset, and populates its defintion for the base type
// that will be copied.
//
void MapAssetManager::ParseAsset(TiXmlElement* Node)
{
	std::string TypeID;
	Rect DestRect;
	std::string Texture;
	int AnimationSpeed;
	int NumberFrames;
	std::string Animation;

	bool Found = false;
	int Index = -1;
	const std::string *GetAttribute;
	std::vector<std::string> Textures;
	std::vector<int> AnimationSpeeds;
	std::vector<int> NumbersOfFrames;

	TypeID = CheckAndCopy(Node, TypeIDStr);
	Animation = CheckAndCopy(Node, AnimationBoolStr);
	DestRect.SetWidth(stoi(CheckAndCopy(Node, DestWidthStr)));
	DestRect.SetHeight(stoi(CheckAndCopy(Node, DestHeightStr)));
	Texture = CheckAndCopy(Node, TextureStr);
	Textures.push_back(Texture);
	AnimationSpeed = stoi(CheckAndCopy(Node, AnimationStr));
	AnimationSpeeds.push_back(AnimationSpeed);
	NumberFrames = stoi(CheckAndCopy(Node, NumberFramesStr));
	NumbersOfFrames.push_back(NumberFrames);

	// Now we can define the base type for this data

	// First search for the name of the asset
	for (size_t i = 0; i < m_AssetNames.size(); i++)
	{
		if (m_AssetNames[i] == TypeID)
		{
			Found = true;
			Index = i;
			break;
		}
	}

	// Then define the type with Load()
	if (Found)
	{
		// Then we have an animation
		if (Animation == "true")
		{
			// m_AssetNames and m_Assets are parallel vectors
			// We do not know the position, so it is not filled out for now
			m_Assets[Index]->Load(
				Textures,
				MapCoordinate(0, 0),
				DestRect,
				Cell::Asset,
				NumbersOfFrames,
				AnimationSpeeds);
		}
		// No animation
		else
		{
			// m_AssetNames and m_Assets are parallel vectors
			// We do not know the position, so it is not filled out for now
			m_Assets[Index]->Load(
				Textures,
				MapCoordinate(0, 0),
				DestRect,
				Cell::Asset);
		}
	}
	else
	{
#ifdef DEBUG
		std::cout << "No map asset found for " << TypeID << "!" << std::endl;
#endif // DEBUG
	}
}

//
// ParseList()
//
//
void MapAssetManager::ParseList(TiXmlElement* Node)
{

	// Create a new list and add the items within it to the list data structure

}