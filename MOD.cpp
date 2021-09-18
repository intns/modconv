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

void Color::read(util::fstream_reader& reader)
{
    r = reader.readU8();
    g = reader.readU8();
    b = reader.readU8();
    a = reader.readU8();
}

void Texture::read(util::fstream_reader& reader)
{
    m_width = reader.readU16();
    m_height = reader.readU16();
    m_format = reader.readU32();
    m_unknown = reader.readU32();
    for (u32 i = 0; i < 4; i++) {
        reader.readU32();
    }
    m_imageData.resize(reader.readU32());
    reader.read_buffer(reinterpret_cast<char*>(m_imageData.data()), m_imageData.size());
}

void MOD::align(util::fstream_reader& reader, u32 amt)
{
    u32 offs = amt - (reader.tellg() % amt);
    if (offs != 0x20) {
        reader.seekg(offs, std::ios_base::cur);
    }
}

void MOD::read(util::fstream_reader& reader)
{
    bool stopRead = false;
    while (!stopRead) {
        std::streampos position = reader.tellg();
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
        case 0x13:
            m_vcolors.resize(reader.readU32());

            align(reader, 0x20);
            for (Color& color : m_vcolors) {
                color.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vertexnbt.size() << " vertex NBTs found\n"
                      << std::endl;
            break;
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F: {

            const u32 texcoordNum = opcode - 0x18;
            m_texcoords[texcoordNum].resize(reader.readU32());

            align(reader, 0x20);
            for (Vector2f& coords : m_texcoords[texcoordNum]) {
                coords.read(reader);
            }
            align(reader, 0x20);

            std::cout << "Texture Coordinate " << texcoordNum
                      << " has " << m_texcoords[texcoordNum].size() << " coordinates\n"
                      << std::endl;
            break;
        }
        case 0x20:
            m_textures.resize(reader.readU32());

            align(reader, 0x20);
            for (Texture& texture : m_textures) {
                texture.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_textures.size() << " texture(s) found\n"
                      << std::endl;
            break;
        case 0xFFFF:
            stopRead = true;
            break;
        default:
            reader.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(length), std::ios_base::cur);
            break;
        }
    }
}

void MOD::reset()
{
    m_vertices.clear();
    m_vnormals.clear();
    m_vertexnbt.clear();
    m_vcolors.clear();
    for (u32 i = 0; i < 8; i++) {
        m_texcoords[i].clear();
    }
    m_textures.clear();
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