#ifndef UTIL_FSTREAM_READER_HPP
#define UTIL_FSTREAM_READER_HPP

#include <math.h>

#include <fstream>
#include <type_traits>
#include "../types.hpp"
#include <vector>
#include <cstring>

namespace util {

class fstream_reader : public std::ifstream {
public:
	fstream_reader()           = default;
	~fstream_reader() override = default;

	fstream_reader(const fstream_reader&)            = delete;
	fstream_reader& operator=(const fstream_reader&) = delete;

	std::streampos getRemaining() { return m_filesize - tellg(); }
	[[nodiscard]] std::streampos getFilesize() const { return m_filesize; }

	void align(std::streamoff amt = cfg::MOD_ALIGNMENT_AMT)
	{
		if (amt == 0) {
			return;
		}

		std::streampos const offs = amt - (tellg() % amt);
		if (offs != amt) {
			seekg(offs, std::ios_base::cur);
		}
	}

	void open_fstream(std::streampos position = 0)
	{
		seekg(0, std::ios_base::end);
		m_filesize = tellg();
		setPosition(position);
	}

	void setPosition(std::streampos position)
	{
		if (good() && position <= m_filesize) {
			seekg(position);
		}
	}

	void read_buffer(char* buffer, std::streamsize size) { read(buffer, size); }

	u8 readU8()
	{
		u8 byte = 0;
		read(reinterpret_cast<char*>(&byte), 1);
		return byte;
	}
	u16 readU16()
	{
		u16 v = 0;
		read(reinterpret_cast<char*>(&v), 2);
		return ((v & 0xff00) >> 8) | ((v & 0x00ff) << 8);
	}
	u32 readU32()
	{
		u32 v = 0;
		read(reinterpret_cast<char*>(&v), 4);
		return (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) | ((v & 0x0000ff00) << 8) | ((v & 0x000000ff) << 24));
	}

	s8 readS8()
	{
		char b = 0;
		read(&b, 1);
		return b;
	}
	s16 readS16()
	{
		s16 v = 0;
		read(reinterpret_cast<char*>(&v), 2);
		return ((v & 0xff00) >> 8) | ((v & 0x00ff) << 8);
	}
	s32 readS32()
	{
		s32 v = 0;
		read(reinterpret_cast<char*>(&v), 4);
		return (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) | ((v & 0x0000ff00) << 8) | ((v & 0x000000ff) << 24));
	}

	f32 readF32()
	{
		u32 i = readU32();
		f32 f = NAN;
		std::memcpy(&f, &i, sizeof(f32));
		return f;
	}

private:
	std::streampos m_filesize;
};

} // namespace util

#endif