#include <array>
#include <optional>
#include <string_view>
#include <util/fstream_reader.hpp>
#include <vector>

struct Vector2i {
    u32 x, y;

    void read(util::fstream_reader&);
};

struct Vector2f {
    f32 x, y;

    void read(util::fstream_reader&);
};

struct Vector3i {
    u32 x, y, z;

    void read(util::fstream_reader&);
};

struct Vector3f {
    f32 x, y, z;

    void read(util::fstream_reader&);
};

struct Color {
    u8 r, g, b, a;

    void read(util::fstream_reader&);
};

struct Header {
    struct {
        u16 m_year;
        u8 m_month;
        u8 m_day;
    } m_dateTime;
    u32 m_flags;
};

struct NBT {
    Vector3f m_normals;
    Vector3f m_binormals;
    Vector3f m_tangent;

    void read(util::fstream_reader&);
};

struct Texture {
    u16 m_width;
    u16 m_height;
    u32 m_format;
    u32 m_unknown;
    std::vector<u8> m_imageData;

    void read(util::fstream_reader&);
};

struct TextureAttributes {
    u16 m_index;
    u16 m_tilingMode;
    u16 m_unknown1;
    f32 m_unknown2;

    void read(util::fstream_reader&);
};

struct MOD {
    MOD() = default;
    MOD(util::fstream_reader& reader) { read(reader); }
    ~MOD() = default;

    void align(util::fstream_reader& reader, u32 amt);
    void read(util::fstream_reader& reader);
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