#include "texture.hpp"

void Texture::read(util::fstream_reader& reader)
{
	mWidth         = reader.readU16();
	mHeight        = reader.readU16();
	mFormat        = static_cast<TextureFormat>(reader.readU32());
	mDataPtrOffset = reader.readS32();

	for (u32 i = 0; i < 4; i++) {
		reader.readU32();
	}

	mImageData.resize(reader.readU32());
	reader.read_buffer(reinterpret_cast<char*>(mImageData.data()), mImageData.size());
}

void Texture::write(util::fstream_writer& writer)
{
	writer.writeU16(mWidth);
	writer.writeU16(mHeight);
	writer.writeU32(static_cast<u32>(mFormat));
	writer.writeS32(mDataPtrOffset);

	for (u32 i = 0; i < 4; i++) {
		writer.writeU32(0);
	}

	writer.writeU32(static_cast<u32>(mImageData.size()));
	writer.write(reinterpret_cast<char*>(mImageData.data()), mImageData.size());
}

void TextureAttributes::read(util::fstream_reader& reader)
{
	mIndex = reader.readS16();
	reader.readU16();
	mTilingType       = reader.readS16();
	mUseOffsetImgData = reader.readU16();
	mLODBias          = reader.readF32();
}

void TextureAttributes::write(util::fstream_writer& writer) const
{
	writer.writeS16(mIndex);
	writer.writeU16(0);
	writer.writeS16(mTilingType);
	writer.writeU16(mUseOffsetImgData);
	writer.writeF32(mLODBias);
}
