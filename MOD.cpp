#include "MOD.hpp"
#include <iostream>
#include <map>

namespace {
static inline void align(util::fstream_reader& reader, u32 amt)
{
    u32 offs = amt - (reader.tellg() % amt);
    if (offs != 0x20) {
        reader.seekg(offs, std::ios_base::cur);
    }
}
} // namespace

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

void Colour::read(util::fstream_reader& reader)
{
    r = reader.readU8();
    g = reader.readU8();
    b = reader.readU8();
    a = reader.readU8();
}

void Colour::write(util::fstream_writer& writer)
{
    writer.writeU8(r);
    writer.writeU8(g);
    writer.writeU8(b);
    writer.writeU8(a);
}

void ShortColour::read(util::fstream_reader& reader)
{
    r = reader.readU16();
    g = reader.readU16();
    b = reader.readU16();
    a = reader.readU16();
}

void ShortColour::write(util::fstream_writer& writer)
{
    writer.writeU16(r);
    writer.writeU16(g);
    writer.writeU16(b);
    writer.writeU16(a);
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
    writer.writeU8(0);
    writer.writeU8(0);

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

void KeyInfoS10::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS16();
    reader.readS16();
    m_unknown2 = reader.readF32();
    m_unknown3 = reader.readF32();
}

void KeyInfoS10::write(util::fstream_writer& writer)
{
    writer.writeS16(m_unknown1);
    writer.writeS16(0);
    writer.writeF32(m_unknown2);
    writer.writeF32(m_unknown3);
}

void PCI_Unk1::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);
    m_unknown3.read(reader);
    m_unknown4.read(reader);
}

void PCI_Unk1::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);
    m_unknown3.write(writer);
    m_unknown4.write(writer);
}

void PCI_Unk2::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);
}

void PCI_Unk2::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);
}

void PolygonColourInfo::read(util::fstream_reader& reader)
{
    m_diffuseColour.read(reader);
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
    m_diffuseColour.write(writer);
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
    m_unknown1 = reader.readU32();
    m_unknown2 = reader.readF32();
}

void LightingInfo::write(util::fstream_writer& writer)
{
    writer.writeU32(m_unknown1);
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

    m_unknown8 = reader.readU32();
    m_unknown9 = reader.readS32();

    m_unknown10 = reader.readF32();
    m_unknown11 = reader.readF32();
    m_unknown12 = reader.readF32();
    m_unknown13 = reader.readF32();
    m_unknown14 = reader.readF32();
    m_unknown15 = reader.readF32();
    m_unknown16 = reader.readF32();
    m_unknown17 = reader.readF32();

    m_unknown18.resize(reader.readU32());
    for (mat::TXD_Unk1& unk : m_unknown18) {
        unk.read(reader);
    }

    m_unknown19.resize(reader.readU32());
    for (mat::TXD_Unk1& unk : m_unknown19) {
        unk.read(reader);
    }

    m_unknown20.resize(reader.readU32());
    for (mat::TXD_Unk1& unk : m_unknown20) {
        unk.read(reader);
    }
}

void TextureData::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);

    writer.writeS16(m_unknown2);
    writer.writeS16(m_unknown3);

    writer.writeU8(m_unknown4);
    writer.writeU8(m_unknown5);
    writer.writeU8(m_unknown6);
    writer.writeU8(m_unknown7);

    writer.writeU32(m_unknown8);
    writer.writeS32(m_unknown9);

    writer.writeF32(m_unknown10);
    writer.writeF32(m_unknown11);
    writer.writeF32(m_unknown12);
    writer.writeF32(m_unknown13);
    writer.writeF32(m_unknown14);
    writer.writeF32(m_unknown15);
    writer.writeF32(m_unknown16);
    writer.writeF32(m_unknown17);

    writer.writeU32(m_unknown18.size());
    for (mat::TXD_Unk1& unk : m_unknown18) {
        unk.write(writer);
    }

    writer.writeU32(m_unknown19.size());
    for (mat::TXD_Unk1& unk : m_unknown19) {
        unk.write(writer);
    }

    writer.writeU32(m_unknown20.size());
    for (mat::TXD_Unk1& unk : m_unknown20) {
        unk.write(writer);
    }
}

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

