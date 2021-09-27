#include <common/vtxmatrix.hpp>

void VtxMatrix::read(util::fstream_reader& reader)
{
    m_index = reader.readU16();
}

void VtxMatrix::write(util::fstream_writer& writer)
{
    writer.writeU16(m_index);
}
