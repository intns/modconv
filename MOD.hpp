#include <array>
#include <format>
#include <optional>
#include <string_view>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>
#include <vector>

struct Vector2i {
    u32 x = 0, y = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct Vector2f {
    f32 x = 0, y = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct Vector3i {
    u32 x = 0, y = 0, z = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct Vector3f {
    f32 x = 0, y = 0, z = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct Colour {
    u8 r = 0, g = 0, b = 0, a = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct ShortColour {
    u16 r = 0, g = 0, b = 0, a = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct Header {
    struct {
        u16 m_year = 2021;
        u8 m_month = 9;
        u8 m_day   = 18;
    } m_dateTime;

    u32 m_flags = 0;
};

struct NBT {
    Vector3f m_normals;
    Vector3f m_binormals;
    Vector3f m_tangent;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct Texture {
    u16 m_width   = 0;
    u16 m_height  = 0;
    u32 m_format  = 0;
    u32 m_unknown = 0;
    std::vector<u8> m_imageData;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct TextureAttributes {
    u16 m_index      = 0;
    u16 m_tilingMode = 0;
    u16 m_unknown1   = 0;
    f32 m_unknown2   = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

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
};

struct KeyInfoF32 {
    f32 m_unknown1 = 0;
    f32 m_unknown2 = 0;
    f32 m_unknown3 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct KeyInfoS10 {
    s16 m_unknown1 = 0;
    f32 m_unknown2 = 0;
    f32 m_unknown3 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct PCI_Unk1 {
    s32 m_unknown1 = 0;
    KeyInfoU8 m_unknown2;
    KeyInfoU8 m_unknown3;
    KeyInfoU8 m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct PCI_Unk2 {
    s32 m_unknown1 = 0;
    KeyInfoU8 m_unknown2;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct PolygonColourInfo {
    Colour m_diffuseColour;
    s32 m_unknown2 = 0;
    f32 m_unknown3 = 0;
    std::vector<PCI_Unk1> m_unknown4;
    std::vector<PCI_Unk2> m_unknown5;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct LightingInfo {
    u32 m_unknown1 = 0;
    f32 m_unknown2 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct PeInfo {
    s32 m_unknown1 = 0;
    s32 m_unknown2 = 0;
    s32 m_unknown3 = 0;
    s32 m_unknown4 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct TexGenData {
    u8 m_unknown1 = 0;
    u8 m_unknown2 = 0;
    u8 m_unknown3 = 0;
    u8 m_unknown4 = 0;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct TXD_Unk1 {
    s32 m_unknown1 = 0;
    KeyInfoF32 m_unknown2;
    KeyInfoF32 m_unknown3;
    KeyInfoF32 m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
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
};

struct TextureInfo {
    s32 m_unknown1 = 0;
    Vector3f m_unknown2;
    std::vector<TexGenData> m_unknown3;
    std::vector<TextureData> m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

enum class MaterialFlags : u8 { UsePVW = 1 };

struct Material {
    u32 m_flags    = 0;
    u32 m_unknown1 = 0;
    Colour m_colour;

    u32 m_unknown2 = 0;
    PolygonColourInfo m_colourInfo;
    LightingInfo m_lightingInfo;
    PeInfo m_peInfo;
    TextureInfo m_texInfo;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct TCR_Unk1 {
    s32 m_unknown1 = 0;
    KeyInfoS10 m_unknown2;
    KeyInfoS10 m_unknown3;
    KeyInfoS10 m_unknown4;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct TCR_Unk2 {
    s32 m_unknown1 = 0;
    KeyInfoS10 m_unknown2;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};

struct TEVColReg {
    ShortColour m_unknown1;
    s32 m_unknown2 = 0;
    f32 m_unknown3 = 0;
    std::vector<TCR_Unk1> m_unknown4;
    std::vector<TCR_Unk2> m_unknown5;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
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
};

struct TEVInfo {
    // Probably RGB
    TEVColReg m_unknown1;
    TEVColReg m_unknown2;
    TEVColReg m_unknown3;

    Colour m_unknown4;
    Colour m_unknown5;
    Colour m_unknown6;
    Colour m_unknown7;

    std::vector<TEVStage> m_unknown8;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
};
} // namespace mat

struct MaterialContainer {
    std::vector<mat::Material> m_materials;
    std::vector<mat::TEVInfo> m_texEnvironments;
};

struct VtxMatrix {
    u16 m_index = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

enum class MODFlags : u8 { UseNBT = 0x01 };

struct MOD {
    MOD() = default;
    MOD(util::fstream_reader& reader) { read(reader); }
    ~MOD() = default;

    void read(util::fstream_reader& reader);
    void write(util::fstream_writer& writer);
    void reset();

    static const std::optional<std::string_view> getChunkName(u32 opcode);

    Header m_header;
    std::vector<Vector3f> m_vertices;
    std::vector<Vector3f> m_vnormals;
    std::vector<NBT> m_vertexnbt;
    std::vector<Colour> m_vcolours;
    std::array<std::vector<Vector2f>, 8> m_texcoords;
    std::vector<Texture> m_textures;
    std::vector<TextureAttributes> m_texattrs;
    MaterialContainer m_materials;
    std::vector<VtxMatrix> m_vtxMatrix;
};