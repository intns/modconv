#ifndef JOINT_HPP
#define JOINT_HPP

#include "vector3.hpp"
#include "../types.hpp"
#include "../util/fstream_reader.hpp"
#include "../util/fstream_writer.hpp"

struct JointMatPoly {
	s16 mMaterialIndex = 0;
	s16 mMeshIndex     = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
};

struct Joint {
	s32 mParentIndex = 0;
	u32 mIsVisible   = 0;
	Vector3f mMinBounds;
	Vector3f mMaxBounds;
	f32 mVolumeRadius = 0;
	Vector3f mScale;
	Vector3f mRotation;
	Vector3f mPosition;
	std::vector<JointMatPoly> mLinkedPolygons;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
};

#endif