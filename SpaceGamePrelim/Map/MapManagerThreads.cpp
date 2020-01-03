#include <iostream>
#include <chrono>

#include "MapManagerThreads.h"

using namespace std;

// User-level threads that work for all platforms
#ifdef CrossPlatform
//
// ThreadGenerateMap()
// Thead handler used for each map generation thread to generate its given map
//
static int ThreadGenerateMap(void* Mem)
{
	if (Mem == nullptr) return -1;

	// Time seed generation
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch();
	long duration = value.count();

	std::chrono::milliseconds dur(duration);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);

	srand(duration);

	// Convert the memory to its map form
	Map* GenMap = static_cast<Map*>(Mem);

	// Then proceed to generate it
	GenMap->Generate();

	return EXIT_SUCCESS;
}

static int ThreadConnectMaps(void* Mem)
{
	// Time seed generation
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch();
	auto duration = value.count();

	std::chrono::milliseconds dur(duration);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);

	srand((unsigned int)duration);

	if (!Mem)
	{
#ifdef _DEBUG
		cout << "Memory passed in was nullptr" << endl;
#endif // _DEBUG
		return -1;
	}

	CorridorThreadInformation* Info = static_cast<CorridorThreadInformation*>(Mem);

	Info->Manager->ConnectTwoMaps(Info->MapTo, Info->MapFrom, Info->LinkBetween);

	return EXIT_SUCCESS;
}
#endif // !CrossPlatform

// Windows kernel-level threads
#ifdef Windows

// Thread used to generate a single map
DWORD WINAPI ThreadGenerateMap(LPVOID lpParam)
{
	// Time seed generation
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch();
	auto duration = value.count();

	std::chrono::milliseconds dur(duration);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);

	srand((unsigned int)duration);

	if (!lpParam)
	{
#ifdef _DEBUG
		cout << "Memory passed in was nullptr" << endl;
#endif // _DEBUG
		return -1;
	}
	Map* NewMap = static_cast<Map*>(lpParam);
	NewMap->Generate();

	return EXIT_SUCCESS;
}

// Thread used to connect all of the pending maps with corridors
DWORD WINAPI ThreadConnectMaps(LPVOID lpParam)
{

	// Time seed generation
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch();
	auto duration = value.count();

	std::chrono::milliseconds dur(duration);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);

	srand((unsigned int)duration);

	if (!lpParam)
	{
#ifdef _DEBUG
		cout << "Memory passed in was nullptr" << endl;
#endif // _DEBUG
		return -1;
	}

	CorridorThreadInformation* Info = static_cast<CorridorThreadInformation*>(lpParam);

	Info->Manager->ConnectTwoMaps(Info->MapTo, Info->MapFrom, Info->LinkBetween);
	
	return EXIT_SUCCESS;
}

#endif // Windows

//
// ThreadGenerator()
// Universal implementation for MapManager threads
//
int ThreadGenerator(void* Memory, MapManagerThreadType Type)
{
	if (!Memory) return -1;

#ifdef Windows
	switch (Type)
	{
	case MapManagerThreadType::MapGenerate:
		CreateThread(
			nullptr,					// default security attributess
			0,							// use default stack size  
			ThreadGenerateMap,			// thread function name
			Memory,						// argument to thread function 
			0,							// use default creation flags 
			nullptr);					// returns the thread identifier 
		break;
	case MapManagerThreadType::MapCorridorGenerate:
		CreateThread(
			nullptr,					// default security attributess
			0,							// use default stack size  
			ThreadConnectMaps,			// thread function name
			Memory,						// argument to thread function 
			0,							// use default creation flags 
			nullptr);					// returns the thread identifier 
		break;
	case MapManagerThreadType::CellsUpdate:
		//...
		//...
		break;
	default:
		break;
	}
#endif // Windows
#ifdef CrossPlatform
	switch (Type)
	{
	case MapManagerThreadType::MapGenerate:
		SDL_CreateThread(
			ThreadGenerateMap,
			"MapGenThread",
			Memory);
		break;
	case MapManagerThreadType::MapCorridorGenerate:
		SDL_CreateThread(
			ThreadConnectMaps,
			"CorridorConnectThread",
			Memory);
		break;
	case MapManagerThreadType::CellsUpdate:
		//...
		//...
		break;
	default:
		break;
	}
#endif // CrossPlatform

	return EXIT_SUCCESS;
}