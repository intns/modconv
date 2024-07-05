#include <common/texture.hpp>

void Texture::read(util::fstream_reader& reader)
{
    mWidth   = reader.readU16();
    mHeight  = reader.readU16();
    mFormat  = static_cast<TextureFormat>(reader.readU32());
    
    for (u32 i = 0; i < 5; i++) {
        reader.readU32();
    }

    mImageData.resize(reader.readU32());
    reader.read_buffer(reinterpret_cast<char*>(mImageData.data()),
                       mImageData.size());
}

void Texture::write(util::fstream_writer& writer)
{
    writer.writeU16(mWidth);
    writer.writeU16(mHeight);
    writer.writeU32(static_cast<u32>(mFormat));
    
    for (u32 i = 0; i < 5; i++) {
        writer.writeU32(0);
    }

    writer.writeU32(static_cast<u32>(mImageData.size()));
    writer.write(reinterpret_cast<char*>(mImageData.data()),
                 mImageData.size());
}

void TextureAttributes::read(util::fstream_reader& reader)
{
    mIndex = reader.readU16();
    reader.readU16();
    mWrapFlags = reader.readU16();
    mUnknown   = reader.readU16();
    mWidthPercent   = reader.readF32();
}

void TextureAttributes::write(util::fstream_writer& writer)
{
    writer.writeU16(mIndex);
    writer.writeU16(0);
    writer.writeU16(mWrapFlags);
    writer.writeU16(mUnknown);
    writer.writeF32(mWidthPercent);
}
