#include "fstream_writer.hpp"

namespace util {

fstream_writer::fstream_writer(Endianness endianness)
    : m_endianness(endianness)
{
}

}