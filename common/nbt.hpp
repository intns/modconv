#ifndef NBT_HPP
#define NBT_HPP

#include <common/vector3.hpp>
#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct NBT {
    Vector3f m_normals;
    Vector3f m_binormals;
    Vector3f m_tangent;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

#endif