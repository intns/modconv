#include <common/plane.hpp>

void Plane::read(util::fstream_reader& reader)
{
	m_normal.read(reader);
	m_distance = reader.readF32();
}

void Plane::write(util::fstream_writer& writer)
{
	m_normal.write(writer);
	writer.writeF32(m_distance);
}
