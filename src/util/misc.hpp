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
namespace fs = std::filesystem;

inline bool AreFilesIdentical(const fs::path& path1, const fs::path& path2)
{
	if (!fs::is_regular_file(path1) || !fs::is_regular_file(path2)) {
		return false;
	}

	if (fs::file_size(path1) != fs::file_size(path2)) {
		return false;
	}

	std::ifstream file1(path1, std::ios::binary);
	std::ifstream file2(path2, std::ios::binary);

	if (!file1 || !file2) {
		return false;
	}

	constexpr std::size_t bufferSize = 0x1000;
	std::array<char, bufferSize> buffer1 {}, buffer2 {};

	while (file1 && file2) {
		file1.read(buffer1.data(), bufferSize);
		file2.read(buffer2.data(), bufferSize);

		const auto bytesRead1 = file1.gcount();
		const auto bytesRead2 = file2.gcount();

		if (bytesRead1 != bytesRead2) {
			return false;
		}

		if (std::memcmp(buffer1.data(), buffer2.data(), bytesRead1) != 0) {
			return false;
		}
	}

	return file1.eof() && file2.eof();
}

inline const u32 CalculateTxeSize(u32 format, u32 x, u32 y)
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