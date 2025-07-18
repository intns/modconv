#ifndef MESH_HPP
#define MESH_HPP

#include "../types.hpp"
#include "../util/fstream_reader.hpp"
#include "../util/fstream_writer.hpp"

enum class DLFlags : u32 {
	Front            = 1 << 0,
	Back             = 1 << 1,
	Both             = Front | Back,
	
	UseTriangleStrip = 1 << 24,
};

struct DisplayList {
	DLFlags mFlags;
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

enum VCD : u32 {
	MatrixIndex    = 1 << 0, // 0x001 - GX_VA_PNMTXIDX
	TexMatrixIndex = 1 << 1, // 0x002 - GX_VA_TEX1MTXIDX
	Color0         = 1 << 2, // 0x004 - GX_VA_CLR0
	Tex0           = 1 << 3, // 0x008 - GX_VA_TEX0
	Tex1           = 1 << 4, // 0x010 - GX_VA_TEX1
	Tex2           = 1 << 5, // 0x020 - GX_VA_TEX2
	Tex3           = 1 << 6, // 0x040 - GX_VA_TEX3
	Tex4           = 1 << 7, // 0x080 - GX_VA_TEX4
	Tex5           = 1 << 8, // 0x100 - GX_VA_TEX5
	Tex6           = 1 << 9, // 0x200 - GX_VA_TEX6
	Tex7           = 1 << 10 // 0x400 - GX_VA_TEX7
};

struct Mesh {
	u32 mBoneIndex     = 0;
	u32 mVtxDescriptor = 0;
	std::vector<MeshPacket> mPackets;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

#endif