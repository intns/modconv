#include <util/vector_reader.hpp>

namespace util {

vector_reader::vector_reader(Endianness endianness)
    : m_buffer({ 0 })
    , m_position(0)
    , m_endianness(endianness)
{
}

vector_reader::vector_reader(std::vector<u8> bytes, std::size_t position, Endianness endianness)
    : m_buffer(bytes)
    , m_position(position)
    , m_endianness(endianness)
{
}

} // namespace util