#pragma once
#include <ostream>

struct Vec2
{
	int x;
	int y;

	Vec2() : x(0), y(0) {}
	Vec2(int _x, int _y) : x(_x), y(_y) {}

	Vec2 operator+(const Vec2& b) const
	{
		return Vec2(x + b.x, y + b.y);
	}

	Vec2 operator-(const Vec2& b) const
	{
		return Vec2(x - b.x, y - b.y);
	}

	void operator+=(const Vec2& b)
	{
		x += b.x;
		y += b.y;
	}

	void operator-=(const Vec2& b)
	{
		x -= b.x;
		y -= b.y;
	}
};

inline bool operator==(const Vec2& a, const Vec2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
	os << "<" << v.x << ", " << v.y << ">";
	return os;
}

