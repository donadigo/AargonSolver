#pragma once
#include <memory>
#include "Types.h"
#include "Reflector.h"

enum PieceType
{
	REFLECTOR,
	COIN,
	LASER,
};

struct Piece
{
	PieceType type = REFLECTOR;
	Vec2 pos;
	Rotation rotation = Rotation::NORTH;
	Color color = NONE;
	std::shared_ptr<Reflector> reflector = nullptr;
	bool movable = true;
	bool rotatable = true;
	bool lit = false;

	static Piece createCoin(const int x, const int y, const Color _color)
	{
		return Piece(COIN, Vec2(x, y), NORTH, _color, nullptr, false, false);
	}

	static Piece createLaser(const int x, const int y, const Rotation _rot, Color _color)
	{
		return Piece(LASER, Vec2(x, y), _rot, _color, nullptr, false, false);
	}

	static Piece createReflector(const int x,
								 const int y,
								 const Rotation _rot,
								 std::shared_ptr<Reflector> ref = nullptr,
								 bool movable = true, bool rotatable = true)
	{
		return Piece(REFLECTOR, Vec2(x, y), _rot, NONE, ref, movable, rotatable);
	}

	Piece() {}
	//TODO: inplace construction
	Piece(const PieceType _type, 
		  const Vec2& _pos, 
		  const Rotation _rot, 
		  Color _color, 
		  std::shared_ptr<Reflector> ref = nullptr,
		  bool _movable = true, bool _rotatable = true)
	{
		type = _type;
		pos = _pos;
		color = _color;
		rotation = _rot;
		reflector = ref;
		movable = _movable;
		rotatable = _rotatable;
	}

	~Piece() {}

	std::vector<Beam> reflect(const Beam& beam)
	{
		if (reflector == nullptr) {
			return {};
		}

		return reflector->reflect(this, beam);
	}
};