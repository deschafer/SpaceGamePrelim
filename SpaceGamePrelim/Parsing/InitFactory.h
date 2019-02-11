#pragma once

#include <string>


// Singleton class structure
class InitFactory
{
private:

	InitFactory();

	static InitFactory* m_Instance;

public:

	static InitFactory* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new InitFactory;
			return m_Instance;
		}
		else return m_Instance;
	}

	bool LoadRoomDefinitions(std::string File);
	bool LoadTextures(std::string File);

	~InitFactory();
};

