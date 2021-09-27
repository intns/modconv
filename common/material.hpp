#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <common/colour.hpp>
#include <common/vector3.hpp>
#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

namespace mat {
////////////////////////////////////////////////////////////////////
// NOTE: the names of the classes are taken directly from sysCore //
// with the exception of unknowns (_Unk)                          //
////////////////////////////////////////////////////////////////////
// Also, I am using signed types because I am unsure whether or   //
// not negative values are needed                                 //
////////////////////////////////////////////////////////////////////
// PCI = PolygonColourInfo                                        //
// TXD = TextureData                                              //
// TEV = TextureEnvironment                                       //
// TCR = Texture Environment (TEV) Colour Register                //
////////////////////////////////////////////////////////////////////

struct KeyInfoU8 {
    u8 m_unknown1  = 0;
    f32 m_unknown2 = 0;
    f32 m_unknown3 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream& os, KeyInfoU8 const& k);
};

struct KeyInfoF32 {
    f32 m_unknown1 = 0;
    f32 m_unknown2 = 0;
    f32 m_unknown3 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream& os, KeyInfoF32 const& k);
};

struct KeyInfoS10 {
    s16 m_unknown1 = 0;
    f32 m_unknown2 = 0;
    f32 m_unknown3 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream& os, KeyInfoS10 const& k);
};

struct PCI_Unk1 {
    s32 m_unknown1 = 0;
    KeyInfoU8 m_unknown2;
    KeyInfoU8 m_unknown3;
    KeyInfoU8 m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, PCI_Unk1 const&);
};

struct PCI_Unk2 {
    s32 m_unknown1 = 0;
    KeyInfoU8 m_unknown2;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, PCI_Unk2 const&);
};

struct PolygonColourInfo {
    ColourU8 m_diffuseColour;
    s32 m_unknown2 = 0;
    f32 m_unknown3 = 0;
    std::vector<PCI_Unk1> m_unknown4;
    std::vector<PCI_Unk2> m_unknown5;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, PolygonColourInfo const&);
};

enum struct LightingInfoFlags : u8 {
    USE_SPOTLIGHT = 1
};

struct LightingInfo {
    u32 m_typeFlags = 0; // see LightingInfoFlags
    f32 m_unknown2 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, LightingInfo const&);
};

struct PeInfo {
    s32 m_unknown1 = 0;
    s32 m_unknown2 = 0;
    s32 m_unknown3 = 0;
    s32 m_unknown4 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, PeInfo const&);
};

struct TexGenData {
    u8 m_unknown1 = 0;
    u8 m_unknown2 = 0;
    u8 m_unknown3 = 0;
    u8 m_unknown4 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream& os, TexGenData const& t);
};

struct TXD_Unk1 {
    s32 m_unknown1 = 0;
    KeyInfoF32 m_unknown2;
    KeyInfoF32 m_unknown3;
    KeyInfoF32 m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream& os, TXD_Unk1 const& t);
};

struct TextureData {
    s32 m_unknown1 = 0;
    s16 m_unknown2 = 0;
    s16 m_unknown3 = 0;

    u8 m_unknown4 = 0;
    u8 m_unknown5 = 0;
    u8 m_unknown6 = 0;
    u8 m_unknown7 = 0;

    u32 m_unknown8 = 0;
    s32 m_unknown9 = 0;

    f32 m_unknown10 = 0;
    f32 m_unknown11 = 0;
    f32 m_unknown12 = 0;
    f32 m_unknown13 = 0;
    f32 m_unknown14 = 0;
    f32 m_unknown15 = 0;
    f32 m_unknown16 = 0;
    f32 m_unknown17 = 0;

    std::vector<TXD_Unk1> m_unknown18;
    std::vector<TXD_Unk1> m_unknown19;
    std::vector<TXD_Unk1> m_unknown20;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream& os, TextureData const& t);
};

struct TextureInfo {
    s32 m_unknown1 = 0;
    Vector3f m_unknown2;
    std::vector<TexGenData> m_unknown3;
    std::vector<TextureData> m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream& os, TextureInfo const& ti);
};

enum class MaterialFlags : u8 { UsePVW = 1 };

struct Material {
    u32 m_flags    = 0;
    u32 m_unknown1 = 0;
    ColourU8 m_colour;

    u32 m_unknown2 = 0;
    PolygonColourInfo m_colourInfo;
    LightingInfo m_lightingInfo;
    PeInfo m_peInfo;
    TextureInfo m_texInfo;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, Material const&);
};

struct TCR_Unk1 {
    s32 m_unknown1 = 0;
    KeyInfoS10 m_unknown2;
    KeyInfoS10 m_unknown3;
    KeyInfoS10 m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, TCR_Unk1 const&);
};

struct TCR_Unk2 {
    s32 m_unknown1 = 0;
    KeyInfoS10 m_unknown2;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, TCR_Unk2 const&);
};

struct TEVColReg {
    ColourU16 m_unknown1;
    s32 m_unknown2 = 0;
    f32 m_unknown3 = 0;
    std::vector<TCR_Unk1> m_unknown4;
    std::vector<TCR_Unk2> m_unknown5;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, TEVColReg const&);
};

struct PVWCombiner {
    u8 m_unknown1  = 0;
    u8 m_unknown2  = 0;
    u8 m_unknown3  = 0;
    u8 m_unknown4  = 0;
    u8 m_unknown5  = 0;
    u8 m_unknown6  = 0;
    u8 m_unknown7  = 0;
    u8 m_unknown8  = 0;
    u8 m_unknown9  = 0;
    u8 m_unknown10 = 0;
    u8 m_unknown11 = 0;
    u8 m_unknown12 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, PVWCombiner const&);
};

struct TEVStage {
    u8 m_unknown1 = 0;
    u8 m_unknown2 = 0;
    u8 m_unknown3 = 0;
    u8 m_unknown4 = 0;
    u8 m_unknown5 = 0;
    u8 m_unknown6 = 0;
    PVWCombiner m_unknown7;
    PVWCombiner m_unknown8;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, TEVStage const&);
};

struct TEVInfo {
    // Probably RGB
    TEVColReg m_unknown1;
    TEVColReg m_unknown2;
    TEVColReg m_unknown3;

    ColourU8 m_unknown4;
    ColourU8 m_unknown5;
    ColourU8 m_unknown6;
    ColourU8 m_unknown7;

    std::vector<TEVStage> m_unknown8;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    friend std::ostream& operator<<(std::ostream&, TEVInfo const&);
};
} // namespace mat

struct MaterialContainer {
    std::vector<mat::Material> m_materials;
    std::vector<mat::TEVInfo> m_texEnvironments;
};

#endif