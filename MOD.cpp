#include "MOD.hpp"
#include <iostream>
#include <map>

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

void NBT::read(util::fstream_reader& reader)
{
    m_normals.read(reader);
    m_binormals.read(reader);
    m_tangent.read(reader);
}

void NBT::write(util::fstream_writer& writer)
{
    m_normals.write(writer);
    m_binormals.write(writer);
    m_tangent.write(writer);
}

void Color::read(util::fstream_reader& reader)
{
    r = reader.readU8();
    g = reader.readU8();
    b = reader.readU8();
    a = reader.readU8();
}

void Color::write(util::fstream_writer& writer)
{
    writer.writeU8(r);
    writer.writeU8(g);
    writer.writeU8(b);
    writer.writeU8(a);
}

void Texture::read(util::fstream_reader& reader)
{
    m_width   = reader.readU16();
    m_height  = reader.readU16();
    m_format  = reader.readU32();
    m_unknown = reader.readU32();
    for (u32 i = 0; i < 4; i++) {
        reader.readU32();
    }
    m_imageData.resize(reader.readU32());
    reader.read_buffer(reinterpret_cast<char*>(m_imageData.data()), m_imageData.size());
}

void Texture::write(util::fstream_writer& writer)
{
    writer.writeU16(m_width);
    writer.writeU16(m_height);
    writer.writeU32(m_format);
    writer.writeU32(m_unknown);
    for (u32 i = 0; i < 4; i++) {
        writer.writeU32(0);
    }
    writer.writeU32(m_imageData.size());
    writer.write(reinterpret_cast<char*>(m_imageData.data()), m_imageData.size());
}

void TextureAttributes::read(util::fstream_reader& reader)
{
    m_index = reader.readU16();
    reader.readU16();
    m_tilingMode = reader.readU16();
    m_unknown1   = reader.readU16();
    m_unknown2   = reader.readF32();
}

