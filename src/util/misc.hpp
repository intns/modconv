#ifndef _MISC_HPP
#define _MISC_HPP

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <types.hpp>
#include <vector>

namespace util {
static const u32 CalculateTxeSize(u32 format, u32 x, u32 y)
{
	switch (format) {
	case 0:
	case 2:
	case 6:
		return 2 * y * x;
	case 1:
		return (x * y) / 2;
	case 3:
		return y * x / 2;
	case 4:
	case 5:
	case 8:
	case 9:
		return y * x;
	case 7:
		return 4 * y * x;
	default:
		return 0;
	}
}

} // namespace util

#endif