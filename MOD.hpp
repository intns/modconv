#include <util/fstream_reader.hpp>
#include <string_view>
#include <optional>

struct MOD {
    MOD() = default;
    ~MOD() = default;

    void align(util::fstream_reader& reader, u32 amt);
    void read(util::fstream_reader& reader);

    const std::optional<std::string_view> getChunkName(u32 opcode);
};