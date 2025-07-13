#include "joint.hpp"

void JointMatPoly::read(util::fstream_reader& reader)
{
	mMaterialIndex = reader.readU16();
	mMeshIndex     = reader.readU16();
}

void JointMatPoly::write(util::fstream_writer& writer) const
{
	writer.writeU16(mMaterialIndex);
	writer.writeU16(mMeshIndex);
}

void Joint::read(util::fstream_reader& reader)
{
	mParentIndex = reader.readU32();
	mIsVisible   = reader.readU32();
	mMinBounds.read(reader);
	mMaxBounds.read(reader);
	mVolumeRadius = reader.readF32();
	mScale.read(reader);
	mRotation.read(reader);
	mPosition.read(reader);
	mLinkedPolygons.resize(reader.readU32());
	for (JointMatPoly& poly : mLinkedPolygons) {
		poly.read(reader);
	}
}

void Joint::write(util::fstream_writer& writer)
{
	writer.writeU32(mParentIndex);
	writer.writeU32(mIsVisible);
	mMinBounds.write(writer);
	mMaxBounds.write(writer);
	writer.writeF32(mVolumeRadius);
	mScale.write(writer);
	mRotation.write(writer);
	mPosition.write(writer);
	writer.writeU32(static_cast<u32>(mLinkedPolygons.size()));
	for (JointMatPoly& poly : mLinkedPolygons) {
		poly.write(writer);
	}
}
