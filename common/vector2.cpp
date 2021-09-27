#include <common/vector2.hpp>

void Vector2f::read(util::fstream_reader& reader)
{
    x = reader.readF32();
    y = reader.readF32();
}

void Vector2f::write(util::fstream_writer& writer)
{
    writer.writeF32(x);
    writer.writeF32(y);
}

void Vector2i::read(util::fstream_reader& reader)
{
    x = reader.readU32();
    y = reader.readU32();
}

void Vector2i::write(util::fstream_writer& writer)
{
    writer.writeU32(x);
    writer.writeU32(y);
}