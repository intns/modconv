#include <common/texture.hpp>

void Texture::read(util::fstream_reader& reader)
{
    m_width   = reader.readU16();
    m_height  = reader.readU16();
    m_format  = static_cast<TextureFormat>(reader.readU32());
    
    for (u32 i = 0; i < 5; i++) {
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
    writer.writeU32(static_cast<u32>(m_format));
    
    for (u32 i = 0; i < 5; i++) {
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
    m_tilingModeS = reader.readU16();
    m_tilingModeT   = reader.readU16();
    m_widthPercent   = reader.readF32();
}

void TextureAttributes::write(util::fstream_writer& writer)
{
    writer.writeU16(m_index);
    writer.writeU16(0);
    writer.writeU16(m_tilingModeS);
    writer.writeU16(m_tilingModeT);
    writer.writeF32(m_widthPercent);
}
