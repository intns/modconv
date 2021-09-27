#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <types.hpp>
#include <common/vector3.hpp>
#include <common/plane.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct BaseRoomInfo {
    u32 m_index = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct BaseCollTriInfo {
    u32 m_mapCode = 0;
    Vector3i m_indice;
    u16 m_unknown2 = 0;
    u16 m_unknown3 = 0;
    u16 m_unknown4 = 0;
    u16 m_unknown5 = 0;
    Plane m_plane;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct CollTriInfo {
    std::vector<BaseRoomInfo> m_roominfo;
    std::vector<BaseCollTriInfo> m_collinfo;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct CollGroup {
    std::vector<u8> m_unknown1;
    std::vector<u32> m_unknown2;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct CollGrid {
    Vector3f m_boundsMin;
    Vector3f m_boundsMax;
    f32 m_unknown1 = 0;
    u32 m_gridX    = 0;
    u32 m_gridY    = 0;
    std::vector<CollGroup> m_groups;
    std::vector<s32> m_unknown2;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    void clear();
};

#endif