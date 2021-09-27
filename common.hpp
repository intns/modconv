#ifndef COMMON_HPP
#define COMMON_HPP

#include <common/collision.hpp>
#include <common/colour.hpp>
#include <common/envelope.hpp>
#include <common/joint.hpp>
#include <common/mesh.hpp>
#include <common/plane.hpp>
#include <common/vector2.hpp>
#include <common/vector3.hpp>
#include <common/vtxmatrix.hpp>
#include <common/nbt.hpp>
#include <common/texture.hpp>
#include <common/material.hpp>

namespace {
inline const u32 startChunk(util::fstream_writer& writer, u32 chunk)
{
    writer.writeU32(chunk);
    const u32 position = static_cast<u32>(writer.tellp());
    writer.writeU32(0);
    return position;
}

inline void finishChunk(util::fstream_writer& writer, u32 chunkStart)
{
    writer.align(0x20);
    const u32 position = static_cast<u32>(writer.tellp());
    writer.seekp(chunkStart, std::ios_base::beg);
    writer.writeU32(position - chunkStart - 4);
    writer.seekp(position, std::ios_base::beg);
}
} // namespace

#endif