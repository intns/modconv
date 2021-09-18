#include <MOD.hpp>

int main(int argc, char** argv)
{
    std::ifstream mfStream("mizu.mod", std::ios_base::binary);
    if (!mfStream.is_open()) {
        return EXIT_FAILURE;
    }

    MOD mod;
    util::fstream_reader reader(mfStream, 0, util::fstream_reader::Endianness::Big);
    mod.read(reader);

    return EXIT_SUCCESS;
}