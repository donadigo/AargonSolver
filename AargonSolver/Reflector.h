#pragma once
#include "Types.h"
#include <string_view>
#include <math.h>

struct Piece;
class Reflector
{
public:
	virtual std::vector<Beam> reflect(const Piece* piece, const Beam& beam) = 0;
	virtual std::string_view repr() = 0;
};

#define DEFINE_REFLECTOR(className, reprString) \
class className : public Reflector \
{ \
public: \
	std::vector<Beam> reflect(const Piece* piece, const Beam& beam); \
	std::string_view repr() { return reprString; } \
}

DEFINE_REFLECTOR(MirrorReflector, "M");
DEFINE_REFLECTOR(OneWayReflector, "OW");
DEFINE_REFLECTOR(PrismReflector, "P");
DEFINE_REFLECTOR(SplitterReflector, "S");
DEFINE_REFLECTOR(ThreewayReflector, "T");
DEFINE_REFLECTOR(RefractorReflector, "R");
DEFINE_REFLECTOR(ConvertReflector, "CV");
DEFINE_REFLECTOR(DoubleReflector, "D");