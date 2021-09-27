#ifndef MOD_HPP
#define MOD_HPP

#include <array>
#include <common.hpp>
#include <optional>
#include <string_view>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>
#include <vector>

struct MODHeader {
    struct {
        u16 m_year = 2021;
        u8 m_month = 9;
        u8 m_day   = 18;
    } m_dateTime;

    u32 m_flags = 0;
};

enum class MODFlags : u8 { UseNBT = 0x01 };

struct MOD {
    MOD() = default;
    MOD(util::fstream_reader& reader) { read(reader); }
    ~MOD() = default;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    void reset();

    static const std::optional<std::string_view> getChunkName(u32 opcode);

    MODHeader m_header;
    std::vector<Vector3f> m_vertices;
    std::vector<Vector3f> m_vnormals;
    std::vector<NBT> m_vertexnbt;
    std::vector<ColourU8> m_vcolours;
    std::array<std::vector<Vector2f>, 8> m_texcoords;
    std::vector<Texture> m_textures;
    std::vector<TextureAttributes> m_texattrs;
    MaterialContainer m_materials;
    std::vector<VtxMatrix> m_vtxMatrix;
    std::vector<Envelope> m_envelopes;
    std::vector<Mesh> m_meshes;
    std::vector<Joint> m_joints;
    std::vector<std::string> m_jointNames;
    CollTriInfo m_colltris;
    CollGrid m_collgrid;
    std::vector<u8> m_eofBytes;
};

#endif