#ifndef COMMON_HPP
#define COMMON_HPP

#include "common/collision.hpp"
#include "common/colour.hpp"
#include "common/envelope.hpp"
#include "common/joint.hpp"
#include "common/material.hpp"
#include "common/mesh.hpp"
#include "common/nbt.hpp"
#include "common/plane.hpp"
#include "common/texture.hpp"
#include "common/vector2.hpp"
#include "common/vector3.hpp"
#include "common/vtxmatrix.hpp"
#include "common/dlist_reader.hpp"
#include "common/material_serializer.hpp"
#include "common/collision_serializer.hpp"
#include "util/fstream_writer.hpp"

namespace {
inline u32 startChunk(util::fstream_writer& writer, u32 chunk)
{
	writer.writeU32(chunk);
	const u32 position = static_cast<u32>(writer.tellp());
	writer.writeU32(0);
	return position;
}

inline void finishChunk(util::fstream_writer& writer, u32 chunkStart)
{
	writer.align();
	const u32 position = static_cast<u32>(writer.tellp());
	writer.seekp(chunkStart, std::ios_base::beg);
	writer.writeU32(position - chunkStart - 4);
	writer.seekp(position, std::ios_base::beg);
}
} // namespace

#endif