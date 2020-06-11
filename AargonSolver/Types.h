#pragma once
#include <vector>
#include <ostream>
#include "Vec2.h"

enum Color
{
	NONE = 0b000,
	RED = 0b100,
	GREEN = 0b010,
	BLUE = 0b001,
	YELLOW = 0b110,
	MAGENTA = 0b101,
	CYAN = 0b011,
	WHITE = 0b111
};

inline constexpr char colorLetter(const Color color)
{
	switch (color)
	{
	case RED:
		return 'R';
	case GREEN:
		return 'G';
	case BLUE:
		return 'B';
	case YELLOW:
		return 'Y';
	case MAGENTA:
		return 'M';
	case CYAN:
		return 'C';
	case WHITE:
		return 'W';
	case NONE:
	default:
		break;
	}

	return ' ';
}

inline constexpr Color colorMix(const Color a, const Color b)
{
	return (Color)(a | b);
}

enum Rotation
{
	NORTH = 0, 
	NORTH_EAST = 45,
	EAST = 90,
	SOUTH_EAST = 135,
	SOUTH = 180,
	SOUTH_WEST = 225,
	WEST = 270,
	NORTH_WEST = 315
};

struct Beam
{
	Vec2 pos;
	Rotation rotation = NORTH;
	Color color = NONE;
	int updates = 0;

	Beam()
	{
	}

	Beam(const Vec2& _pos, const Rotation _rot, Color _color)
	{
		pos = _pos;
		rotation = _rot;
		color = _color;
	}
};

inline Vec2 rotationToOffset(const Rotation rot)
{
	switch (rot)
	{
	case NORTH:
		return Vec2(0, -1);
	case NORTH_EAST:
		return Vec2(1, -1);
	case WEST:
		return Vec2(-1, 0);
	case SOUTH_EAST:
		return Vec2(1, 1);
	case SOUTH:
		return Vec2(0, 1);
	case SOUTH_WEST:
		return Vec2(-1, 1);
	case EAST:
		return Vec2(1, 0);
	case NORTH_WEST:
		return Vec2(-1, -1);
	default:
		break;
	}

	return Vec2(0, 0);
}

inline constexpr bool isContemporaryRotation(const Rotation a, const Rotation b)
{
	if (a == b) {
		return true;
	}

#define CHECK(a, b, rot1, rot2) \
	if ((a == rot1 && b == rot2) || (a == rot2 && b == rot1)) return true

	CHECK(a, b, NORTH, SOUTH);
	CHECK(a, b, NORTH_EAST, SOUTH_WEST);
	CHECK(a, b, NORTH_WEST, SOUTH_EAST);
	CHECK(a, b, EAST, WEST);

#undef CHECK
	return false;
}

inline constexpr Rotation rotate(const Rotation rot, int r)
{
	return static_cast<Rotation>((rot + r * 45) % 360);
}

inline constexpr Rotation rotationAdd(const int a, const int b)
{
	int r = a + b;
	if (r < 0) {
		r += 360;
	}

	return static_cast<Rotation>(r % 360);
}