#include <util/fstream_reader.hpp>
#include <string_view>
#include <optional>
#include <vector>

struct Vector2i {
    u32 x, y;
};

struct Vector2f {
    f32 x, y;
};

struct Vector3i {
    u32 x, y, z;
};

struct Vector3f {
    f32 x, y, z;
};

struct Header {
    struct {
        u16 m_year;
        u8 m_month;
        u8 m_day;
    } m_dateTime;
    u32 m_flags;
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
};