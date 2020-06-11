#pragma once
#include <memory>
#include <ostream>
#include <iostream>
#include "Types.h"
#include "Piece.h"

#define ROWS 11
#define COLS 15 // Full is 18

class PieceMap
{
public:
	PieceMap();
	PieceMap(const PieceMap& pieceMap);
	void update();
	bool isSolved() const;
	int getLitCoins() const;
	std::unique_ptr<Piece>& getAt(const int x, const int y);
	std::vector<Vec2> getAvailableBeamPositions() const;
	const std::vector<Beam>& getBeamsAt(const int x, const int y);
	void add(const Piece& piece);
	void print();
private:
	std::unique_ptr<Piece> map[ROWS][COLS];
	std::vector<Beam> beamMap[ROWS][COLS];
	void clearBeams();
	void emit(const Beam& beam);
	bool updateBeams(const Beam& beam);
};
