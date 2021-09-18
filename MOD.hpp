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

struct MOD {
    MOD() = default;
    ~MOD() = default;

    void align(util::fstream_reader& reader, u32 amt);
    void read(util::fstream_reader& reader);

    const std::optional<std::string_view> getChunkName(u32 opcode);

    Header m_header;
    std::vector<Vector3f> m_vertices;
    std::vector<Vector3f> m_vnormals;
    std::vector<NBT> m_vertexnbt;
};