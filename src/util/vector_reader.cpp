#include "vector_reader.hpp"
#include <utility>

namespace util {

vector_reader::vector_reader(Endianness endianness)
    : m_buffer({ 0 })
    , m_position(0)
    , m_endianness(endianness)
{
}

vector_reader::vector_reader(std::vector<u8> bytes, std::size_t position, Endianness endianness)
    : m_buffer(std::move(bytes))
    , m_position(position)
    , m_endianness(endianness)
{
}

} // namespace util