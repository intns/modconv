#ifndef COLOUR_HPP
#define COLOUR_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

template <typename T>
struct ColourBase {
	T r = 0, g = 0, b = 0, a = 255;

	virtual void read(util::fstream_reader&)  = 0;
	virtual void write(util::fstream_writer&) = 0;
	friend std::ostream& operator<<(std::ostream& os, ColourBase const& c)
	{
		os << static_cast<u32>(c.r) << " " << static_cast<u32>(c.g) << " " << static_cast<u32>(c.b) << " " << static_cast<u32>(c.a);
		return os;
	}
};

struct ColourU8 : public ColourBase<u8> {
	void read(util::fstream_reader&) override;
	void write(util::fstream_writer&) override;
};

struct ColourU16 : public ColourBase<u16> {
	void read(util::fstream_reader&) override;
	void write(util::fstream_writer&) override;
};

#endif