void TextureInfo::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);

    writer.writeU32(m_unknown3.size());
    for (mat::TexGenData& genData : m_unknown3) {
        genData.write(writer);
    }

    writer.writeU32(m_unknown4.size());
    for (mat::TextureData& texData : m_unknown4) {
        texData.write(writer);
    }
}

void Material::read(util::fstream_reader& reader)
{
    m_flags    = reader.readU32();
    m_unknown1 = reader.readU32();
    m_colour.read(reader);

    if (m_flags & static_cast<u32>(MaterialFlags::UsePVW)) {
        m_unknown2 = reader.readU32();
        m_colourInfo.read(reader);
        m_lightingInfo.read(reader);
        m_peInfo.read(reader);
        m_texInfo.read(reader);
    }
}

void Material::write(util::fstream_writer& writer)
{
    writer.writeU32(m_flags);
    writer.writeU32(m_unknown1);
    m_colour.write(writer);

    if (m_flags & static_cast<u32>(MaterialFlags::UsePVW)) {
        writer.writeS32(m_unknown2);
        m_colourInfo.write(writer);
        m_lightingInfo.write(writer);
        m_peInfo.write(writer);
        m_texInfo.write(writer);
    }
}

void TCR_Unk1::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);
    m_unknown3.read(reader);
    m_unknown4.read(reader);
}

void TCR_Unk1::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);
    m_unknown3.write(writer);
    m_unknown4.write(writer);
}

void TCR_Unk2::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readS32();
    m_unknown2.read(reader);
}

void TCR_Unk2::write(util::fstream_writer& writer)
{
    writer.writeS32(m_unknown1);
    m_unknown2.write(writer);
}

void TEVColReg::read(util::fstream_reader& reader)
{
    m_unknown1.read(reader);
    m_unknown2 = reader.readS32();
    m_unknown3 = reader.readF32();

    m_unknown4.resize(reader.readU32());
    for (mat::TCR_Unk1& unk : m_unknown4) {
        unk.read(reader);
    }

    m_unknown5.resize(reader.readU32());
    for (mat::TCR_Unk2& unk : m_unknown5) {
        unk.read(reader);
    }
}

void TEVColReg::write(util::fstream_writer& writer)
{
    m_unknown1.write(writer);
    writer.writeS32(m_unknown2);
    writer.writeF32(m_unknown3);

    writer.writeU32(m_unknown4.size());
    for (mat::TCR_Unk1& unk : m_unknown4) {
        unk.write(writer);
    }

    writer.writeU32(m_unknown5.size());
    for (mat::TCR_Unk2& unk : m_unknown5) {
        unk.write(writer);
    }
}

void PVWCombiner::read(util::fstream_reader& reader)
{
    m_unknown1  = reader.readU8();
    m_unknown2  = reader.readU8();
    m_unknown3  = reader.readU8();
    m_unknown4  = reader.readU8();
    m_unknown5  = reader.readU8();
    m_unknown6  = reader.readU8();
    m_unknown7  = reader.readU8();
    m_unknown8  = reader.readU8();
    m_unknown9  = reader.readU8();
    m_unknown10 = reader.readU8();
    m_unknown11 = reader.readU8();
    m_unknown12 = reader.readU8();
}

void PVWCombiner::write(util::fstream_writer& writer)
{
    writer.writeU8(m_unknown1);
    writer.writeU8(m_unknown2);
    writer.writeU8(m_unknown3);
    writer.writeU8(m_unknown4);
    writer.writeU8(m_unknown5);
    writer.writeU8(m_unknown6);
    writer.writeU8(m_unknown7);
    writer.writeU8(m_unknown8);
    writer.writeU8(m_unknown9);
    writer.writeU8(m_unknown10);
    writer.writeU8(m_unknown11);
    writer.writeU8(m_unknown12);
}

void TEVStage::read(util::fstream_reader& reader)
{
    m_unknown1 = reader.readU8();
    m_unknown2 = reader.readU8();
    m_unknown3 = reader.readU8();
    m_unknown4 = reader.readU8();
    m_unknown5 = reader.readU8();
    m_unknown6 = reader.readU8();
    reader.readU16();
    m_unknown7.read(reader);
    m_unknown8.read(reader);
}

