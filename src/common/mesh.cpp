#include "mesh.hpp"

void DisplayList::read(util::fstream_reader& reader)
{
	mFlags.intView = reader.readU32();
	mCommandCount  = reader.readU32();
	mData.resize(reader.readU32());
	reader.align(0x20);
	reader.read(reinterpret_cast<char*>(mData.data()), mData.size());
}

void DisplayList::write(util::fstream_writer& writer)
{
	writer.writeU32(mFlags.intView);
	writer.writeU32(mCommandCount);
	writer.writeU32(static_cast<u32>(mData.size()));
	writer.align(0x20);
	writer.write(reinterpret_cast<char*>(mData.data()), mData.size());
}

void MeshPacket::read(util::fstream_reader& reader)
{
	mIndices.resize(reader.readU32());
	for (s16& index : mIndices) {
		index = reader.readS16();
	}

	mDisplayLists.resize(reader.readU32());
	for (DisplayList& dlist : mDisplayLists) {
		dlist.read(reader);
	}
}

void MeshPacket::write(util::fstream_writer& writer)
{
	writer.writeU32(static_cast<u32>(mIndices.size()));
	for (s16& index : mIndices) {
		writer.writeS16(index);
	}

	writer.writeU32(static_cast<u32>(mDisplayLists.size()));
	for (DisplayList& dlist : mDisplayLists) {
		dlist.write(writer);
	}
}

void Mesh::read(util::fstream_reader& reader)
{
	mBoneIndex     = reader.readU32();
	mVtxDescriptor = reader.readU32();
	mPackets.resize(reader.readU32());
	for (MeshPacket& packet : mPackets) {
		packet.read(reader);
	}
}

void Mesh::write(util::fstream_writer& writer)
{
	writer.writeU32(mBoneIndex);
	writer.writeU32(mVtxDescriptor);
	writer.writeU32(static_cast<u32>(mPackets.size()));
	for (MeshPacket& packet : mPackets) {
		packet.write(writer);
	}
}