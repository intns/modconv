#include <common/plane.hpp>

void Plane::read(util::fstream_reader& reader)
{
    m_position.read(reader);
    m_diameter = reader.readF32();
}

void Plane::write(util::fstream_writer& writer)
{
    m_position.write(writer);
    writer.writeF32(m_diameter);
}