void TEVStage::write(util::fstream_writer& writer)
{
    writer.writeU8(m_unknown1);
    writer.writeU8(m_unknown2);
    writer.writeU8(m_unknown3);
    writer.writeU8(m_unknown4);
    writer.writeU8(m_unknown5);
    writer.writeU8(m_unknown6);
    writer.writeU16(0);
    m_unknown7.write(writer);
    m_unknown8.write(writer);
}

void TEVInfo::read(util::fstream_reader& reader)
{
    m_unknown1.read(reader);
    m_unknown2.read(reader);
    m_unknown3.read(reader);

    m_unknown4.read(reader);
    m_unknown5.read(reader);
    m_unknown6.read(reader);
    m_unknown7.read(reader);

    m_unknown8.resize(reader.readU32());
    for (mat::TEVStage& stage : m_unknown8) {
        stage.read(reader);
    }
}

void TEVInfo::write(util::fstream_writer& writer)
{
    m_unknown1.write(writer);
    m_unknown2.write(writer);
    m_unknown3.write(writer);

    m_unknown4.write(writer);
    m_unknown5.write(writer);
    m_unknown6.write(writer);
    m_unknown7.write(writer);

    writer.writeU32(m_unknown8.size());
    for (mat::TEVStage& stage : m_unknown8) {
        stage.write(writer);
    }
}

} // namespace mat

void VtxMatrix::read(util::fstream_reader& reader) { m_index = reader.readU16(); }
void VtxMatrix::write(util::fstream_writer& writer) { writer.writeU16(m_index); }

void Envelope::read(util::fstream_reader& reader)
{
    m_indices.resize(reader.readU16());
    m_weights.reserve(m_indices.size());

    for (std::size_t i = 0; i < m_indices.size(); i++) {
        m_indices[i] = reader.readU16();
        m_weights[i] = reader.readF32();
    }
}

void Envelope::write(util::fstream_writer& writer)
{
    writer.writeU16(static_cast<u16>(m_indices.size()));

    for (std::size_t i = 0; i < m_indices.size(); i++) {
        writer.writeU16(m_indices[i]);
        writer.writeF32(m_weights[i]);
    }
}

void DisplayList::read(util::fstream_reader& reader)
{
    m_flags.intView = reader.readU32();
    m_unknown1      = reader.readU32();
    m_dlistData.resize(reader.readU32());
    align(reader, 0x20);
    reader.read(reinterpret_cast<char*>(m_dlistData.data()), m_dlistData.size());
}

void DisplayList::write(util::fstream_writer& writer)
{
    writer.writeU32(m_flags.intView);
    writer.writeU32(m_unknown1);
    writer.writeU32(m_dlistData.size());
    writer.align(0x20);
    writer.write(reinterpret_cast<char*>(m_dlistData.data()), m_dlistData.size());
}

void MeshPacket::read(util::fstream_reader& reader)
{
    m_indices.resize(reader.readU32());
    for (u16& index : m_indices) {
        index = reader.readU16();
    }

    m_displaylists.resize(reader.readU32());
    for (DisplayList& dlist : m_displaylists) {
        dlist.read(reader);
    }
}

void MeshPacket::write(util::fstream_writer& writer)
{
    writer.writeU32(m_indices.size());
    for (u16& index : m_indices) {
        writer.writeU16(index);
    }

    writer.writeU32(m_displaylists.size());
    for (DisplayList& dlist : m_displaylists) {
        dlist.write(writer);
    }
}

void Mesh::read(util::fstream_reader& reader)
{
    m_boneIndex     = reader.readU32();
    m_vtxDescriptor = reader.readU32();
    m_packets.resize(reader.readU32());
    for (MeshPacket& packet : m_packets) {
        packet.read(reader);
    }
}

void Mesh::write(util::fstream_writer& writer)
{
    writer.writeU32(m_boneIndex);
    writer.writeU32(m_vtxDescriptor);
    writer.writeU32(m_packets.size());
    for (MeshPacket& packet : m_packets) {
        packet.write(writer);
    }
}

void JointMatPoly::read(util::fstream_reader& reader)
{
    m_matIdx  = reader.readU16();
    m_meshIdx = reader.readU16();
}

