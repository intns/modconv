#include <common/material.hpp>
#include <iostream>

#define PrintIndent(os, tabCnt, item)                                          \
    for (std::size_t idntIdx = 0; idntIdx < tabCnt; idntIdx++) {               \
        os << '\t';                                                            \
    }                                                                          \
    os << item << std::endl;

template <typename T>
static inline void
PrintList(std::ostream& os, T& vector, const std::string& size_msg,
          const std::string& itemIdx_msg, const u32 itemTabCount = 1)
{
    // EXAMPLE:
    // TEXGEN_SIZE 2
    PrintIndent(os, itemTabCount - 1, size_msg << " " << vector.size());

    std::size_t index = 0;
    for (auto& item : vector) {
        PrintIndent(os, itemTabCount, itemIdx_msg << " " << index++);
        PrintIndent(os, itemTabCount, item);
    }
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

std::ostream& operator<<(std::ostream& os, KeyInfoU8 const& k)
{
    os << (u32)k.m_unknown1 << " " << k.m_unknown2 << " " << k.m_unknown3;
    return os;
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

std::ostream& operator<<(std::ostream& os, KeyInfoF32 const& k)
{
    os << k.m_unknown1 << " " << k.m_unknown2 << " " << k.m_unknown3;
    return os;
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

std::ostream& operator<<(std::ostream& os, KeyInfoS10 const& k)
{
    os << k.m_unknown1 << " " << k.m_unknown2 << " " << k.m_unknown3;
    return os;
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

std::ostream& operator<<(std::ostream& os, PCI_Unk1 const& p)
{
    os << "\t\tPCI_UNK1" << std::endl;
    os << "\t\t\tUNK1 " << p.m_unknown1 << std::endl;
    os << "\t\t\tUNK2_KEYFRAME " << p.m_unknown2 << std::endl;
    os << "\t\t\tUNK3_KEYFRAME " << p.m_unknown3 << std::endl;
    os << "\t\t\tUNK4_KEYFRAME " << p.m_unknown4 << std::endl;
    return os;
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

std::ostream& operator<<(std::ostream& os, PCI_Unk2 const& p)
{
    os << "\t\tPCI_UNK2" << std::endl;
    os << "\t\t\tUNK1 " << p.m_unknown1 << std::endl;
    os << "\t\t\tUNK2_KEYFRAME " << p.m_unknown2 << std::endl;
    return os;
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

std::ostream& operator<<(std::ostream& os, PolygonColourInfo const& p)
{
    PrintIndent(os, 1, "POLYGON_COLOUR_INFO");
    PrintIndent(os, 2, "DIFFUSE_COLOUR " << p.m_diffuseColour);
    PrintIndent(os, 2, "UNK2 " << p.m_unknown2);
    PrintIndent(os, 2, "UNK3 " << p.m_unknown3);
    PrintList(os, p.m_unknown4, "UNK4_SIZE", "UNK4_INDEX", 3);
    PrintList(os, p.m_unknown5, "UNK5_SIZE", "UNK5_INDEX", 3);

    return os;
}

void LightingInfo::read(util::fstream_reader& reader)
{
    m_typeFlags = reader.readU32();
    m_unknown2  = reader.readF32();
}

void LightingInfo::write(util::fstream_writer& writer)
{
    writer.writeU32(m_typeFlags);
    writer.writeF32(m_unknown2);
}

std::ostream& operator<<(std::ostream& os, LightingInfo const& l)
{
    os << "\tLIGHTING_INFO" << std::endl;
    os << "\t\tTYPE_FLAGS " << ((l.m_typeFlags & 1) ? "USE_SPOTLIGHT" : "NONE")
       << std::endl;
    os << "\t\tUNK2 " << l.m_unknown2 << std::endl;
    return os;
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

std::ostream& operator<<(std::ostream& os, PeInfo const& p)
{
    os << "\tPE_INFO" << std::endl;
    os << "\t\tUNK1 " << p.m_unknown1 << std::endl;
    PrintIndent(os, 2, "UNK2 " << p.m_unknown2);
    os << "\t\tUNK3 " << p.m_unknown3 << std::endl;
    os << "\t\tUNK4 " << p.m_unknown4 << std::endl;
    return os;
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

std::ostream& operator<<(std::ostream& os, TexGenData const& t)
{
    os << (u32)t.m_unknown1 << " " << (u32)t.m_unknown2 << " "
       << (u32)t.m_unknown3 << " " << (u32)t.m_unknown4 << std::endl;
    return os;
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

std::ostream& operator<<(std::ostream& os, TXD_Unk1 const& t)
{
    os << "\tUNK1 " << t.m_unknown1 << std::endl;
    PrintIndent(os, 7, "UNK2 " << t.m_unknown2);
    PrintIndent(os, 7, "UNK3 " << t.m_unknown3);
    PrintIndent(os, 7, "UNK4 " << t.m_unknown4);
    return os;
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
    for (TXD_Unk1& unk : m_unknown18) {
        unk.write(writer);
    }

    writer.writeU32(m_unknown19.size());
    for (TXD_Unk1& unk : m_unknown19) {
        unk.write(writer);
    }

    writer.writeU32(m_unknown20.size());
    for (TXD_Unk1& unk : m_unknown20) {
        unk.write(writer);
    }
}

std::ostream& operator<<(std::ostream& os, TextureData const& t)
{
    os << "\tUNK1 " << t.m_unknown1 << std::endl;
    PrintIndent(os, 5, "UNK2 " << t.m_unknown2);
    PrintIndent(os, 5, "UNK3 " << t.m_unknown3);
    PrintIndent(os, 5, "UNK4 " << (u32)t.m_unknown4);
    PrintIndent(os, 5, "UNK5 " << (u32)t.m_unknown5);
    PrintIndent(os, 5, "UNK6 " << (u32)t.m_unknown6);
    PrintIndent(os, 5, "UNK7 " << (u32)t.m_unknown7);
    PrintIndent(os, 5, "UNK8 " << (u32)t.m_unknown8);
    PrintIndent(os, 5, "UNK9 " << (u32)t.m_unknown9);
    PrintIndent(os, 5, "UNK10 " << t.m_unknown10);
    PrintIndent(os, 5, "UNK11 " << t.m_unknown11);
    PrintIndent(os, 5, "UNK12 " << t.m_unknown12);
    PrintIndent(os, 5, "UNK13 " << t.m_unknown13);
    PrintIndent(os, 5, "UNK14 " << t.m_unknown14);
    PrintIndent(os, 5, "UNK15 " << t.m_unknown15);
    PrintIndent(os, 5, "UNK16 " << t.m_unknown16);
    PrintIndent(os, 5, "UNK17 " << t.m_unknown17);

    PrintList(os, t.m_unknown18, "UNK18_SIZE", "UNK18_IDX", 6);
    PrintList(os, t.m_unknown19, "UNK19_SIZE", "UNK19_IDX", 6);
    PrintList(os, t.m_unknown20, "UNK20_SIZE", "UNK20_INDEX", 6);

    return os;
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

std::ostream& operator<<(std::ostream& os, TextureInfo const& ti)
{
    os << "\tTEXTURE_INFO" << std::endl;
    os << "\t\tUNK1 " << ti.m_unknown1 << std::endl;
    os << "\t\tUNK2 " << ti.m_unknown2 << std::endl;

    u32 idx = 0;
    os << "\t\tTEXGEN_SIZE " << ti.m_unknown3.size() << std::endl;
    for (const TexGenData& txgen : ti.m_unknown3) {
        os << "\t\t\t"
           << "TXG" << idx << " " << txgen;
        idx++;
    }

    PrintList(os, ti.m_unknown4, "TEXDATA_SIZE", "TEXDATA_IDX", 4);
    /*idx = 0;
    os << "\t\tTEXDATA_SIZE " << ti.m_unknown4.size() << std::endl;
    for (const TextureData& txdat : ti.m_unknown4) {
        os << "\t\t\tTXD" << idx << std::endl << txdat;
        idx++;
    }*/

    return os;
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

std::ostream& operator<<(std::ostream& os, Material const& m)
{
    os << "\tFLAGS " << m.m_flags << std::endl;
    os << "\tUNK1 " << std::hex << "0x" << m.m_unknown1 << std::dec
       << std::endl;
    os << "\tCOLOUR " << m.m_colour << std::endl;

    if (m.m_flags & static_cast<u32>(mat::MaterialFlags::UsePVW)) {
        os << "\tPVW_UNK " << m.m_unknown2 << std::endl;
        os << m.m_colourInfo;
        os << m.m_lightingInfo;
        os << m.m_peInfo;
        os << m.m_texInfo;
    }
    os << std::endl;

    return os;
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

std::ostream& operator<<(std::ostream& os, TCR_Unk1 const& t)
{
    os << "\t\t\tUNK1 " << t.m_unknown1 << std::endl;
    os << "\t\t\tUNK2 " << t.m_unknown2 << std::endl;
    os << "\t\t\tUNK3 " << t.m_unknown3 << std::endl;
    os << "\t\t\tUNK4 " << t.m_unknown3 << std::endl;
    return os;
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

std::ostream& operator<<(std::ostream& os, TCR_Unk2 const& t)
{
    os << "\t\t\tUNK1 " << t.m_unknown1 << std::endl;
    os << "\t\t\tUNK2 " << t.m_unknown2 << std::endl;
    return os;
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

std::ostream& operator<<(std::ostream& os, TEVColReg const& t)
{
    os << "\t\tTEVCOLREG1 " << t.m_unknown1 << std::endl;
    os << "\t\tUNK2 " << t.m_unknown2 << std::endl;
    os << "\t\tUNK3 " << t.m_unknown3 << std::endl;

    os << "\t\tUNK4_SIZE " << t.m_unknown4.size() << std::endl;
    for (const TCR_Unk1& elem : t.m_unknown4) {
        os << elem;
    }

    os << "\t\tUNK5_SIZE " << t.m_unknown5.size() << std::endl;
    for (const TCR_Unk2& elem : t.m_unknown5) {
        os << elem;
    }

    os << std::endl;

    return os;
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

std::ostream& operator<<(std::ostream& os, PVWCombiner const& i)
{
    os << (u32)i.m_unknown1 << " " << (u32)i.m_unknown2 << " "
       << (u32)i.m_unknown3 << " " << (u32)i.m_unknown4 << " "
       << (u32)i.m_unknown5 << " " << (u32)i.m_unknown6 << " "
       << (u32)i.m_unknown7 << " " << (u32)i.m_unknown8 << " "
       << (u32)i.m_unknown9 << " " << (u32)i.m_unknown10 << " "
       << (u32)i.m_unknown11 << " " << (u32)i.m_unknown12 << std::endl;

    return os;
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

std::ostream& operator<<(std::ostream& os, TEVStage const& i)
{
    os << "\t\t\tUNK1 " << (u32)i.m_unknown1 << std::endl;
    os << "\t\t\tUNK2 " << (u32)i.m_unknown2 << std::endl;
    os << "\t\t\tUNK3 " << (u32)i.m_unknown3 << std::endl;
    os << "\t\t\tUNK4 " << (u32)i.m_unknown4 << std::endl;
    os << "\t\t\tUNK5 " << (u32)i.m_unknown5 << std::endl;
    os << "\t\t\tUNK6 " << (u32)i.m_unknown6 << std::endl;
    os << "\t\t\tUNK7 " << i.m_unknown7;
    os << "\t\t\tUNK8 " << i.m_unknown8;
    os << std::endl;

    return os;
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

std::ostream& operator<<(std::ostream& os, TEVInfo const& i)
{
    os << "\tTEVCOLREG1" << std::endl << i.m_unknown1;
    os << "\tTEVCOLREG2" << std::endl << i.m_unknown2;
    os << "\tTEVCOLREG3" << std::endl << i.m_unknown3;
    os << "\tUNK4 " << i.m_unknown4;
    os << "\tUNK5 " << i.m_unknown5;
    os << "\tUNK6 " << i.m_unknown6;
    os << "\tUNK7 " << i.m_unknown7;
    os << "\tUNK8_SIZE " << i.m_unknown8.size() << std::endl;
    u32 tevIdx = 0;
    for (const TEVStage& elem : i.m_unknown8) {
        os << "\t\tTEVSTAGE" << tevIdx++ << std::endl;
        os << elem;
    }

    os << std::endl;

    return os;
}

} // namespace mat
