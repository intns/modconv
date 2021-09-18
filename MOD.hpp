#include <util/fstream_reader.hpp>

struct MOD {
    MOD() = default;
    ~MOD() = default;

    void align(util::fstream_reader& reader, u32 amt);
    void read(util::fstream_reader& reader);
};