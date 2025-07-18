#include "common.hpp"
#include "collision.hpp"

void BaseRoomInfo::read(util::fstream_reader& reader) { mIndex = reader.readU32(); }
void BaseRoomInfo::write(util::fstream_writer& writer) const { writer.writeU32(mIndex); }

void BaseCollTriInfo::read(util::fstream_reader& reader)
{
	mMapCode = static_cast<MapAttributes>(reader.readU32());

	mVertexIndexA = reader.readU32();
	mVertexIndexB = reader.readU32();
	mVertexIndexC = reader.readU32();

	mCollRoomId          = reader.readU16();
	mNeighbourIndices[0] = reader.readS16();
	mNeighbourIndices[1] = reader.readS16();
	mNeighbourIndices[2] = reader.readS16();

	mPlane.read(reader);
}

void BaseCollTriInfo::write(util::fstream_writer& writer)
{
	writer.writeS32(static_cast<s32>(mMapCode));

	writer.writeU32(mVertexIndexA);
	writer.writeU32(mVertexIndexB);
	writer.writeU32(mVertexIndexC);

	writer.writeU16(mCollRoomId);
	writer.writeS16(mNeighbourIndices[0]);
	writer.writeS16(mNeighbourIndices[1]);
	writer.writeS16(mNeighbourIndices[2]);

	mPlane.write(writer);
}

void CollTriInfo::read(util::fstream_reader& reader)
{
	mCollInfo.resize(reader.readU32());
	mRoomInfo.resize(reader.readU32());

	reader.align();

	for (BaseRoomInfo& info : mRoomInfo) {
		info.read(reader);
	}

	reader.align();

	for (BaseCollTriInfo& info : mCollInfo) {
		info.read(reader);
	}

	reader.align();
}

void CollTriInfo::write(util::fstream_writer& writer)
{
	const u32 start = startChunk(writer, 0x100);
	writer.writeU32(static_cast<u32>(mCollInfo.size()));
	writer.writeU32(static_cast<u32>(mRoomInfo.size()));

	writer.align();

	for (BaseRoomInfo const& info : mRoomInfo) {
		info.write(writer);
	}

	writer.align();

	for (BaseCollTriInfo& info : mCollInfo) {
		info.write(writer);
	}

	finishChunk(writer, start);
}

void CollGroup::read(util::fstream_reader& reader)
{
	mFarCullDistances.resize(reader.readU16());
	mTriangleIndices.resize(reader.readU16());

	for (u32& index : mTriangleIndices) {
		index = reader.readU32();
	}

	for (u8& distance : mFarCullDistances) {
		distance = reader.readU8();
	}
}

void CollGroup::write(util::fstream_writer& writer)
{
	writer.writeU16(static_cast<u16>(mFarCullDistances.size()));
	writer.writeU16(static_cast<u16>(mTriangleIndices.size()));

	for (u32 const& i : mTriangleIndices) {
		writer.writeU32(i);
	}

	for (u8 const& i : mFarCullDistances) {
		writer.writeU8(i);
	}
}

void CollGrid::read(util::fstream_reader& reader)
{
	reader.align();

	mAABBMin.read(reader);
	mAABBMax.read(reader);

	mCellSize   = reader.readF32();
	mCellCountX = reader.readU32();
	mCellCountY = reader.readU32();

	mGroups.resize(reader.readU32());
	for (CollGroup& group : mGroups) {
		group.read(reader);
	}

	mGroupIndices.reserve(mCellCountX * mCellCountY);
	for (u32 y = 0; y < mCellCountY; y++) {
		for (u32 x = 0; x < mCellCountX; x++) {
			mGroupIndices.push_back(reader.readS32());
		}
	}

	reader.align();
}

void CollGrid::write(util::fstream_writer& writer)
{
	const u32 start = startChunk(writer, 0x110);
	writer.align();
	mAABBMin.write(writer);
	mAABBMax.write(writer);
	writer.writeF32(mCellSize);
	writer.writeU32(mCellCountX);
	writer.writeU32(mCellCountY);

	writer.writeU32(static_cast<u32>(mGroups.size()));
	for (CollGroup& group : mGroups) {
		group.write(writer);
	}

	for (s32 const& i : mGroupIndices) {
		writer.writeS32(i);
	}

	writer.align();
	finishChunk(writer, start);
}

void CollGrid::clear()
{
	mAABBMin    = { 0, 0, 0 };
	mAABBMax    = { 0, 0, 0 };
	mCellSize   = 0;
	mCellCountX = 0;
	mCellCountY = 0;
	mGroups.clear();
	mGroupIndices.clear();
}