void TextureAttributes::write(util::fstream_writer& writer)
{
    writer.writeU16(m_index);
    writer.writeU16(0);
    writer.writeU16(m_tilingMode);
    writer.writeU16(m_unknown1);
    writer.writeF32(m_unknown2);
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
        u32 opcode              = reader.readU32();
        u32 length              = reader.readU32();

        if (position & 0x1F) {
            std::cout << "Error in chunk " << opcode
                      << ", chunk start isn't aligned to 0x20, this means an improper read occured." << std::endl;
            return;
        }

        std::cout << "Got chunk 0x" << std::hex << opcode << std::dec;
        const auto ocString = getChunkName(opcode);
        std::cout << ", " << (ocString.has_value() ? ocString.value() : "Unknown chunk") << std::endl;

        switch (opcode) {
        case 0:
            align(reader, 0x20);
            m_header.m_dateTime.m_year  = reader.readU16();
            m_header.m_dateTime.m_month = reader.readU8();
            m_header.m_dateTime.m_day   = reader.readU8();
            m_header.m_flags            = reader.readU32();
            align(reader, 0x20);

            std::cout << "MOD File Creation date (YYYY/MM/DD): " << (u32)m_header.m_dateTime.m_year << "/"
                      << (u32)m_header.m_dateTime.m_month << "/" << (u32)m_header.m_dateTime.m_day << '\n'
                      << std::endl;
            break;
        case 0x10:
            m_vertices.resize(reader.readU32());

            align(reader, 0x20);
            for (Vector3f& vertex : m_vertices) {
                vertex.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vertices.size() << " vertices found\n" << std::endl;
            break;
        case 0x11:
            m_vnormals.resize(reader.readU32());

            align(reader, 0x20);
            for (Vector3f& vertex : m_vnormals) {
                vertex.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vnormals.size() << " vertex normals found\n" << std::endl;
            break;
        case 0x12:
            m_vertexnbt.resize(reader.readU32());

            align(reader, 0x20);
            for (NBT& nbt : m_vertexnbt) {
                nbt.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vertexnbt.size() << " vertex NBTs found\n" << std::endl;
            break;
        case 0x13:
            m_vcolors.resize(reader.readU32());

            align(reader, 0x20);
            for (Color& color : m_vcolors) {
                color.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_vertexnbt.size() << " vertex NBTs found\n" << std::endl;
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

            std::cout << "Texture Coordinate " << texcoordNum << " has " << m_texcoords[texcoordNum].size()
                      << " coordinates\n"
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

            std::cout << m_textures.size() << " texture(s) found\n" << std::endl;
            break;
        case 0x22:
            m_texattrs.resize(reader.readU32());

            align(reader, 0x20);
            for (TextureAttributes& attrs : m_texattrs) {
                attrs.read(reader);
            }
            align(reader, 0x20);

            std::cout << m_texattrs.size() << " texture attribute(s) found\n" << std::endl;
            break;
        case 0x30:
            m_materials.m_materials.resize(reader.readU32());
            m_materials.m_texEnvironments.resize(reader.readU32());

            align(reader, 0x20);
            if (m_materials.m_texEnvironments.size()) {
                for (mat::TEVInfo& info : m_materials.m_texEnvironments) {
                    info.read(reader);
                }
            }

            if (m_materials.m_materials.size()) {
                for (mat::Material& mat : m_materials.m_materials) {
                    mat.read(reader);
                }
            }
            align(reader, 0x20);
            break;
        case 0xFFFF:
            stopRead = true;
            break;
        default:
            reader.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(length),
                         std::ios_base::cur);
            break;
        }
    }
}

static const u32 startChunk(util::fstream_writer& writer, u32 chunk)
{
    writer.writeU32(chunk);
    const u32 position = static_cast<u32>(writer.tellp());
    writer.writeU32(0);
    return position;
}

static void finishChunk(util::fstream_writer& writer, u32 chunkStart)
{
    writer.align(0x20);
    const u32 position = static_cast<u32>(writer.tellp());
    writer.seekp(chunkStart, std::ios_base::beg);
    writer.writeU32(position - chunkStart - 4);
    writer.seekp(position, std::ios_base::beg);
}

static inline void writeGenericChunk(util::fstream_writer& writer, auto& vector, u32 chunkIdentifier)
{
    std::optional<std::string_view> chunkName = MOD::getChunkName(chunkIdentifier);
    if (chunkName.has_value()) {
        std::cout << std::hex;
        std::cout << "Writing 0x" << chunkIdentifier << ", " << chunkName.value() << std::endl;
        std::cout << std::dec;
    }

    u32 subchunkPos = startChunk(writer, chunkIdentifier);
    writer.writeU32(vector.size());

    writer.align(0x20);
    for (auto& contents : vector) {
        contents.write(writer);
    }
    finishChunk(writer, subchunkPos);
}

// NOTE: the control flow and layout of this function is a replica of a
// decompiled version of the DMD->MOD process, found in plugTexConv
void MOD::write(util::fstream_writer& writer)
{
    // Write header
    u32 headerPos = startChunk(writer, 0);
    writer.align(0x20);
    writer.writeU16(m_header.m_dateTime.m_year);
    writer.writeU8(m_header.m_dateTime.m_month);
    writer.writeU8(m_header.m_dateTime.m_day);
    writer.writeU32(m_header.m_flags);
    finishChunk(writer, headerPos);

    if (m_vertices.size()) {
        writeGenericChunk(writer, m_vertices, 0x10);
    }

    if (m_vcolors.size()) {
        writeGenericChunk(writer, m_vcolors, 0x13);
    }

    if (m_vnormals.size()) {
        writeGenericChunk(writer, m_vnormals, 0x11);
    }

    if (m_header.m_flags & static_cast<u32>(MODFlags::UseNBT) && m_vertexnbt.size()) {
        writeGenericChunk(writer, m_vnormals, 0x12);
    }

    for (std::size_t i = 0; i < m_texcoords.size(); i++) {
        if (m_texcoords[i].size()) {
            writeGenericChunk(writer, m_texcoords[i], i + 0x18);
        }
    }

    if (m_textures.size()) {
        writeGenericChunk(writer, m_textures, 0x20);
    }

    if (m_texattrs.size()) {
        writeGenericChunk(writer, m_texattrs, 0x22);
    }

    // TODO: implement rest

    // Finalise writing with 0xFFFF chunk and append any INI file
    finishChunk(writer, startChunk(writer, 0xFFFF));
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

// clang-format off
const std::map<u32, std::string_view> gChunkNames = 
{
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
    { 0x22, "Texture Attributes" },
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
// clang-format on

const std::optional<std::string_view> MOD::getChunkName(u32 opcode)
{
    if (gChunkNames.contains(opcode)) {
        return gChunkNames.at(opcode);
    }

    return std::nullopt;
}

namespace mat {
void KeyInfoU8::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readU8();
    reader.readU8();
    reader.readU16();

    m_unknown2 = reader.readF32();
    m_unknown3 = reader.readF32();
}

void KeyInfoU8::write(util::fstream_writer& writer)
{
    writer.writeU8(m_unknown1);
    writer.writeU8(0);
    writer.writeU16(0);

    writer.writeF32(m_unknown2);
    writer.writeF32(m_unknown3);
}

void KeyInfoF32::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readF32();
    m_unknown2 = reader.readF32();
    m_unknown3 = reader.readF32();
}

void KeyInfoF32::write(util::fstream_writer& writer)
{
    writer.writeF32(m_unknown1);
    writer.writeF32(m_unknown2);
    writer.writeF32(m_unknown3);
}

inline void PCI_Unk1::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);
    m_unknown3.read(reader);
    m_unknown4.read(reader);
}

inline void PCI_Unk1::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);
    m_unknown3.write(writer);
    m_unknown4.write(writer);
}

