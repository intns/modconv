#include <array>
#include <optional>
#include <string_view>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>
#include <vector>

struct Vector2i {
    u32 x = 0, y = 0;

    void read(util::fstream_reader&);
};

struct Vector2f {
    f32 x = 0, y = 0;

    void read(util::fstream_reader&);
};

struct Vector3i {
    u32 x = 0, y = 0, z = 0;

    void read(util::fstream_reader&);
};

struct Vector3f {
    f32 x = 0, y = 0, z = 0;

    void read(util::fstream_reader&);
};

struct Color {
    u8 r = 0, g = 0, b = 0, a = 0;

    void read(util::fstream_reader&);
};

struct Header {
    struct {
        u16 m_year = 2021;
        u8 m_month = 9;
        u8 m_day = 18;
    } m_dateTime;

    u32 m_flags = 0;
};

struct NBT {
    Vector3f m_normals;
    Vector3f m_binormals;
    Vector3f m_tangent;

    void read(util::fstream_reader&);
};

struct Texture {
    u16 m_width = 0;
    u16 m_height = 0;
    u32 m_format = 0;
    u32 m_unknown = 0;
    std::vector<u8> m_imageData;

    void read(util::fstream_reader&);
};

struct TextureAttributes {
    u16 m_index = 0;
    u16 m_tilingMode = 0;
    u16 m_unknown1 = 0;
    f32 m_unknown2 = 0;

    void read(util::fstream_reader&);
};

enum class MODFlags : u8{
    UseNBT = 0x01
};

struct MOD {
    MOD() = default;
    MOD(util::fstream_reader& reader) { read(reader); }
    ~MOD() = default;

    void align(util::fstream_reader& reader, u32 amt);
    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    void reset();

    const std::optional<std::string_view> getChunkName(u32 opcode);

    Header m_header;
    std::vector<Vector3f> m_vertices;
    std::vector<Vector3f> m_vnormals;
    std::vector<NBT> m_vertexnbt;
    std::vector<Color> m_vcolors;
    std::array<std::vector<Vector2f>, 8> m_texcoords;
    std::vector<Texture> m_textures;
    std::vector<TextureAttributes> m_texattrs;
};