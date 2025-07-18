#ifndef COLLISION_SERIALIZER_HPP
#define COLLISION_SERIALIZER_HPP

#pragma once

#include "serialization_base.hpp"
#include "serialization_utils.hpp"
#include "common.hpp"
#include <unordered_map>

using namespace serialization;

namespace collision {

struct MapCodeBitfield {
	u32 raw_value;

	// Extract attribute (bits 29-31, top 3 bits)
	int getAttribute() const { return (raw_value >> 29) & 0x7; }

	// Extract slip code (bits 27-28, 2 bits)
	int getSlipCode() const { return (raw_value >> 27) & 0x3; }

	// Extract bald flag (bit 25, 1 bit)
	bool getBaldFlag() const { return (raw_value >> 25) & 0x1; }

	// Get remaining bits (0-24) for other data
	u32 getOtherData() const { return raw_value & 0x1FFFFFF; }

	// Set attribute (bits 29-31)
	void setAttribute(int attr) { raw_value = (raw_value & 0x1FFFFFFF) | ((attr & 0x7) << 29); }

	// Set slip code (bits 27-28)
	void setSlipCode(int slip) { raw_value = (raw_value & 0xE7FFFFFF) | ((slip & 0x3) << 27); }

	// Set bald flag (bit 25)
	void setBaldFlag(bool bald) { raw_value = (raw_value & 0xFDFFFFFF) | ((bald ? 1 : 0) << 25); }

	// Set other data (bits 0-24)
	void setOtherData(u32 data) { raw_value = (raw_value & 0xFE000000) | (data & 0x1FFFFFF); }

	MapCodeBitfield(u32 value = 0)
	    : raw_value(value)
	{
	}
};

// Enum conversion helpers
namespace EnumConverters {

inline std::string MapAttributesToString(int attr)
{
	switch (attr) {
	case 0:
		return "Solid";
	case 1:
		return "Rock";
	case 2:
		return "Grass";
	case 3:
		return "Wood";
	case 4:
		return "Mud";
	case 5:
		return "Water";
	case 6:
		return "Hole";
	default:
		return std::to_string(attr);
	}
}

inline std::string SlipCodeToString(int slip)
{
	switch (slip) {
	case 0:
		return "NoSlip";
	case 1:
		return "WeakSlip";
	case 2:
		return "StrongSlip";
	default:
		return std::to_string(slip);
	}
}

inline int StringToMapAttributes(const std::string& str)
{
	if (str == "Solid")
		return 0;
	if (str == "Rock")
		return 1;
	if (str == "Grass")
		return 2;
	if (str == "Wood")
		return 3;
	if (str == "Mud")
		return 4;
	if (str == "Water")
		return 5;
	if (str == "Hole")
		return 6;
	try {
		return std::stoi(str);
	} catch (...) {
		return 0;
	}
}

inline int StringToSlipCode(const std::string& str)
{
	if (str == "NoSlip")
		return 0;
	if (str == "WeakSlip")
		return 1;
	if (str == "StrongSlip")
		return 2;
	try {
		return std::stoi(str);
	} catch (...) {
		return 0;
	}
}

} // namespace EnumConverters

// Serializable wrapper for Plane
class SerializablePlane : public ISerializable {
public:
	Plane data;

	void serialize(ISerializer& s) const override
	{
		serializeVec3(s, "normal", data.mNormal);
		s.write("distance", data.mDistance);
	}

	void deserialize(IDeserializer& d) override
	{
		deserializeVec3(d, "normal", data.mNormal);
		d.read("distance", data.mDistance);
	}
};

// Serializable wrapper for BaseRoomInfo
class SerializableBaseRoomInfo : public ISerializable {
public:
	BaseRoomInfo data;

	void serialize(ISerializer& s) const override { s.writeU32("index", data.mIndex); }

	void deserialize(IDeserializer& d) override { d.readU32("index", data.mIndex); }
};

// Serializable wrapper for BaseCollTriInfo
class SerializableBaseCollTriInfo : public ISerializable {
public:
	BaseCollTriInfo data;

