#include <common.hpp>
#include <common/collision.hpp>

void BaseRoomInfo::read(util::fstream_reader& reader)
{
    m_index = reader.readU32();
}
void BaseRoomInfo::write(util::fstream_writer& writer)
{
    writer.writeU32(m_index);
}

void BaseCollTriInfo::read(util::fstream_reader& reader)
{
    m_mapCode = reader.readU32();
    m_indice.read(reader);

    m_unknown2 = reader.readU16();
    m_unknown3 = reader.readU16();
    m_unknown4 = reader.readU16();
    m_unknown5 = reader.readU16();

    m_plane.read(reader);
}

void BaseCollTriInfo::write(util::fstream_writer& writer)
{
    writer.writeU32(m_mapCode);
    m_indice.write(writer);

    writer.writeU16(m_unknown2);
    writer.writeU16(m_unknown3);
    writer.writeU16(m_unknown4);
    writer.writeU16(m_unknown5);

    m_plane.write(writer);
}

void CollTriInfo::read(util::fstream_reader& reader)
{
    m_collinfo.resize(reader.readU32());
    m_roominfo.resize(reader.readU32());

    reader.align(0x20);
    for (BaseRoomInfo& info : m_roominfo) {
        info.read(reader);
    }
    reader.align(0x20);

    for (BaseCollTriInfo& info : m_collinfo) {
        info.read(reader);
    }
    reader.align(0x20);
}

void CollTriInfo::write(util::fstream_writer& writer)
{
    const u32 start = startChunk(writer, 0x100);
    writer.writeU32(m_collinfo.size());
    writer.writeU32(m_roominfo.size());
    writer.align(0x20);
    for (BaseRoomInfo& info : m_roominfo) {
        info.write(writer);
    }
    writer.align(0x20);
    for (BaseCollTriInfo& info : m_collinfo) {
        info.write(writer);
    }
    finishChunk(writer, start);
}

void CollGroup::read(util::fstream_reader& reader)
{
    m_unknown1.resize(reader.readU16());

    m_unknown2.resize(reader.readU16());
    for (u32& i : m_unknown2) {
        i = reader.readU32();
    }

    for (u8& i : m_unknown1) {
        i = reader.readU8();
    }
}

void CollGroup::write(util::fstream_writer& writer)
{
    writer.writeU16(static_cast<u16>(m_unknown1.size()));
    writer.writeU16(static_cast<u16>(m_unknown2.size()));
    for (u32& i : m_unknown2) {
        writer.writeU32(i);
    }

    for (u8& i : m_unknown1) {
        writer.writeU8(i);
    }
}

void CollGrid::read(util::fstream_reader& reader)
{
    reader.align(0x20);
    m_boundsMin.read(reader);
    m_boundsMax.read(reader);
    m_unknown1 = reader.readF32();
    m_gridX    = reader.readU32();
    m_gridY    = reader.readU32();
    m_groups.resize(reader.readU32());
    for (CollGroup& group : m_groups) {
        group.read(reader);
    }

    for (u32 x = 0; x < m_gridX; x++) {
        for (u32 y = 0; y < m_gridY; y++) {
            m_unknown2.push_back(reader.readS32());
        }
    }
    reader.align(0x20);
}

void CollGrid::write(util::fstream_writer& writer)
{
    const u32 start = startChunk(writer, 0x110);
    writer.align(0x20);
    m_boundsMin.write(writer);
    m_boundsMax.write(writer);
    writer.writeF32(m_unknown1);
    writer.writeU32(m_gridX);
    writer.writeU32(m_gridY);
    writer.writeU32(m_groups.size());
    for (CollGroup& group : m_groups) {
        group.write(writer);
    }
    for (s32& i : m_unknown2) {
        writer.writeS32(i);
    }
    writer.align(0x20);
    finishChunk(writer, start);
}

void CollGrid::clear()
{
    m_boundsMin = { 0, 0, 0 };
    m_boundsMax = { 0, 0, 0 };
    m_unknown1  = 0;
    m_gridX     = 0;
    m_gridY     = 0;
    m_groups.clear();
    m_unknown2.clear();
}
