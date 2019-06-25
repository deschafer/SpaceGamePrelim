#pragma once

#include "MapAssets\MapAsset.h"
#include "..\Parsing\TinyXML\tinyxml.h"

#include <string>
#include <vector>
#include <list>

//
// This class manages the base types of all map assets within the game.
// The idea is to be able to separate assets into named lists to support
// procedural generation in the future.
//

class MapAssetManager
{
private:

	static MapAssetManager* m_Instance;

	std::vector<MapAsset*> m_Assets;		// Parallel vectors that contain information regarding the base asset type
	std::vector<std::string> m_AssetNames;  // And the actual name of the asset, for creation and XML parsing
	std::vector<std::vector<MapAsset*>> m_TypeLists;
	std::vector<std::string> m_TypeListNames;

	std::string m_ParsingFile;

	MapAssetManager();

	void ParseFile();
	void ParseAsset(TiXmlElement* Node);
	void ParseList(TiXmlElement* Node);

public:

	static MapAssetManager* Instance()
	{
		if (!m_Instance)
		{
			return m_Instance = new MapAssetManager();
		}
		else return m_Instance;
	}

	
	MapAsset* CreateAsset(unsigned AssetID);	
	void RegisterAsset(std::string, MapAsset* Asset);
	void AddAssetSourceFile(std::string Filename);


	~MapAssetManager();
};
