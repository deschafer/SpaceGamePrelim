#include "SceneFactory.h"

#include "TinyXML\tinyxml.h"

SceneFactory::SceneFactory()
{
}

SceneFactory::~SceneFactory()
{
}

// Attributes definitions as how they will appear in the xml file
const std::string ObjectsStr = "OBJECTS";
const std::string ObjectStr = "OBJECT";
const std::string DimXStr = "dimX";
const std::string DimYStr = "dimY";
const std::string DimWStr = "dimW";
const std::string DimHStr = "dimH";
const std::string TextureIDStr = "txtID";
const std::string AnimSpeedStr = "animSpeed";
const std::string NumberFramesStr = "numFrames";
const std::string VelocityXStr = "velX";
const std::string VelocityYStr = "velY";
const std::string AccelYStr = "accelY";
const std::string AccelYStr = "accelY";
const std::string PosXStr = "posX";
const std::string PosYStr = "posY";


//
// RegisterNewObject()
// Registers a new type with this factory for creation later
//
void SceneFactory::RegisterNewObject(std::string TypeID, BaseCreator* ObjectCreator)
{
	if (!m_ObjectCreators[TypeID])
	{
		m_ObjectCreators[TypeID] = ObjectCreator;
	}
	else
	{
		delete ObjectCreator;
	}
}

//
// LoadNewScene()
// Loads in the xml file that contains the information for the given scene,
// and sends it to the ActiveSceneManager
//
bool SceneFactory::LoadNewScene(std::string Filename)
{
	TiXmlDocument Document;

	// Load the input file
	if (!Document.LoadFile(Filename.c_str()))
	{
		std::cout << Document.ErrorDesc() << std::endl;
		return false;
	}

	TextureProperties* Properties = new TextureProperties;
	std::string GetAttribute;
	TiXmlElement* NextNode = Document.RootElement();

	for (; NextNode != nullptr; NextNode = NextNode->NextSiblingElement())
	{
		// Indicates the beginning of a set of objects
		if (NextNode->ValueStr() == ObjectsStr)
		{
			// Getting each of these objects
			for (TiXmlElement* ObjectNode = NextNode->FirstChildElement();
				ObjectNode != nullptr; 
				ObjectNode = ObjectNode->NextSiblingElement())
			{
				if (ObjectNode->Attribute("dimX"))
				{

				}
			}

		}
	}


}