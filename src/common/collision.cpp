#include <common.hpp>
#include <common/collision.hpp>

void BaseRoomInfo::read(util::fstream_reader& reader) { mIndex = reader.readU32(); }
void BaseRoomInfo::write(util::fstream_writer& writer) const { writer.writeU32(mIndex); }

void BaseCollTriInfo::read(util::fstream_reader& reader)
{
	mMapCode = reader.readU32();

	mVertexIndexA = reader.readU32();
	mVertexIndexB = reader.readU32();
	mVertexIndexC = reader.readU32();

	mUnknown             = reader.readU16();
	mNeighbourIndices[0] = reader.readU16();
	mNeighbourIndices[1] = reader.readU16();
	mNeighbourIndices[2] = reader.readU16();

	mPlane.read(reader);
}

void BaseCollTriInfo::write(util::fstream_writer& writer)
{
	writer.writeU32(mMapCode);

	writer.writeU32(mVertexIndexA);
	writer.writeU32(mVertexIndexB);
	writer.writeU32(mVertexIndexC);

	writer.writeU16(mUnknown);
	writer.writeU16(mNeighbourIndices[0]);
	writer.writeU16(mNeighbourIndices[1]);
	writer.writeU16(mNeighbourIndices[2]);

	mPlane.write(writer);
}

void CollTriInfo::read(util::fstream_reader& reader)
{
	mCollInfo.resize(reader.readU32());
	mRoomInfo.resize(reader.readU32());

	reader.align(0x20);
	for (BaseRoomInfo& info : mRoomInfo) {
		info.read(reader);
	}
	reader.align(0x20);

	for (BaseCollTriInfo& info : mCollInfo) {
		info.read(reader);
	}
	reader.align(0x20);
}

void CollTriInfo::write(util::fstream_writer& writer)
{
	const u32 start = startChunk(writer, 0x100);
	writer.writeU32(static_cast<u32>(mCollInfo.size()));
	writer.writeU32(static_cast<u32>(mRoomInfo.size()));
	writer.align(0x20);
	for (BaseRoomInfo& info : mRoomInfo) {
		info.write(writer);
	}
	writer.align(0x20);
	for (BaseCollTriInfo& info : mCollInfo) {
		info.write(writer);
	}
	finishChunk(writer, start);
}

void CollGroup::read(util::fstream_reader& reader)
{
	mUnknown1.resize(reader.readU16());
	mUnknown2.resize(reader.readU16());

	for (u32& i : mUnknown2) {
		i = reader.readU32();
	}

	for (u8& i : mUnknown1) {
		i = reader.readU8();
	}
}

void CollGroup::write(util::fstream_writer& writer)
{
	writer.writeU16(static_cast<u16>(mUnknown1.size()));
	writer.writeU16(static_cast<u16>(mUnknown2.size()));
	for (u32& i : mUnknown2) {
		writer.writeU32(i);
	}

	for (u8& i : mUnknown1) {
		writer.writeU8(i);
	}
}

void CollGrid::read(util::fstream_reader& reader)
{
	reader.align(0x20);
	mBoundsMin.read(reader);
	mBoundsMax.read(reader);
	mGridSize  = reader.readF32();
	mGridSizeX = reader.readU32();
	mGridSizeY = reader.readU32();
	m_groups.resize(reader.readU32());
	for (CollGroup& group : m_groups) {
		group.read(reader);
	}

	for (u32 x = 0; x < mGridSizeX; x++) {
		for (u32 y = 0; y < mGridSizeY; y++) {
			m_unknown2.push_back(reader.readS32());
		}
	}
	reader.align(0x20);
}

void CollGrid::write(util::fstream_writer& writer)
{
	const u32 start = startChunk(writer, 0x110);
	writer.align(0x20);
	mBoundsMin.write(writer);
	mBoundsMax.write(writer);
	writer.writeF32(mGridSize);
	writer.writeU32(mGridSizeX);
	writer.writeU32(mGridSizeY);

	writer.writeU32(static_cast<u32>(m_groups.size()));
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
	mBoundsMin = { 0, 0, 0 };
	mBoundsMax = { 0, 0, 0 };
	mGridSize  = 0;
	mGridSizeX = 0;
	mGridSizeY = 0;
	m_groups.clear();
	m_unknown2.clear();
}
