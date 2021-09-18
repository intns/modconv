#include <util/fstream_reader.hpp>

namespace util {

fstream_reader::fstream_reader(std::size_t position, Endianness endianness)
    : m_endianness(endianness)
{
    seekg(0, std::ios_base::end);
    m_filesize = tellg();
    setPosition(position);
}

} // namespace util