#include "MOD.hpp"
#include <iostream>
#include <map>

void Vector2i::read(util::fstream_reader& reader)
{
    x = reader.readU32();
    y = reader.readU32();
}

void Vector2f::read(util::fstream_reader& reader)
{
    x = reader.readF32();
    y = reader.readF32();
}

void Vector3i::read(util::fstream_reader& reader)
{
    x = reader.readU32();
    y = reader.readU32();
    z = reader.readU32();
}

void Vector3f::read(util::fstream_reader& reader)
{
    x = reader.readF32();
    y = reader.readF32();
    z = reader.readF32();
}

void NBT::read(util::fstream_reader& reader)
{
    m_normals.read(reader);
    m_binormals.read(reader);
    m_tangent.read(reader);
}

void MOD::align(util::fstream_reader& reader, u32 amt)
{
    u32 offs = amt - (reader.m_filestream.tellg() % amt);
    if (offs != 0x20) {
        reader.m_filestream.seekg(offs, std::ios_base::cur);
    }
}

void MOD::read(util::fstream_reader& reader)
{
    bool stopRead = false;
    while (!stopRead) {
        std::streampos position = reader.m_filestream.tellg();
        u32 opcode = reader.readU32();
        u32 length = reader.readU32();

        if (position & 0x1F) {
            std::cout << "Error in chunk " << opcode << ", chunk start isn't aligned to 0x20, this means an improper read occured." << std::endl;
            return;
        }

        std::cout << "Got opcode " << std::hex << opcode << std::dec;
        const auto ocString = getChunkName(opcode);
        std::cout << ", " << (ocString.has_value() ? ocString.value() : "Unknown chunk") << std::endl;

        switch (opcode) {
        case 0:
            align(reader, 0x20);
            m_header.m_dateTime.m_year = reader.readU16();
            m_header.m_dateTime.m_month = reader.readU8();
            m_header.m_dateTime.m_day = reader.readU8();
            align(reader, 0x20);

            std::cout << "MOD File Creation date (YYYY/MM/DD): " << (u32)m_header.m_dateTime.m_year
                      << "/" << (u32)m_header.m_dateTime.m_month
                      << "/" << (u32)m_header.m_dateTime.m_day << '\n'
                      << std::endl;
            break;
        case 0x10:
            m_vertices.resize(reader.readU32());

            align(reader, 0x20);
            for (Vector3f& vertex : m_vertices) {
                vertex.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vertices.size() << " vertices found\n"
                      << std::endl;
            break;
        case 0x11:
            m_vnormals.resize(reader.readU32());

            align(reader, 0x20);
            for (Vector3f& vertex : m_vnormals) {
                vertex.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vnormals.size() << " vertex normals found\n"
                      << std::endl;
            break;
        case 0x12:
            m_vertexnbt.resize(reader.readU32());

            align(reader, 0x20);
            for (NBT& nbt : m_vertexnbt) {
                nbt.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vertexnbt.size() << " vertex NBTs found\n"
                      << std::endl;
            break;
        case 0xFFFF:
            stopRead = true;
            break;
        default:
            reader.m_filestream.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(length), std::ios_base::cur);
            break;
        }
    }
}

const std::map<u32, std::string_view> gChunkNames = {
    { 0x00, "Header" },
    { 0x10, "Vertices" },
    { 0x11, "Vertex Normals" },
    { 0x12, "Vertex Normal/Binormal/Tangent Descriptors" },
    { 0x13, "Vertex Colours" },

    { 0x18, "Texture Coordinate 0" },
    { 0x19, "Texture Coordinate 1" },
    { 0x1A, "Texture Coordinate 2" },
    { 0x1B, "Texture Coordinate 3" },
    { 0x1C, "Texture Coordinate 4" },
    { 0x1D, "Texture Coordinate 5" },
    { 0x1E, "Texture Coordinate 6" },
    { 0x1F, "Texture Coordinate 7" },

    { 0x20, "Textures" },
    { 0x22, "Texture Mipmaps" },
    { 0x30, "Materials" },

    { 0x40, "Vertex Matrix" },
    { 0x41, "Matrix Envelope" },

    { 0x50, "Mesh" },
    { 0x60, "Joints" },
    { 0x61, "Joint Names" },

    { 0x100, "Collision Prism" },
    { 0x110, "Collision Grid" },
    { 0xFFFF, "End Of File" }
};

const std::optional<std::string_view> MOD::getChunkName(u32 opcode)
{
    if (gChunkNames.contains(opcode)) {
        return gChunkNames.at(opcode);
    }

    return std::nullopt;
}
