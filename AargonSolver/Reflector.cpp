#include "Reflector.h"
#include "Piece.h"
#include <iostream>

static inline void transformRotationToOrigin(Rotation& pieceRotation, Rotation& beamRotation)
{
	if (beamRotation >= 180) {
		int diff = 360 - beamRotation;
		beamRotation = NORTH;
		pieceRotation = rotationAdd(pieceRotation, diff);
	}
}

static inline int calculateRelativeRotationDiff(Rotation pieceRotation, Rotation beamRotation)
{
	transformRotationToOrigin(pieceRotation, beamRotation);
	return pieceRotation - beamRotation;
}

std::vector<Beam> MirrorReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	int diff = calculateRelativeRotationDiff(piece->rotation, beam.rotation);
	if (diff == 45 || diff == 90 || diff == 135) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, diff), beam.color);
	}

	return r;
}

std::vector<Beam> OneWayReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	if (piece->rotation == beam.rotation) {
		r.push_back(beam);
	}

	return r;
}

std::vector<Beam> PrismReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	int diff = calculateRelativeRotationDiff(piece->rotation, beam.rotation);
	if (diff == 0) {
		if ((beam.color & RED) != 0) {
			r.emplace_back(beam.pos, beam.rotation, RED);
		}

		if ((beam.color & GREEN) != 0) {
			r.emplace_back(beam.pos, rotationAdd(piece->rotation, 45), GREEN);
		}

		if ((beam.color & BLUE) != 0) {
			r.emplace_back(beam.pos, rotationAdd(piece->rotation, 90), BLUE);
		}
	}

	if (diff == 90 && (beam.color & BLUE) != 0) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 180), BLUE);
	}

	if (diff == 135 && (beam.color & GREEN) != 0) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 180), GREEN);
	}

	if (diff == 180 && (beam.color & RED) != 0) {
		r.emplace_back(beam.pos, beam.rotation, RED);
	}

	return r;
}

std::vector<Beam> SplitterReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	int diff = calculateRelativeRotationDiff(piece->rotation, beam.rotation);
	if (diff == 0) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 90), beam.color);
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 270), beam.color);
	}

	return r;
}

std::vector<Beam> ThreewayReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	int diff = calculateRelativeRotationDiff(piece->rotation, beam.rotation);
	if (diff == 0) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 45), beam.color);
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 315), beam.color);
	} else if (diff == 135 || diff == 225) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 180), beam.color);
	}

	return r;
}

std::vector<Beam> RefractorReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	int diff = calculateRelativeRotationDiff(piece->rotation, beam.rotation);
	if (diff == 0) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 45), beam.color);
	} else if (diff == 135) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, 180), beam.color);
	}

	return r;
}

std::vector<Beam> ConvertReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	int diff = calculateRelativeRotationDiff(piece->rotation, beam.rotation);

#define CONVERT_BEAM(r, beam, from, to) if (beam.color == from) r.emplace_back(beam.pos, beam.rotation, to)
	if (diff == 0) {
		CONVERT_BEAM(r, beam, RED, GREEN);
		CONVERT_BEAM(r, beam, BLUE, RED);
		CONVERT_BEAM(r, beam, GREEN, BLUE);
		CONVERT_BEAM(r, beam, YELLOW, CYAN);
		CONVERT_BEAM(r, beam, CYAN, MAGENTA);
		CONVERT_BEAM(r, beam, MAGENTA, YELLOW);
	} else if (diff == 180) {
		CONVERT_BEAM(r, beam, RED, BLUE);
		CONVERT_BEAM(r, beam, BLUE, GREEN);
		CONVERT_BEAM(r, beam, GREEN, RED);
		CONVERT_BEAM(r, beam, YELLOW, MAGENTA);
		CONVERT_BEAM(r, beam, CYAN, YELLOW);
		CONVERT_BEAM(r, beam, MAGENTA, CYAN);
#undef CONVERT_BEAM
	}

	return r;
}

std::vector<Beam> DoubleReflector::reflect(const Piece* piece, const Beam& beam)
{
	std::vector<Beam> r;
	int diff = calculateRelativeRotationDiff(piece->rotation, beam.rotation);
	if (diff != 0 && diff != 180) {
		r.emplace_back(beam.pos, rotationAdd(piece->rotation, diff), beam.color);
	}

	return r;
}
