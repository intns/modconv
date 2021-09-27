#include <common/colour.hpp>

void ColourU8::read(util::fstream_reader& reader)
{
    r = reader.readU8();
    g = reader.readU8();
    b = reader.readU8();
    a = reader.readU8();
}

void ColourU8::write(util::fstream_writer& writer)
{
    writer.writeU8(r);
    writer.writeU8(g);
    writer.writeU8(b);
    writer.writeU8(a);
}

std::ostream& operator<<(std::ostream& os, ColourU8 const& c)
{
    os << static_cast<u32>(c.r) << " " << static_cast<u32>(c.g) << " "
       << static_cast<u32>(c.b) << " " << static_cast<u32>(c.a);
    return os;
}

void ColourU16::read(util::fstream_reader& reader)
{
    r = reader.readU16();
    g = reader.readU16();
    b = reader.readU16();
    a = reader.readU16();
}

void ColourU16::write(util::fstream_writer& writer)
{
    writer.writeU16(r);
    writer.writeU16(g);
    writer.writeU16(b);
    writer.writeU16(a);
}

std::ostream& operator<<(std::ostream& os, ColourU16 const& c)
{
    os << static_cast<u32>(c.r) << " " << static_cast<u32>(c.g) << " "
       << static_cast<u32>(c.b) << " " << static_cast<u32>(c.a);
    return os;
}