#ifndef MOD_HPP
#define MOD_HPP

#include <array>
#include <common.hpp>
#include <optional>
#include <string_view>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>
#include <vector>

enum class MODFlags : u8 { None = 0x00, UseNBT = 0x01 };

struct MODHeader {
    struct {
        u16 m_year = 2021;
        u8 m_month = 10;
        u8 m_day   = 11;
    } m_dateTime;

    u32 m_flags = static_cast<u32>(MODFlags::None);
};

struct MOD {
    enum class EChunkType {
        Header = 0x00,

        Vertex        = 0x10,
        VertexNormal  = 0x11,
        VertexNBT     = 0x12,
        VertexColour = 0x13,

        TexCoord0 = 0x18,
        TexCoord1 = 0x19,
        TexCoord2 = 0x1A,
        TexCoord3 = 0x1B,
        TexCoord4 = 0x1C,
        TexCoord5 = 0x1D,
        TexCoord6 = 0x1E,
        TexCoord7 = 0x1F,

        Texture          = 0x20,
        TextureAttribute = 0x22,

        Material = 0x30,

        VertexMatrix   = 0x40,
        MatrixEnvelope = 0x41,

        Mesh      = 0x50,
        Joint     = 0x60,
        JointName = 0x61,

        CollisionPrism = 0x100,
        CollisionGrid = 0x110,

        EndOfFile = 0xFFFF
    };

    MOD() = default;
    MOD(util::fstream_reader& reader) { read(reader); }
    ~MOD() = default;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    void reset();

    static const std::optional<std::string_view> getChunkName(u32 opcode);
    static const std::optional<std::string_view> getChunkName(EChunkType chunkType);

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