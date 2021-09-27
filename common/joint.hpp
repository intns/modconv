#ifndef JOINT_HPP
#define JOINT_HPP

#include <common/vector3.hpp>
#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct JointMatPoly {
    s16 m_matIdx  = 0;
    s16 m_meshIdx = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct Joint {
    s32 m_parentIdx = 0;
    u32 m_flags     = 0;
    Vector3f m_boundsMin;
    Vector3f m_boundsMax;
    f32 m_volumeRadius = 0;
    Vector3f m_scale;
    Vector3f m_rotation;
    Vector3f m_position;
    std::vector<JointMatPoly> m_matpolys;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

#endif