#ifndef NBT_HPP
#define NBT_HPP

#include "vector3.hpp"
#include "../types.hpp"
#include "../util/fstream_reader.hpp"
#include "../util/fstream_writer.hpp"

struct NBT {
	Vector3f mNormal;
	Vector3f mBinormal;
	Vector3f mTangent;

	void read(util::fstream_reader&);
	void write(util::fstream_writer&);
};

#endif