void JointMatPoly::write(util::fstream_writer& writer)
{
    writer.writeU16(m_matIdx);
    writer.writeU16(m_meshIdx);
}

void Joint::read(util::fstream_reader& reader)
{
    m_parentIdx = reader.readU32();
    m_flags     = reader.readU32();
    m_boundsMax.read(reader);
    m_boundsMin.read(reader);
    m_volumeRadius = reader.readF32();
    m_scale.read(reader);
    m_rotation.read(reader);
    m_position.read(reader);
    m_matpolys.resize(reader.readU32());
    for (JointMatPoly& poly : m_matpolys) {
        poly.read(reader);
    }
}

void Joint::write(util::fstream_writer& writer)
{
    writer.writeU32(m_parentIdx);
    writer.writeU32(m_flags);
    m_boundsMax.write(writer);
    m_boundsMin.write(writer);
    writer.writeF32(m_volumeRadius);
    m_scale.write(writer);
    m_rotation.write(writer);
    m_position.write(writer);
    writer.writeU32(m_matpolys.size());
    for (JointMatPoly& poly : m_matpolys) {
        poly.write(writer);
    }
}

void Plane::read(util::fstream_reader& reader)
{
    m_position.read(reader);
    m_diameter = reader.readF32();
}

void Plane::write(util::fstream_writer& writer)
{
    m_position.write(writer);
    writer.writeF32(m_diameter);
}

void BaseRoomInfo::read(util::fstream_reader& reader) { m_unknown1 = reader.readU32(); }
void BaseRoomInfo::write(util::fstream_writer& writer) { writer.writeU32(m_unknown1); }

void BaseCollTriInfo::read(util::fstream_reader& reader)
{
    m_mapCode = reader.readU32();
    m_indice.read(reader);

    m_unknown2 = reader.readU16();
    m_unknown3 = reader.readU16();
    m_unknown4 = reader.readU16();
    m_unknown5 = reader.readU16();

    m_plane.read(reader);
}

void BaseCollTriInfo::write(util::fstream_writer& writer)
{
    writer.writeU32(m_mapCode);
    m_indice.write(writer);

    writer.writeU16(m_unknown2);
    writer.writeU16(m_unknown3);
    writer.writeU16(m_unknown4);
    writer.writeU16(m_unknown5);

    m_plane.write(writer);
}

void CollGroup::read(util::fstream_reader& reader)
{
    m_unknown1.resize(reader.readU16());

    m_unknown2.resize(reader.readU16());
    for (u32& i : m_unknown2) {
        i = reader.readU32();
    }

    for (u8& i : m_unknown1) {
        i = reader.readU8();
    }
}

void CollGroup::write(util::fstream_writer& writer)
{
    writer.writeU16(m_unknown1.size());
    writer.writeU16(m_unknown2.size());
    for (u32& i : m_unknown2) {
        writer.writeU32(i);
    }

    for (u8& i : m_unknown1) {
        writer.writeU8(i);
    }
}

static inline const u32 startChunk(util::fstream_writer& writer, u32 chunk)
{
    writer.writeU32(chunk);
    const u32 position = static_cast<u32>(writer.tellp());
    writer.writeU32(0);
    return position;
}

static inline void finishChunk(util::fstream_writer& writer, u32 chunkStart)
{
    writer.align(0x20);
    const u32 position = static_cast<u32>(writer.tellp());
    writer.seekp(chunkStart, std::ios_base::beg);
    writer.writeU32(position - chunkStart - 4);
    writer.seekp(position, std::ios_base::beg);
}

static inline void writeGenericChunk(util::fstream_writer& writer, auto& vector, u32 chunkIdentifier)
{
    std::cout << "Writing 0x" << std::hex << chunkIdentifier << std::dec << ", "
              << MOD::getChunkName(chunkIdentifier).value() << std::endl;

    u32 subchunkPos = startChunk(writer, chunkIdentifier);
    writer.writeU32(vector.size());

    writer.align(0x20);
    for (auto& contents : vector) {
        contents.write(writer);
    }
    finishChunk(writer, subchunkPos);
}

static inline void readGenericChunk(util::fstream_reader& reader, auto& vector)
{
    vector.resize(reader.readU32());

    align(reader, 0x20);
    for (auto& elem : vector) {
        elem.read(reader);
    }
    align(reader, 0x20);
}

