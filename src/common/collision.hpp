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

/**
 * @brief Map surface attributes for collision detection.
 */
enum class MapAttributes : s32 {
	Solid = 0,
	Rock  = 1,
	Grass = 2,
	Wood  = 3,
	Mud   = 4,
	Water = 5,
	Hole  = 6,
};

struct BaseCollTriInfo {
	MapAttributes mMapCode   = MapAttributes::Solid;
	u32 mVertexIndexA        = 0;
	u32 mVertexIndexB        = 0;
	u32 mVertexIndexC        = 0;
	u16 mCollRoomId                    = 0;
	s16 mNeighbourIndices[3] = {};
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
	std::vector<u8> mFarCullDistances;
	std::vector<u32> mTriangleIndices;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

struct CollGrid {
	Vector3f mAABBMin;
	Vector3f mAABBMax;
	f32 mCellSize   = 0;
	u32 mCellCountX = 0;
	u32 mCellCountY = 0;
	std::vector<CollGroup> mGroups;
	std::vector<s32> mGroupIndices;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	void clear();
};

#endif