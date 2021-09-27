#ifndef MESH_HPP
#define MESH_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct DisplayList {
    union {
        // THANKS:
        // https://github.com/KillzXGaming/010-Templates/blob/816cfc57e2ee998b953cf488e4fed25c54e7861a/Pikmin/MOD.bt#L312
        struct {
            char b1 : 8;
            char b2 : 8;
            char b3 : 8;
            char cullMode : 8;
        } byteView;

        int intView;
    } m_flags = { 0 };

    // THANKS: Yoshi2's mod2obj
    u32 m_cmdCount = 0;
    std::vector<u8> m_dlistData;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct MeshPacket {
    std::vector<u16> m_indices;
    std::vector<DisplayList> m_displaylists;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct Mesh {
    u32 m_boneIndex     = 0;
    u32 m_vtxDescriptor = 0;
    std::vector<MeshPacket> m_packets;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};


#endif