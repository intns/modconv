#ifndef MOD_HPP
#define MOD_HPP

#include <array>
#include <common.hpp>
#include <optional>
#include <string_view>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>
#include <vector>

/**
 * @brief Enumeration representing the flags for the MOD.
 */
enum class MODFlags : u8 {
    None   = 0x00,
    UseNBT = 0x01,
};

/**
 * @brief Struct representing the header of the MOD.
 */
struct MODHeader {
    struct {
        u16 m_year = 2024;
        u8 m_month = 01;
        u8 m_day   = 26;
    } m_dateTime;

    u32 m_flags = static_cast<u32>(MODFlags::None);
};

/**
 * @brief Struct representing the MOD.
 */
struct MOD {
    /**
     * @brief Enumeration representing the chunk types in the MOD.
     */
    enum class EChunkType {
        Header = 0x00,

        Vertex       = 0x10,
        VertexNormal = 0x11,
        VertexNBT    = 0x12,
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
        CollisionGrid  = 0x110,

        EndOfFile = 0xFFFF
    };

    /**
     * @brief Default constructor for MOD.
     */
    MOD() = default;

    /**
     * @brief Constructor for MOD that reads data from the given reader.
     * @param reader The fstream_reader to read data from.
     */
    MOD(util::fstream_reader& reader) { read(reader); }

    /**
     * @brief Default destructor for MOD.
     */
    ~MOD() = default;

    /**
     * @brief Reads the MOD data from the given reader.
     * @param reader The fstream_reader to read data from.
     */
    void read(util::fstream_reader& reader);

    /**
     * @brief Writes the MOD data to the given writer.
     * @param writer The fstream_writer to write data to.
     */
    void write(util::fstream_writer& writer);

    /**
     * @brief Resets the MOD data to its default state.
     */
    void reset();

    /**
     * @brief Gets the name of the chunk with the given opcode.
     * @param opcode The opcode of the chunk.
     * @return The name of the chunk as an optional string_view.
     */
    static const std::optional<std::string_view> getChunkName(u32 opcode);

    /**
     * @brief Gets the name of the chunk with the given chunk type.
     * @param chunkType The chunk type.
     * @return The name of the chunk as an optional string_view.
     */
    static const std::optional<std::string_view>
    getChunkName(EChunkType chunkType);

    MODHeader m_header;               /**< The header of the MOD. */
    std::vector<Vector3f> m_vertices; /**< The vertices of the MOD. */
    std::vector<Vector3f> m_vnormals; /**< The vertex normals of the MOD. */
    std::vector<NBT> m_vertexnbt;     /**< The vertex NBT data of the MOD. */
    std::vector<ColourU8> m_vcolours; /**< The vertex colours of the MOD. */
    std::array<std::vector<Vector2f>, 8>
        m_texcoords;                 /**< The texture coordinates of the MOD. */
    std::vector<Texture> m_textures; /**< The textures of the MOD. */
    std::vector<TextureAttributes>
        m_texattrs;                /**< The texture attributes of the MOD. */
    MaterialContainer m_materials; /**< The materials of the MOD. */
    std::vector<VtxMatrix> m_vtxMatrix; /**< The vertex matrices of the MOD. */
    std::vector<Envelope> m_envelopes;  /**< The envelopes of the MOD. */
    std::vector<Mesh> m_meshes;         /**< The meshes of the MOD. */
    std::vector<Joint> m_joints;        /**< The joints of the MOD. */
    std::vector<std::string> m_jointNames; /**< The joint names of the MOD. */
    CollTriInfo m_colltris;     /**< The collision triangles of the MOD. */
    CollGrid m_collgrid;        /**< The collision grid of the MOD. */
    std::vector<u8> m_eofBytes; /**< The end-of-file bytes of the MOD. */
};

#endif