#include "SceneFactory.h"

#include "TinyXML\tinyxml.h"

#include <vector>

SceneFactory::SceneFactory()
{
}

SceneFactory::~SceneFactory()
{
}

SceneFactory* SceneFactory::m_Instance = nullptr;

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
const std::string AccelXStr = "accelX";
const std::string AccelYStr = "accelY";
const std::string PosXStr = "posX";
const std::string PosYStr = "posY";
const std::string TypeIDStr = "typeID";
const std::string SpecIDStr = "SpecID";
const std::string HandlerIDStr = "handlerID";
const std::string CompIDStr = "compID";





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
// RegisterNewComponent()
//
//
void SceneFactory::RegisterNewComponent(std::string CompID, ComponentCreator* CompCreator)
{
	if (!m_CompCreators[CompID])
	{
		m_CompCreators[CompID] = CompCreator;
	}
	else
	{
		delete CompCreator;
	}
}

//
// LoadNewScene()
// Loads in the xml file that contains the information for the given scene,
// and sends it to the ActiveSceneManager
//
bool SceneFactory::LoadNewScene(std::string Filename, Scene* LoadingScene)
{
	TiXmlDocument Document;

	// Load the input file
	if (!Document.LoadFile(Filename.c_str()))
	{
		std::cout << Document.ErrorDesc() << std::endl;
		return false;
	}

	TextureProperties* Properties = nullptr;
	const std::string* GetAttribute;
	TiXmlElement* NextNode = Document.RootElement();
	const char* GetString = nullptr;
	std::vector<GameObject*> ParsedObjects;

	// Parsed in variables
	std::string TypeID;
	std::string SpecID;
	int DimX;
	int DimY;
	int DimW;
	int DimH;
	std::string TxtID;
	int AnimSpeed;
	int NumFrames;
	int VelX;
	int VelY;
	int AccelX;
	int AccelY;
	int PosX;
	int PosY;
	std::string HandlerID;
	std::vector<std::string> ComponentIDs;
	ComponentCreator* CompCreator;

	for (NextNode = NextNode->FirstChildElement(); NextNode != nullptr; NextNode = NextNode->NextSiblingElement())
	{
		// Indicates the beginning of a set of objects
		if (NextNode->ValueStr() == ObjectsStr)
		{
			// Getting each of these objects
			for (TiXmlElement* ObjectNode = NextNode->FirstChildElement();
				ObjectNode != nullptr;
				ObjectNode = ObjectNode->NextSiblingElement())
			{

				// For each object, get the following information
				// each of these attributes is information regarding the
				// creation of an object
				if (GetAttribute = ObjectNode->Attribute(TypeIDStr))
				{
					TypeID = *GetAttribute;
				}
				if (GetAttribute = ObjectNode->Attribute(SpecIDStr))
				{
					SpecID = *GetAttribute;
				}
				if (GetAttribute = ObjectNode->Attribute(DimXStr))
				{
					DimX = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(DimYStr))
				{
					DimY = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(DimWStr))
				{
					DimW = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(DimHStr))
				{
					DimH = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(TextureIDStr))
				{
					TxtID = *GetAttribute;
				}
				if (GetAttribute = ObjectNode->Attribute(AnimSpeedStr))
				{
					AnimSpeed = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(NumberFramesStr))
				{
					NumFrames = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(VelocityXStr))
				{
					VelX = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(VelocityYStr))
				{
					VelY = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(AccelXStr))
				{
					AccelX = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(AccelYStr))
				{
					AccelY = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(PosXStr))
				{
					PosX = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(PosYStr))
				{
					PosY = std::stoi(*GetAttribute);
				}
				if (GetAttribute = ObjectNode->Attribute(HandlerIDStr))
				{
					HandlerID = *GetAttribute;
				}

				// Getting each of the components
				
				// Getting each of these objects
				for (TiXmlElement* CompNode = ObjectNode->FirstChildElement();
					CompNode != nullptr;
					CompNode = CompNode->NextSiblingElement())
				{
					if (GetAttribute = CompNode->Attribute(CompIDStr))
					{
						ComponentIDs.push_back(*GetAttribute);
					}
				}

				// First create this specefic object
				GameObject* Object = m_ObjectCreators[TypeID]->CreateObject();
				if (GameEntity* Entity = dynamic_cast<GameEntity*>(Object)) {
					// Then load in its standard data
					Entity->Load(
						new TextureProperties(
							Rect(
								DimX,
								DimY,
								DimW,
								DimH
							),
							TxtID,
							NumFrames,
							AnimSpeed),
						TypeID,
						SpecID,
						Rect(
							DimX,
							DimY,
							DimW,
							DimH
						),
						Vector((float)VelX, (float)VelY),
						Vector((float)AccelX, (float)AccelY),
						Vector((float)PosX, (float)PosY),
						LoadingScene->GetCallback(HandlerID));

					// Then creating all the components and adding them to the 
					// owning object
					for (size_t i = 0; i < ComponentIDs.size(); i++)
					{
						if (CompCreator = m_CompCreators[ComponentIDs[i]])
						{
							Entity->SetComponent(CompCreator->Create(Entity));
						}
					}
				}

				// we also need to set the scene of the object
				Object->SetParentScene(LoadingScene);

				// Adding this object to the parsed objects
				ParsedObjects.push_back(Object);
				ComponentIDs.clear();
			}
		}
	}
	// Before finishing, set the new objects
	LoadingScene->SetGameObjects(ParsedObjects);

	return true;
}