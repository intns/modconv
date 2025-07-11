#ifndef VTXMATRIX_HPP
#define VTXMATRIX_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct VtxMatrix {
	u32 mIndex              = 0;
	bool mHasPartialWeights = false;

	void read(util::fstream_reader&);
	void write(util::fstream_writer&) const;
};

#endif