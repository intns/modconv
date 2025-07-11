#include <common/plane.hpp>

void Plane::read(util::fstream_reader& reader)
{
	mNormal.read(reader);
	mDistance = reader.readF32();
}

void Plane::write(util::fstream_writer& writer)
{
	mNormal.write(writer);
	writer.writeF32(mDistance);
}
