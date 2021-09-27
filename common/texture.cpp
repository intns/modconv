#include <common/texture.hpp>

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
    reader.read_buffer(reinterpret_cast<char*>(m_imageData.data()),
                       m_imageData.size());
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
    writer.write(reinterpret_cast<char*>(m_imageData.data()),
                 m_imageData.size());
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
