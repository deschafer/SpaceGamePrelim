#include "TextureParser.h"
#include "TinyXML\tinyxml.h"
#include "..\BasicTypes\Rect.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Frame\MainApplication.h"

TextureParser* TextureParser::m_Instance = nullptr;

TextureParser::TextureParser()
{
}

TextureParser::~TextureParser()
{
}

bool TextureParser::LoadTextures(std::string File)
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