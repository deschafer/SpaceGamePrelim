#pragma once

#include <string>

//
// This class is used to store and manage any fallback rooms
//

class FallbackRoomParser
{
private:

	static FallbackRoomParser* m_Instance;

	FallbackRoomParser();

public:

	static FallbackRoomParser* Instance()
	{
		if (!m_Instance)
		{
			return m_Instance = new FallbackRoomParser;
		}
		else 
		{
			return m_Instance;
		}
	}

	bool LoadFallbackRooms(std::string File);
	virtual ~FallbackRoomParser();
};

