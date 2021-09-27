#include <common/mesh.hpp>


void DisplayList::read(util::fstream_reader& reader)
{
    m_flags.intView = reader.readU32();
    m_cmdCount      = reader.readU32();
    m_dlistData.resize(reader.readU32());
    reader.align(0x20);
    reader.read(reinterpret_cast<char*>(m_dlistData.data()),
                m_dlistData.size());
}

void DisplayList::write(util::fstream_writer& writer)
{
    writer.writeU32(m_flags.intView);
    writer.writeU32(m_cmdCount);
    writer.writeU32(m_dlistData.size());
    writer.align(0x20);
    writer.write(reinterpret_cast<char*>(m_dlistData.data()),
                 m_dlistData.size());
}

void MeshPacket::read(util::fstream_reader& reader)
{
    m_indices.resize(reader.readU32());
    for (u16& index : m_indices) {
        index = reader.readU16();
    }

    m_displaylists.resize(reader.readU32());
    for (DisplayList& dlist : m_displaylists) {
        dlist.read(reader);
    }
}

void MeshPacket::write(util::fstream_writer& writer)
{
    writer.writeU32(m_indices.size());
    for (u16& index : m_indices) {
        writer.writeU16(index);
    }

    writer.writeU32(m_displaylists.size());
    for (DisplayList& dlist : m_displaylists) {
        dlist.write(writer);
    }
}

void Mesh::read(util::fstream_reader& reader)
{
    m_boneIndex     = reader.readU32();
    m_vtxDescriptor = reader.readU32();
    m_packets.resize(reader.readU32());
    for (MeshPacket& packet : m_packets) {
        packet.read(reader);
    }
}

void Mesh::write(util::fstream_writer& writer)
{
    writer.writeU32(m_boneIndex);
    writer.writeU32(m_vtxDescriptor);
    writer.writeU32(m_packets.size());
    for (MeshPacket& packet : m_packets) {
        packet.write(writer);
    }
}