void MOD::read(util::fstream_reader& reader)
{
    bool stopRead = false;
    while (!stopRead) {
        std::streampos position = reader.tellg();
        u32 opcode              = reader.readU32();
        u32 length              = reader.readU32();

        if (position & 0x1F) {
            std::cout << "Error in chunk " << opcode << ", offset " << position
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

            std::cout << "File creation date (YYYY/MM/DD): " << (u32)m_header.m_dateTime.m_year << "/"
                      << (u32)m_header.m_dateTime.m_month << "/" << (u32)m_header.m_dateTime.m_day << '\n'
                      << std::endl;
            break;
        case 0x10:
            readGenericChunk(reader, m_vertices);
            std::cout << m_vertices.size() << " vertice(s) found\n" << std::endl;
            break;
        case 0x11:
            readGenericChunk(reader, m_vnormals);
            std::cout << m_vnormals.size() << " vertex normal(s) found\n" << std::endl;
            break;
        case 0x12:
            readGenericChunk(reader, m_vertexnbt);
            std::cout << m_vertexnbt.size() << " vertex NBT(s) found\n" << std::endl;
            break;
        case 0x13:
            readGenericChunk(reader, m_vcolours);
            std::cout << m_vertexnbt.size() << " vertex colour(s) found\n" << std::endl;
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
            readGenericChunk(reader, m_texcoords[texcoordNum]);
            std::cout << "Texture Coordinate " << texcoordNum << " has " << m_texcoords[texcoordNum].size()
                      << " coordinate(s)\n"
                      << std::endl;
            break;
        }
        case 0x20:
            readGenericChunk(reader, m_textures);
            std::cout << m_textures.size() << " texture(s) found\n" << std::endl;
            break;
        case 0x22:
            readGenericChunk(reader, m_texattrs);
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

            std::cout << m_materials.m_materials.size() << " material(s) found, "
                      << m_materials.m_texEnvironments.size() << " texture environment(s) found\n"
                      << std::endl;
            break;
        case 0x40:
            readGenericChunk(reader, m_vtxMatrix);
            std::cout << m_vtxMatrix.size() << " vertex matrice(s) found\n" << std::endl;
            break;
        case 0x41:
            readGenericChunk(reader, m_envelopes);
            std::cout << m_envelopes.size() << " envelope(s) found\n" << std::endl;
            break;
        case 0x50:
            readGenericChunk(reader, m_meshes);
            std::cout << m_meshes.size() << " mesh(es) found\n" << std::endl;
            break;
        case 0x60:
            readGenericChunk(reader, m_joints);
            std::cout << m_joints.size() << " joint(s) found\n" << std::endl;
            break;
        case 0x61:
            m_jointNames.resize(reader.readU32());
            align(reader, 0x20);
            for (std::string& str : m_jointNames) {
                str.resize(reader.readU32());
                for (u32 i = 0; i < str.size(); i++) {
                    str[i] = reader.readU8();
                }
            }
            align(reader, 0x20);
            std::cout << m_jointNames.size() << " joint name(s) found\n" << std::endl;
            break;
        case 0x100:
            m_colltris.m_collinfo.resize(reader.readU32());
            m_colltris.m_roominfo.resize(reader.readU32());

            align(reader, 0x20);
            for (BaseRoomInfo& info : m_colltris.m_roominfo) {
                info.read(reader);
            }
            align(reader, 0x20);

            for (BaseCollTriInfo& info : m_colltris.m_collinfo) {
                info.read(reader);
            }
            align(reader, 0x20);
            break;
        case 0x110:
            align(reader, 0x20);
            m_collgrid.m_boundsMin.read(reader);
            m_collgrid.m_boundsMax.read(reader);
            m_collgrid.m_unknown1 = reader.readF32();
            m_collgrid.m_gridX    = reader.readU32();
            m_collgrid.m_gridY    = reader.readU32();
            m_collgrid.m_groups.resize(reader.readU32());
            for (CollGroup& group : m_collgrid.m_groups) {
                group.read(reader);
            }

            for (u32 x = 0; x < m_collgrid.m_gridX; x++) {
                for (u32 y = 0; y < m_collgrid.m_gridY; y++) {
                    m_collgrid.m_unknown2.push_back(reader.readS32());
                }
            }
            align(reader, 0x20);
            break;
        case 0xFFFF:
            reader.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(length),
                         std::ios_base::cur);

            while (!reader.eof()) {
                m_eofBytes.push_back(reader.get());
                reader.peek();
            }

            stopRead = true;
            break;
        default:
            reader.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(length),
                         std::ios_base::cur);
            break;
        }
    }
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

    if (m_vcolours.size()) {
        writeGenericChunk(writer, m_vcolours, 0x13);
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

    if (m_materials.m_materials.size()) {
        std::cout << "Writing 0x30, " << MOD::getChunkName(0x30).value() << std::endl;

        const u32 start = startChunk(writer, 0x30);
        writer.writeU32(m_materials.m_materials.size());
        writer.writeU32(m_materials.m_texEnvironments.size());
        writer.align(0x20);

        for (mat::TEVInfo& tevInfo : m_materials.m_texEnvironments) {
            tevInfo.write(writer);
        }

        for (mat::Material& material : m_materials.m_materials) {
            material.write(writer);
        }
        finishChunk(writer, start);
    }

    if (m_envelopes.size()) {
        writeGenericChunk(writer, m_envelopes, 0x41);
    }

    if (m_vtxMatrix.size()) {
        writeGenericChunk(writer, m_vtxMatrix, 0x40);
    }

    if (m_meshes.size()) {
        writeGenericChunk(writer, m_meshes, 0x50);
    }

    if (m_joints.size()) {
        writeGenericChunk(writer, m_joints, 0x60);

        if (m_jointNames.size()) {
            const u32 start = startChunk(writer, 0x61);
            writer.writeU32(m_jointNames.size());
            writer.align(0x20);
            for (std::string& name : m_jointNames) {
                writer.writeU32(name.size());
                for (std::size_t i = 0; i < name.size(); i++) {
                    writer.writeU8(name[i]);
                }
            }
            finishChunk(writer, start);
        }
    }

    if (m_colltris.m_collinfo.size()) {
        std::cout << "Writing 0x100, " << MOD::getChunkName(0x100).value() << std::endl;
        u32 start = startChunk(writer, 0x100);
        writer.writeU32(m_colltris.m_collinfo.size());
        writer.writeU32(m_colltris.m_roominfo.size());
        writer.align(0x20);
        for (BaseRoomInfo& info : m_colltris.m_roominfo) {
            info.write(writer);
        }
        writer.align(0x20);
        for (BaseCollTriInfo& info : m_colltris.m_collinfo) {
            info.write(writer);
        }
        finishChunk(writer, start);

        start = startChunk(writer, 0x110);
        writer.align(0x20);
        m_collgrid.m_boundsMin.write(writer);
        m_collgrid.m_boundsMax.write(writer);
        writer.writeF32(m_collgrid.m_unknown1);
        writer.writeU32(m_collgrid.m_gridX);
        writer.writeU32(m_collgrid.m_gridY);
        writer.writeU32(m_collgrid.m_groups.size());
        for (CollGroup& group : m_collgrid.m_groups) {
            group.write(writer);
        }
        for (s32& i : m_collgrid.m_unknown2) {
            writer.writeS32(i);
        }
        writer.align(0x20);
        finishChunk(writer, start);
    }

    // Finalise writing with 0xFFFF chunk and append any INI file
    finishChunk(writer, startChunk(writer, 0xFFFF));
    if (m_eofBytes.size()) {
        std::cout << "Writing EOF bytes" << std::endl;
        writer.write(reinterpret_cast<char*>(m_eofBytes.data()), m_eofBytes.size());
    }
}

void MOD::reset()
{
    m_vertices.clear();
    m_vnormals.clear();
    m_vertexnbt.clear();
    m_vcolours.clear();
    for (u32 i = 0; i < 8; i++) {
        m_texcoords[i].clear();
    }
    m_textures.clear();
    m_texattrs.clear();
    m_materials.m_materials.clear();
    m_materials.m_texEnvironments.clear();
    m_vtxMatrix.clear();
    m_envelopes.clear();
    m_meshes.clear();
    m_joints.clear();
    m_jointNames.clear();
    m_eofBytes.clear();
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
