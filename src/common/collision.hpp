#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "../types.hpp"
#include "../common/vector3.hpp"
#include "../common/plane.hpp"
#include "../util/fstream_reader.hpp"
#include "../util/fstream_writer.hpp"

struct BaseRoomInfo {
	u32 mIndex = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
};

struct BaseCollTriInfo {
	u32 mMapCode             = 0;
	u32 mVertexIndexA        = 0;
	u32 mVertexIndexB        = 0;
	u32 mVertexIndexC        = 0;
	u16 mUnknown             = 0;
	u16 mNeighbourIndices[3] = {};
	Plane mPlane;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

struct CollTriInfo {
	std::vector<BaseRoomInfo> mRoomInfo;
	std::vector<BaseCollTriInfo> mCollInfo;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

struct CollGroup {
	std::vector<u8> mUnknown1;
	std::vector<u32> mUnknown2;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

struct CollGrid {
	Vector3f mBoundsMin;
	Vector3f mBoundsMax;
	f32 mGridSize  = 0;
	u32 mGridSizeX = 0;
	u32 mGridSizeY = 0;
	std::vector<CollGroup> m_groups;
	std::vector<s32> m_unknown2;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	void clear();
};

#endif