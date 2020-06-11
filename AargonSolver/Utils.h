#pragma once
#include "Types.h"
#include <Windows.h>

#ifdef _WIN32
inline constexpr WORD colorToBgWord(const Color color, const bool light = true)
{
	if (color == NONE) {
		return BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	}

	WORD bg = light ? BACKGROUND_INTENSITY : 0;
	
	if ((color & RED) != 0) {
		bg |= BACKGROUND_RED;
	}

	if ((color & GREEN) != 0) {
		bg |= BACKGROUND_GREEN;
	}

	if ((color & BLUE) != 0) {
		bg |= BACKGROUND_BLUE;
	}

	return bg;
}

inline static void printColorBlock(const Color color, std::string_view s, const bool light = true)
{
	static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	static bool first = false;
	static WORD wDefaultAttributes = 0;
	if (!first) {
		first = true;
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(handle, &info);
		wDefaultAttributes = info.wAttributes;
	}

	SetConsoleTextAttribute(handle, colorToBgWord(color, light));
	std::cout << s;
	SetConsoleTextAttribute(handle, wDefaultAttributes);

}
#endif
