#include "InitFactory.h"
#include "..\Parsing\TinyXML\tinyxml.h"

#include <iostream>

InitFactory* InitFactory::m_Instance = nullptr;


InitFactory::InitFactory()
{
}


InitFactory::~InitFactory()
{
}

//
//
//
//
bool InitFactory::LoadRoomDefinitions(std::string File)
{
	TiXmlDocument Document;

	// Load the input file
	if (!Document.LoadFile(File.c_str()))
	{
		std::cout << Document.ErrorDesc() << std::endl;
		return false;
	}

	// Get the root element
	TiXmlElement* Root = Document.RootElement();

	if (Root == nullptr)
	{
		std::cout << "Root nullptr" << std::endl;
		return false;
	}

	// declare the states root node
	TiXmlElement* pStateRoot = nullptr;
	std::string temp;

	for (TiXmlElement* Current = Root->FirstChildElement();
		Current != nullptr;
		Current = pStateRoot->NextSiblingElement())
	{

		if ((Current != nullptr &&
			(temp = Current->Value()) == "DEF"))
		{
			pStateRoot = Current;

			std::string filenameAttribute = Current->Attribute("name");
			std::cout << filenameAttribute << std::endl; // push into list

			Current = Current->FirstChildElement();

			if (Current != nullptr &&
				(temp = Current->Value()) == "Turns")
			{
				for (TiXmlElement* CurrentTurn = Current->FirstChildElement();
					CurrentTurn != nullptr;
					CurrentTurn = CurrentTurn->NextSiblingElement())
				{
					std::string filenameAttribute = CurrentTurn->Attribute("t");
					std::cout << filenameAttribute << std::endl;
				}
			}

			Current = Current->NextSiblingElement();

			if (Current != nullptr &&
				(temp = Current->Value()) == "GSideDef")
			{
				for (TiXmlElement* CurrentTurn = Current->FirstChildElement();
					CurrentTurn != nullptr;
					CurrentTurn = CurrentTurn->NextSiblingElement())
				{
					std::string filenameAttribute = CurrentTurn->Attribute("g");
					std::cout << filenameAttribute << std::endl;
				}
			}



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