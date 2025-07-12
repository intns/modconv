#ifndef UTIL_FSTREAM_WRITER_HPP
#define UTIL_FSTREAM_WRITER_HPP

#include <fstream>
#include <type_traits>
#include "../types.hpp"
#include <vector>

namespace util {

class fstream_writer : public std::ofstream {
public:
	fstream_writer()

	    = default;
	~fstream_writer() override                       = default;
	fstream_writer(const fstream_writer&)            = delete;
	fstream_writer& operator=(const fstream_writer&) = delete;

	void align(u32 amt)
	{
		const u32 pos = static_cast<u32>(tellp());
		for (u32 i = 0; i < ((~(amt - 1) & (pos + amt - 1)) - pos); ++i) {
			writeU8(0);
		}
	}

	void writeU8(u8 val) { write(reinterpret_cast<char*>(&val), 1); }
	void writeU16(u16 val)
	{
		u16 value = ((val & 0xff00) >> 8) | ((val & 0x00ff) << 8);
		write(reinterpret_cast<char*>(&value), 2);
	}
	void writeU32(u32 val)
	{
		u32 value = ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
		write(reinterpret_cast<char*>(&value), 4);
	}

	void writeS8(s8 val) { write(reinterpret_cast<char*>(&val), 1); }
	void writeS16(s16 val)
	{
		s16 value = ((val & 0xff00) >> 8) | ((val & 0x00ff) << 8);
		write(reinterpret_cast<char*>(&value), 2);
	}
	void writeS32(s32 val)
	{
		s32 value = ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
		write(reinterpret_cast<char*>(&value), 4);
	}

	void writeF32(f32 val)
	{
		u32 const intVal = reinterpret_cast<u32&>(val);
		u32 value
		    = ((intVal & 0xff000000) >> 24) | ((intVal & 0x00ff0000) >> 8) | ((intVal & 0x0000ff00) << 8) | ((intVal & 0x000000ff) << 24);
		write(reinterpret_cast<char*>(&value), 4);
	}
};

} // namespace util

#endif