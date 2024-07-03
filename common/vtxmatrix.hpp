#ifndef VTXMATRIX_HPP
#define VTXMATRIX_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct VtxMatrix {
	u16 mIndex = 0;

	void read(util::fstream_reader&);
	void write(util::fstream_writer&) const;
};

#endif