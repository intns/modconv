#include <common/vector3.hpp>

void Vector3f::read(util::fstream_reader& reader)
{
    x = reader.readF32();
    y = reader.readF32();
    z = reader.readF32();
}

void Vector3f::write(util::fstream_writer& writer)
{
    writer.writeF32(x);
    writer.writeF32(y);
    writer.writeF32(z);
}

void Vector3i::read(util::fstream_reader& reader)
{
    x = reader.readU32();
    y = reader.readU32();
    z = reader.readU32();
}

void Vector3i::write(util::fstream_writer& writer)
{
    writer.writeU32(x);
    writer.writeU32(y);
    writer.writeU32(z);
}