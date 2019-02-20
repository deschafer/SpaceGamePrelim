#pragma once

#include <map>
#include <string>

#include "..\Scene\ActiveSceneManager.h"
#include "..\Objects\BaseCreator.h"

// singleton class structure
class SceneFactory
{
private:

	std::map<std::string, BaseCreator*> m_ObjectCreators;	// Holds the required creation functions for each object
	static SceneFactory* m_Instance;

	SceneFactory();
		
public:

	static SceneFactory* Instance()
	{
		if (m_Instance == nullptr)
		{
			return (m_Instance = new SceneFactory());
		}
		else return m_Instance;
	}

	void RegisterNewObject(std::string TypeID, BaseCreator* ObjectCreator);
	bool LoadNewScene(std::string Filename, Scene* LoadingScene);


	~SceneFactory();
};

