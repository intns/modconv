#ifndef PLANE_HPP
#define PLANE_HPP

#include <common/vector3.hpp>
#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct Plane {
    Vector3f m_position;
    f32 m_diameter = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

#endif