#ifndef MESH_HPP
#define MESH_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

enum class DLCullMode { Front = 0, Back = 1, Both = 2, None = 3 };

struct DisplayList {
	union {
		struct {
			char b1 : 8;
			char b2 : 8;
			char b3 : 8;
			DLCullMode cullMode : 8;
		} byteView;

		int intView;
	} mFlags = { 0 };

	u32 mCommandCount = 0;
	std::vector<u8> mData;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

struct MeshPacket {
	std::vector<s16> mIndices;
	std::vector<DisplayList> mDisplayLists;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

struct Mesh {
	u32 mBoneIndex     = 0;
	u32 mVtxDescriptor = 0;
	std::vector<MeshPacket> mPackets;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

#endif