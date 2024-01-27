#ifndef MESH_HPP
#define MESH_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

class MeshPacket {
public:
    std::vector<u16> indices;
    std::vector<DisplayList> displaylists;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

enum class DLCullMode { Front = 0, Back = 1, Both = 2, None = 3 };

struct DisplayList {
    union {
        struct {
            char b1 : 8;
            char b2 : 8;
            char b3 : 8;
            DLCullMode cullMode : 8;
        } byteView;

        int intView;
    } m_flags = { 0 };

    u32 m_cmdCount = 0;
    std::vector<u8> m_data;

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