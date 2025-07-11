#include <common/nbt.hpp>

void NBT::read(util::fstream_reader& reader)
{
	mNormal.read(reader);
	mBinormal.read(reader);
	mTangent.read(reader);
}

void NBT::write(util::fstream_writer& writer)
{
	mNormal.write(writer);
	mBinormal.write(writer);
	mTangent.write(writer);
}