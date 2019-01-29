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

Direction TurnRight(Direction Dir)
{
	int num = static_cast<int>(Dir);
	num += 1;
	if (num == 4) num = 0;

	return static_cast<Direction>(num);
}

Direction TurnLeft(Direction Dir)
{
	int num = static_cast<int>(Dir);
	num -= 1;
	if (num == -1) num = 3;

	return static_cast<Direction>(num);
}

Direction Turn(Direction Dir, char code)
{

	if (code == 'R')
	{
		return TurnRight(Dir);
	}
	else if (code == 'L')
	{
		return TurnLeft(Dir);
	}
	else
	{
		std::cout << "incorrect turn" << std::endl;
		abort();
	}

}

Side NextSide(Side side)
{
	int num = static_cast<int>(side);
	num += 1;

	return static_cast<Side>(num);
}

Side CorrespondingSide(Direction Dir)
{

	switch (Dir)
	{
	case Direction::EAST:
		return Side::TOP;

	case Direction::SOUTH:
		return Side::RIGHT;

	case Direction::WEST:
		return Side::BOTTOM;

	case Direction::NORTH:
		return Side::LEFT;

	default:
		break;
	}

}

Direction CorrespondingDirection(Side side)
{

	switch (side)
	{
	case Side::TOP:
		return Direction::EAST;

	case Side::RIGHT:
		return Direction::SOUTH;

	case Side::BOTTOM:
		return Direction::WEST;

	case Side::LEFT:
		return Direction::NORTH;

	default:
		break;
	}

}

bool SideHorizontal(Side side)
{
	switch (side)
	{
	case Side::TOP:
		return true;
		break;
	case Side::RIGHT:
		return false;
		break;
	case Side::BOTTOM:
		return true;
		break;
	case Side::LEFT:
		return false;
		break;
	case Side::COMPL:
		throw "incorrect side";
		break;
	default:
		break;
	}

}

bool SideVertical(Side side)
{
	switch (side)
	{
	case Side::TOP:
		return false;
		break;
	case Side::RIGHT:
		return true;
		break;
	case Side::BOTTOM:
		return false;
		break;
	case Side::LEFT:
		return true;
		break;
	case Side::COMPL:
		throw "incorrect side";
		break;
	default:
		break;
	}

}