#include "PieceMap.h"
#include "Piece.h"
#include "Utils.h"
#include <iostream>

#define MAX_BEAM_UPDATES 2

PieceMap::PieceMap()
{

}

PieceMap::PieceMap(const PieceMap& pieceMap)
{
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			auto& piece = pieceMap.map[row][col];
			if (piece) {
				map[row][col] = std::make_unique<Piece>(*piece);
			}
		}
	}
}

void PieceMap::clearBeams()
{
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			beamMap[row][col].clear();
		}
	}
}

void PieceMap::update()
{
	clearBeams();
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			auto& piece = map[row][col];
			if (!piece) {
				continue;
			}

			if (piece->type == LASER) {
				Beam beam;
				beam.color = piece->color;
				beam.pos = piece->pos;
				beam.rotation = piece->rotation;
				emit(beam);
			}
		}
	}
}

bool PieceMap::isSolved() const
{
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			auto& piece = map[row][col];
			if (piece && piece->type == COIN && !piece->lit) {
				return false;
			}
		}
	}

	return true;
}

int PieceMap::getLitCoins() const
{
	int coins = 0;
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			auto& piece = map[row][col];
			if (piece && piece->type == COIN && piece->lit) {
				coins++;
			}
		}
	}

	return coins;
}

std::unique_ptr<Piece>& PieceMap::getAt(const int x, const int y)
{
	return map[y][x];
}

std::vector<Vec2> PieceMap::getAvailableBeamPositions() const
{
	std::vector<Vec2> positions;
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			auto& piece = map[row][col];
			if (!beamMap[row][col].empty() && !piece) {
				positions.push_back(Vec2(col, row));
			}
		}
	}

	return positions;
}

const std::vector<Beam>& PieceMap::getBeamsAt(const int x, const int y)
{
	return beamMap[y][x];
}

void PieceMap::add(const Piece& piece)
{
	map[piece.pos.y][piece.pos.x] = std::make_unique<Piece>(piece);
}

/**
 * Updates contemporary beams that may be at current position.
 * If a beam changed it's color, coin state at that position 
 * will be updated too.
 *
 * Returns true when emmitting should be continued, false when 
 * emitting is no longer needed (prevents from infinite recursion).
 */
bool PieceMap::updateBeams(const Beam& beam)
{
	auto& beams = beamMap[beam.pos.y][beam.pos.x];
	bool hasMatchingBeam = false;
	for (auto& b : beams) {
		if (isContemporaryRotation(b.rotation, beam.rotation)) {
			if (b.updates >= MAX_BEAM_UPDATES) {
				return false;
			}

			b.color = colorMix(b.color, beam.color);
			b.updates++;
			auto& coin = map[b.pos.y][b.pos.x];
			if (coin && coin->type == COIN) {
				coin->lit = coin->color == b.color;
			}

			hasMatchingBeam = true;
			break;
		}
	}

	if (!hasMatchingBeam) {
		beamMap[beam.pos.y][beam.pos.x].push_back(beam);
	}

	return true;
}

/**
 * Emits the beam reflecting off any pieces until it reaches 
 * the map boundary.
 */
void PieceMap::emit(const Beam& beam)
{
	Vec2 offset = rotationToOffset(beam.rotation);
	Beam current = beam;
	current.pos += offset;

	bool hitPiece = false;
	while (current.pos.x >= 0 && current.pos.y >= 0 && current.pos.x < COLS && current.pos.y < ROWS) {
		auto& piece = map[current.pos.y][current.pos.x];
		if (piece) {
			if (piece->type == COIN) {
				if (piece->color == beam.color) {
					piece->lit = true;
				}
			} else {
				hitPiece = true;
				break;
			}
		}

		if (!updateBeams(current)) {
			return;
		}

		current.pos += offset;
	}

	if (hitPiece) {
		auto& piece = map[current.pos.y][current.pos.x];
#ifdef _DEBUG
		//std::cout << "Hit piece: " << piece->pos << "\n";
#endif _DEBUG
		if (piece->type == REFLECTOR) {
			auto beams = piece->reflect(current);
#ifdef _DEBUG			
			//std::cout << "Reflector " << piece->pos << " produced " << beams.size() << " beams" << "\n";
#endif
			for (auto& b : beams) {
				emit(b);
			}
		}
	}
}

static inline std::string padPieceString(std::string_view sv)
{
	std::string s;
	auto size = sv.size();
	if (size == 1) {
		s += " ";
		s += sv;
		s += " ";
	} else if (size == 2) {
		s += " ";
		s += sv;
	} else if (size == 3) {
		s = sv;
	} else {
		s = "   ";
	}

	return s;
}

static std::string printPiece(const Piece& piece)
{
	std::string s;
	if (piece.type == REFLECTOR) {
		if (piece.reflector) {
			s += padPieceString(piece.reflector->repr());
		} else {
#ifdef _WIN32
			printColorBlock(NONE, "   ");
#else
			s += " \u25A0 ";
#endif
		}
	} else if (piece.type == COIN) {
#ifdef _WIN32
		printColorBlock(piece.color, " C ", piece.lit);
#else
		s += " C";
		s += colorLetter(piece.color);
#endif
	} else if (piece.type == LASER) {
#ifdef _WIN32
		printColorBlock(piece.color, " L ");
#else
		s += " L";
		s += colorLetter(piece.color);
#endif
	} else {
		s += "   ";
	}

	return s;
}

static void printBeams(const std::vector<Beam>& beams)
{
	auto size = beams.size();
	if (size == 0) {
		std::cout << "   ";
	} else if (size > 0 && size < 4) {
		if (size == 1) {
#ifdef  _WIN32
			std::cout << " ";
			printColorBlock(beams[0].color, " ");
			std::cout << " ";
#else
			std::cout << " B" << colorLetter(beams[0].color);
#endif
		} else if (size == 2) {
#ifdef  _WIN32
			std::cout << " ";
			printColorBlock(beams[0].color, " ");
			printColorBlock(beams[1].color, " ");
#else
			std::cout << " B*";
#endif
		} else if (size == 3) {
#ifdef  _WIN32
			printColorBlock(beams[0].color, " ");
			printColorBlock(beams[1].color, " ");
			printColorBlock(beams[2].color, " ");
#else
			std::cout << " B*";
#endif
		}
	} else {
		std::cout << " B*";
	}
}

void PieceMap::print()
{
	std::string separator;
	separator.resize(COLS * 4 + 1, '-');
	separator += "\n";

	std::cout << separator;
	for (int row = 0; row < ROWS; row++) {
		std::cout << "|";
		for (int col = 0; col < COLS; col++) {
			Vec2 pos(col, row);
			auto& piece = getAt(col, row);
			if (piece) {
				std::cout << printPiece(*piece);
			} else {
				auto& beams = getBeamsAt(col, row);
				printBeams(beams);
			}

			std::cout << '|';
		}

		std::cout << "\n";
		std::cout << separator;
	}
}