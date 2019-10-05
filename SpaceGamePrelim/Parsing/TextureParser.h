#pragma once

#include <string>

//
// This class is used to parse in all texture definitions from xml files
//

class TextureParser
{
private:

	static TextureParser* m_Instance;

	TextureParser();

public:

	static TextureParser* Instance()
	{
		if (!m_Instance) 
		{
			return m_Instance = new TextureParser;
		}
		else
		{
			return m_Instance;
		}
	}

	bool LoadTextures(std::string File);
	virtual ~TextureParser();
};

