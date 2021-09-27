#include <common/joint.hpp>

void JointMatPoly::read(util::fstream_reader& reader)
{
    m_matIdx  = reader.readU16();
    m_meshIdx = reader.readU16();
}

void JointMatPoly::write(util::fstream_writer& writer)
{
    writer.writeU16(m_matIdx);
    writer.writeU16(m_meshIdx);
}

void Joint::read(util::fstream_reader& reader)
{
    m_parentIdx = reader.readU32();
    m_flags     = reader.readU32();
    m_boundsMax.read(reader);
    m_boundsMin.read(reader);
    m_volumeRadius = reader.readF32();
    m_scale.read(reader);
    m_rotation.read(reader);
    m_position.read(reader);
    m_matpolys.resize(reader.readU32());
    for (JointMatPoly& poly : m_matpolys) {
        poly.read(reader);
    }
}

void Joint::write(util::fstream_writer& writer)
{
    writer.writeU32(m_parentIdx);
    writer.writeU32(m_flags);
    m_boundsMax.write(writer);
    m_boundsMin.write(writer);
    writer.writeF32(m_volumeRadius);
    m_scale.write(writer);
    m_rotation.write(writer);
    m_position.write(writer);
    writer.writeU32(m_matpolys.size());
    for (JointMatPoly& poly : m_matpolys) {
        poly.write(writer);
    }
}