inline void PCI_Unk2::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);
}

inline void PCI_Unk2::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);
}

void PolygonColourInfo::read(util::fstream_reader& reader)
{
    m_unknown1.read(reader);
    m_unknown2 = reader.readS32();
    m_unknown3 = reader.readF32();

    m_unknown4.resize(reader.readU32());
    for (mat::PCI_Unk1& unk : m_unknown4) {
        unk.read(reader);
    }

    m_unknown5.resize(reader.readU32());
    for (mat::PCI_Unk2& unk : m_unknown5) {
        unk.read(reader);
    }
}

void PolygonColourInfo::write(util::fstream_writer& writer)
{
    m_unknown1.write(writer);
    writer.writeS32(m_unknown2);
    writer.writeF32(m_unknown3);

    writer.writeU32(m_unknown4.size());
    for (mat::PCI_Unk1& unk : m_unknown4) {
        unk.write(writer);
    }

    writer.writeU32(m_unknown5.size());
    for (mat::PCI_Unk2& unk : m_unknown5) {
        unk.write(writer);
    }
}

void LightingInfo::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2 = reader.readF32();
}

void LightingInfo::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    writer.writeF32(m_unknown2);
}

void PeInfo::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2 = reader.readS32();
    m_unknown3 = reader.readS32();
    m_unknown4 = reader.readS32();
}

void PeInfo::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    writer.writeS32(m_unknown2);
    writer.writeS32(m_unknown3);
    writer.writeS32(m_unknown4);
}

void TexGenData::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readU8();
    m_unknown2 = reader.readU8();
    m_unknown3 = reader.readU8();
    m_unknown4 = reader.readU8();
}

void TexGenData::write(util::fstream_writer& writer)
{
    writer.writeU8(m_unknown1);
    writer.writeU8(m_unknown2);
    writer.writeU8(m_unknown3);
    writer.writeU8(m_unknown4);
}

void TXD_Unk1::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);
    m_unknown3.read(reader);
    m_unknown4.read(reader);
}

void TXD_Unk1::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);
    m_unknown3.write(writer);
    m_unknown4.write(writer);
}

void TextureData::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2 = reader.readS16();
    m_unknown3 = reader.readS16();

    m_unknown4 = reader.readU8();
    m_unknown5 = reader.readU8();
    m_unknown6 = reader.readU8();
    m_unknown7 = reader.readU8();

    m_unknown8 = reader.readS32();
    m_unknown9 = reader.readS32();

    m_unknown10 = reader.readF32();
    m_unknown11 = reader.readF32();
    m_unknown12 = reader.readF32();
    m_unknown13 = reader.readF32();
    m_unknown14 = reader.readF32();
    m_unknown15 = reader.readF32();
    m_unknown16 = reader.readF32();
    m_unknown17 = reader.readF32();
    m_unknown18 = reader.readF32();

    m_unknown19.resize(reader.readU32());
    for (mat::TXD_Unk1& unk : m_unknown19) {
        unk.read(reader);
    }

    m_unknown20.resize(reader.readU32());
    for (mat::TXD_Unk1& unk : m_unknown20) {
        unk.read(reader);
    }

    m_unknown21.resize(reader.readU32());
    for (mat::TXD_Unk1& unk : m_unknown21) {
        unk.read(reader);
    }
}

void TextureData::write(util::fstream_writer& reader) { }

void TextureInfo::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);

    m_unknown3.resize(reader.readU32());
    for (mat::TexGenData& genData : m_unknown3) {
        genData.read(reader);
    }

    m_unknown4.resize(reader.readU32());
    for (mat::TextureData& texData : m_unknown4) {
        texData.read(reader);
    }
}

// TODO
void TextureInfo::write(util::fstream_writer& writer) { }

void Material::read(util::fstream_reader& reader)
{
    m_flags  = reader.readU32();
    m_texIdx = reader.readU32();
    m_color.read(reader);

    if (m_flags & static_cast<u32>(mat::MaterialFlags::UsePVW)) {
        m_unknown1 = reader.readU32();
        m_colourInfo.read(reader);
        m_lightingInfo.read(reader);
        m_peInfo.read(reader);
        m_texInfo.read(reader);
    }
}

// TODO
void Material::write(util::fstream_writer& writer) { }

void TEVInfo::read(util::fstream_reader& reader)
{
    // TODO: PVWTevColReg
    // TODO: PVWTevStage
}

// TODO
void TEVInfo::write(util::fstream_writer& writer) { }

} // namespace mat