# SpaceGamePrelim

*An **initial** game framwork*

The first, basic components that will be incorporated into a fully working game later in the future.

The goal of this project is to create a set of tools on top of the Simple DirectMedia Layer Graphics Library to allow much simpler creation of games. This will be accomplished by creating a basic game framework forming the backbone of this project. This framework is concerned with the lower level aspects of this game, including, the game cycle, texture management, memory, etc. 

In addition to this framework, a map system will also be added. The goal for this map system is to devise an algorithm that can generate both rooms and maps randomly based on a definition. These maps will also be procedural, and have the ability to be endless.

**UPDATE 2/8/2019**

Added a variety of improvements from the first initial version. Some of the changes include the following:
* Class hierarchy for both normal game objects and map objects
* Variety of Map types such as a Map, MapRoom, MapObject, MapManager singleton, to help with the generation and maintenance of rooms and maps.
* Scene management system
* Buttons and other simple types
* Input handling via a singleton class
* Room Generation...

The Room Generation is the most complicated part of this project so far. Essentially, it generates a room based off a user-defined room definition. This definition, stored in Map/RoomManager.h RoomProperties, contains the information needed to contruct a complex or simple shape. This definition is then used by the generation algorithm to use as a model for its generated version. The algorithm is stored in /Map/MapRoom MapRoom::Generate().

**UPDATE 2/17/2019**

Enhanced and added several new components to further expand the framework. These include the following:
* Added several parameters that alter the appearance of a defined room to RoomProperties
* Added min height and width params for each room definition in RoomProperties
* Added support for dynamic inner sides in select defined rooms
* Added support for reflection of generated room types
* Added xml parsing with the tinyXML library for both textures and room definitions. These are now stored in a file and loaded into the program with the InitFactory class
* Added the concept of reduced textures
* Added the concept of texture groups

*Below are some images of the current room generation

![alt-text](https://raw.githubusercontent.com/deschafer/SpaceGamePrelim/master/Presentation/room1.jpg "Room 1")

![alt-text](https://raw.githubusercontent.com/deschafer/SpaceGamePrelim/master/Presentation/room2.jpg "Room 2")

![alt-text](https://raw.githubusercontent.com/deschafer/SpaceGamePrelim/master/Presentation/room3.jpg "Room 2")

![alt-text](https://raw.githubusercontent.com/deschafer/SpaceGamePrelim/master/Presentation/room4.jpg "Room 2")
