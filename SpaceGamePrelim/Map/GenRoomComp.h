#pragma once

#include <iostream>

enum class Direction { EAST, SOUTH, WEST, NORTH };
enum class Side { TOP, RIGHT, BOTTOM, LEFT, COMPL };
enum class Event { NONE, CHANGE_DIR, SWITCH };


Direction TurnRight(Direction Dir);
Direction TurnLeft(Direction Dir);
Direction Turn(Direction Dir, char code);
Side NextSide(Side side);
Side CorrespondingSide(Direction Dir);
Direction CorrespondingDirection(Side side);
bool SideHorizontal(Side side);
bool SideVertical(Side side);

