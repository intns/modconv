#include <common/nbt.hpp>

void NBT::read(util::fstream_reader& reader)
{
    m_normals.read(reader);
    m_binormals.read(reader);
    m_tangent.read(reader);
}

void NBT::write(util::fstream_writer& writer)
{
    m_normals.write(writer);
    m_binormals.write(writer);
    m_tangent.write(writer);
}