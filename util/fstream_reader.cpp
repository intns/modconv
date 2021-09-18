#include <util/fstream_reader.hpp>

namespace util {

fstream_reader::fstream_reader(std::ifstream& file, std::size_t position, Endianness endianness)
    : m_filestream(file)
    , m_endianness(endianness)
{
    file.seekg(0, std::ios_base::end);
    m_filesize = file.tellg();
    setPosition(position);
}

} // namespace util