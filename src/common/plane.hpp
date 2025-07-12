#ifndef PLANE_HPP
#define PLANE_HPP

#include "vector3.hpp"
#include "../types.hpp"
#include "../util/fstream_reader.hpp"
#include "../util/fstream_writer.hpp"

struct Plane {
	Vector3f mNormal;
	f32 mDistance = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

#endif