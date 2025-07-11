#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

enum class TextureFormat {
	RGB565 = 0,
	CMPR   = 1,
	RGB5A3 = 2,
	I4     = 3,
	I8     = 4,
	IA4    = 5,
	IA8    = 6,
	RGBA32 = 7,
};

struct Texture {
	u16 mWidth            = 0;
	u16 mHeight           = 0;
	TextureFormat mFormat = TextureFormat::RGB565;
	s32 mDataPtrOffset;
	std::vector<u8> mImageData;

	void read(util::fstream_reader&);
	void write(util::fstream_writer&);
};

enum class TextureTilingMode {
	Repeat = 0,
	Clamp  = 1,
};

struct TextureAttributes {
	s16 mIndex            = 0;
	s16 mTilingType       = 0;
	u16 mUseOffsetImgData = 0;
	f32 mLODBias          = 0;

	void read(util::fstream_reader&);
	void write(util::fstream_writer&);
};

#endif