	void serialize(ISerializer& s) const override
	{
		// Decode the map code bitfield
		MapCodeBitfield mapCode(static_cast<u32>(data.mMapCode));

		s.writeComment("Map code breakdown:");
		s.writeComment("surface_type options: Solid, Rock, Grass, Wood, Mud, Water, Hole");
		s.write("surface_type", EnumConverters::MapAttributesToString(mapCode.getAttribute()));
		s.writeComment("slip_code options: NoSlip, WeakSlip, StrongSlip");
		s.write("slip_code", EnumConverters::SlipCodeToString(mapCode.getSlipCode()));
		s.writeComment("bald_flag: true = Pikmin can't be planted here, false = Pikmin can be planted here");
		s.write("bald_flag", mapCode.getBaldFlag());
		s.writeComment("other_data: custom data (0-33554431), raw_map_code: direct hex editing");
		s.writeU32("other_data", mapCode.getOtherData());
		s.writeU32("raw_map_code", mapCode.raw_value);

		s.writeU32("vertex_index_a", data.mVertexIndexA);
		s.writeU32("vertex_index_b", data.mVertexIndexB);
		s.writeU32("vertex_index_c", data.mVertexIndexC);
		s.write("collision_room_id", static_cast<int>(data.mCollRoomId));

		s.beginArray("neighbour_indices");
		for (int i = 0; i < 3; ++i) {
			s.writeValue(static_cast<int>(data.mNeighbourIndices[i]));
		}
		s.endArray();

		s.beginObject("plane");
		SerializablePlane plane;
		plane.data = data.mPlane;
		plane.serialize(s);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		// Try to read individual components first
		MapCodeBitfield mapCode;
		std::string surfaceTypeStr, slipCodeStr;
		bool baldFlag  = false;
		u32 otherData  = 0;
		u32 rawMapCode = 0;

		if (d.read("surface_type", surfaceTypeStr)) {
			mapCode.setAttribute(EnumConverters::StringToMapAttributes(surfaceTypeStr));
		}

		if (d.read("slip_code", slipCodeStr)) {
			mapCode.setSlipCode(EnumConverters::StringToSlipCode(slipCodeStr));
		}

		if (d.read("bald_flag", baldFlag)) {
			mapCode.setBaldFlag(baldFlag);
		}

		if (d.readU32("other_data", otherData)) {
			mapCode.setOtherData(otherData);
		}

		// If raw_map_code is provided, use it instead (for direct editing)
		if (d.readU32("raw_map_code", rawMapCode)) {
			mapCode.raw_value = rawMapCode;
		}

		data.mMapCode = static_cast<MapAttributes>(mapCode.raw_value);

		d.readU32("vertex_index_a", data.mVertexIndexA);
		d.readU32("vertex_index_b", data.mVertexIndexB);
		d.readU32("vertex_index_c", data.mVertexIndexC);

		int roomId;
		if (d.read("collision_room_id", roomId)) {
			data.mCollRoomId = static_cast<u16>(roomId);
		}

		if (d.enterArray("neighbour_indices")) {
			size_t count = std::min(d.getArraySize(), static_cast<size_t>(3));
			for (size_t i = 0; i < count; ++i) {
				int neighbour;
				if (d.readArrayValue(neighbour)) {
					data.mNeighbourIndices[i] = static_cast<u16>(neighbour);
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		if (d.enterObject("plane")) {
			SerializablePlane plane;
			plane.deserialize(d);
			data.mPlane = plane.data;
			d.exitObject();
		}
	}
};

// Serializable wrapper for CollTriInfo
class SerializableCollTriInfo : public ISerializable {
public:
	CollTriInfo data;

	void serialize(ISerializer& s) const override
	{
		s.beginArray("room_info");
		for (const auto& room : data.mRoomInfo) {
			SerializableBaseRoomInfo wrapper;
			wrapper.data = room;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();

		s.beginArray("collision_triangles");
		for (const auto& tri : data.mCollInfo) {
			SerializableBaseCollTriInfo wrapper;
			wrapper.data = tri;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		// Deserialize room info
		data.mRoomInfo.clear();
		if (d.enterArray("room_info")) {
			size_t count = d.getArraySize();
			data.mRoomInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableBaseRoomInfo wrapper;
					wrapper.deserialize(d);
					data.mRoomInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// Deserialize collision triangles
		data.mCollInfo.clear();
		if (d.enterArray("collision_triangles")) {
			size_t count = d.getArraySize();
			data.mCollInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableBaseCollTriInfo wrapper;
					wrapper.deserialize(d);
					data.mCollInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

// Serializable wrapper for CollGroup
class SerializableCollGroup : public ISerializable {
public:
	CollGroup data;

	void serialize(ISerializer& s) const override
	{
		s.beginArray("far_cull_distances");
		for (const auto& distance : data.mFarCullDistances) {
			s.writeValue(static_cast<int>(distance));
		}
		s.endArray();

		s.beginArray("triangle_indices");
		for (const auto& index : data.mTriangleIndices) {
			s.writeValue(static_cast<int>(index));
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		// Deserialize far cull distances
		data.mFarCullDistances.clear();
		if (d.enterArray("far_cull_distances")) {
			size_t count = d.getArraySize();
			data.mFarCullDistances.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				int distance;
				if (d.readArrayValue(distance)) {
					data.mFarCullDistances.push_back(static_cast<u8>(distance));
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// Deserialize triangle indices
		data.mTriangleIndices.clear();
		if (d.enterArray("triangle_indices")) {
			size_t count = d.getArraySize();
			data.mTriangleIndices.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				int index;
				if (d.readArrayValue(index)) {
					data.mTriangleIndices.push_back(static_cast<u32>(index));
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

// Serializable wrapper for CollGrid
class SerializableCollGrid : public ISerializable {
public:
	CollGrid data;

	void serialize(ISerializer& s) const override
	{
		serializeVec3(s, "aabb_min", data.mAABBMin);
		serializeVec3(s, "aabb_max", data.mAABBMax);
		s.write("cell_size", data.mCellSize);
		s.writeU32("cell_count_x", data.mCellCountX);
		s.writeU32("cell_count_y", data.mCellCountY);

		s.beginArray("groups");
		for (const auto& group : data.mGroups) {
			SerializableCollGroup wrapper;
			wrapper.data = group;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();

		s.beginArray("group_indices");
		for (const auto& index : data.mGroupIndices) {
			s.writeValue(static_cast<int>(index));
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		deserializeVec3(d, "aabb_min", data.mAABBMin);
		deserializeVec3(d, "aabb_max", data.mAABBMax);
		d.read("cell_size", data.mCellSize);
		d.readU32("cell_count_x", data.mCellCountX);
		d.readU32("cell_count_y", data.mCellCountY);

		// Deserialize groups
		data.mGroups.clear();
		if (d.enterArray("groups")) {
			size_t count = d.getArraySize();
			data.mGroups.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableCollGroup wrapper;
					wrapper.deserialize(d);
					data.mGroups.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// Deserialize group indices
		data.mGroupIndices.clear();
		if (d.enterArray("group_indices")) {
			size_t count = d.getArraySize();
			data.mGroupIndices.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				int index;
				if (d.readArrayValue(index)) {
					data.mGroupIndices.push_back(static_cast<s32>(index));
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

// Main collision data serializer
class SerializableCollisionData : public ISerializable {
public:
	CollTriInfo triangles;
	CollGrid grid;

	void serialize(ISerializer& s) const override
	{
		s.writeComment("Collision Data Export");
		s.writeComment("Contains collision triangles and spatial grid information");

		s.beginObject("collision_grid");
		SerializableCollGrid gridWrapper;
		gridWrapper.data = grid;
		gridWrapper.serialize(s);
		s.endObject();

		s.writeComment("This is exported by a tool, it isn't recommended to edit this directly");
		s.writeComment("modconv doesn't support exporting / importing the triangles to generate this, yet");
		s.beginObject("collision_triangles");
		SerializableCollTriInfo triWrapper;
		triWrapper.data = triangles;
		triWrapper.serialize(s);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		if (d.enterObject("collision_triangles")) {
			SerializableCollTriInfo triWrapper;
			triWrapper.deserialize(d);
			triangles = triWrapper.data;
			d.exitObject();
		}

		if (d.enterObject("collision_grid")) {
			SerializableCollGrid gridWrapper;
			gridWrapper.deserialize(d);
			grid = gridWrapper.data;
			d.exitObject();
		}
	}
};

// Convenience functions
inline bool saveCollisionToFile(const std::string& filename, const CollTriInfo& triangles, const CollGrid& grid)
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	JsonTextSerializer serializer(file);
	serializer.beginDocument();

	SerializableCollisionData collision;
	collision.triangles = triangles;
	collision.grid      = grid;
	collision.serialize(serializer);

	serializer.endDocument();
	return true;
}

inline bool loadCollisionFromFile(const std::string& filename, CollTriInfo& triangles, CollGrid& grid)
{
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	JsonTextDeserializer deserializer(file);
	if (!deserializer.parseDocument())
		return false;

	SerializableCollisionData collision;
	collision.deserialize(deserializer);

	triangles = collision.triangles;
	grid      = collision.grid;
	return true;
}

} // namespace collision

#endif // COLLISION_SERIALIZER_HPP