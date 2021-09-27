#include "MOD.hpp"
#include <iostream>
#include <map>

namespace {
static inline void writeGenericChunk(util::fstream_writer& writer, auto& vector,
                                     u32 chunkIdentifier)
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

    reader.align(0x20);
    for (auto& elem : vector) {
        elem.read(reader);
    }
    reader.align(0x20);
}
} // namespace

void MOD::read(util::fstream_reader& reader)
{
    bool stopRead = false;
    while (!stopRead) {
        std::streampos position = reader.tellg();
        u32 opcode              = reader.readU32();
        u32 length              = reader.readU32();

        if (position & 0x1F) {
            std::cout << "Error in chunk " << opcode << ", offset " << position
                      << ", chunk start isn't aligned to 0x20, this means an "
                         "improper read occured."
                      << std::endl;
            return;
        }

        const auto ocString = getChunkName(opcode);
        std::cout << "Reading 0x" << std::hex << opcode << std::dec << ", "
                  << (ocString.has_value() ? ocString.value() : "Unknown chunk")
                  << std::endl;

        switch (opcode) {
        case 0:
            reader.align(0x20);
            m_header.m_dateTime.m_year  = reader.readU16();
            m_header.m_dateTime.m_month = reader.readU8();
            m_header.m_dateTime.m_day   = reader.readU8();
            m_header.m_flags            = reader.readU32();
            reader.align(0x20);
            break;
        case 0x10:
            readGenericChunk(reader, m_vertices);
            break;
        case 0x11:
            readGenericChunk(reader, m_vnormals);
            break;
        case 0x12:
            readGenericChunk(reader, m_vertexnbt);
            break;
        case 0x13:
            readGenericChunk(reader, m_vcolours);
            break;
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            readGenericChunk(reader, m_texcoords[opcode - 0x18]);
            break;
        case 0x20:
            readGenericChunk(reader, m_textures);
            break;
        case 0x22:
            readGenericChunk(reader, m_texattrs);
            break;
        case 0x30:
            m_materials.m_materials.resize(reader.readU32());
            m_materials.m_texEnvironments.resize(reader.readU32());

            reader.align(0x20);
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
            reader.align(0x20);
            break;
        case 0x40:
            readGenericChunk(reader, m_vtxMatrix);
            break;
        case 0x41:
            readGenericChunk(reader, m_envelopes);
            break;
        case 0x50:
            readGenericChunk(reader, m_meshes);
            break;
        case 0x60:
            readGenericChunk(reader, m_joints);
            break;
        case 0x61:
            m_jointNames.resize(reader.readU32());
            reader.align(0x20);
            for (std::string& str : m_jointNames) {
                str.resize(reader.readU32());
                for (u32 i = 0; i < str.size(); i++) {
                    str[i] = reader.readU8();
                }
            }
            reader.align(0x20);
            break;
        case 0x100:
            m_colltris.read(reader);
            break;
        case 0x110:
            m_collgrid.read(reader);
            break;
        case 0xFFFF:
            reader.seekg(
                static_cast<
                    std::basic_istream<char, std::char_traits<char>>::off_type>(
                    length),
                std::ios_base::cur);

            while (!reader.eof()) {
                m_eofBytes.push_back(reader.get());
                reader.peek();
            }

            stopRead = true;
            break;
        default:
            reader.seekg(
                static_cast<
                    std::basic_istream<char, std::char_traits<char>>::off_type>(
                    length),
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

    if (m_header.m_flags & static_cast<u32>(MODFlags::UseNBT)
        && m_vertexnbt.size()) {
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
        std::cout << "Writing 0x30, " << MOD::getChunkName(0x30).value()
                  << std::endl;

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
            std::cout << "Writing 0x61, " << MOD::getChunkName(0x61).value()
                      << std::endl;

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
        std::cout << "Writing 0x100, " << MOD::getChunkName(0x100).value()
                  << std::endl;
        m_colltris.write(writer);

        const u32 start = startChunk(writer, 0x110);
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
        std::cout << "Writing 0xffff, " << MOD::getChunkName(0xffff).value()
                  << std::endl;

        writer.write(reinterpret_cast<char*>(m_eofBytes.data()),
                     m_eofBytes.size());
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

    m_colltris.m_collinfo.clear();
    m_colltris.m_roominfo.clear();

    m_collgrid.clear();

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
