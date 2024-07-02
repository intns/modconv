#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <common/colour.hpp>
#include <common/vector3.hpp>
#include <common/vector2.hpp>
#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

namespace mat {
struct KeyInfoU8 {
	u8 mAnimationFrame = 0;
	f32 mStartValue    = 0;
	f32 mEndValue      = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, KeyInfoU8 const& k);
};

struct KeyInfoF32 {
	f32 mKeyDataA = 0;
	f32 mKeyDataB = 0;
	f32 mKeyDataC = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, KeyInfoF32 const& k);
};

struct KeyInfoS10 {
	s16 mAnimationFrame = 0;
	f32 mStartValue     = 0;
	f32 mEndValue       = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, KeyInfoS10 const& k);
};

struct ColourAnimInfo {
	s32 mIndex = 0;
	KeyInfoU8 mKeyDataR;
	KeyInfoU8 mKeyDataG;
	KeyInfoU8 mKeyDataB;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, ColourAnimInfo const&);
};

struct AlphaAnimInfo {
	s32 mIndex = 0;
	KeyInfoU8 mKeyData;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, AlphaAnimInfo const&);
};

struct PolygonColourInfo {
	ColourU8 mDiffuseColour;
	s32 mAnimLength = 0;
	f32 mAnimSpeed  = 0;
	std::vector<ColourAnimInfo> mColourAnimInfo;
	std::vector<AlphaAnimInfo> mAlphaAnimInfo;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, PolygonColourInfo const&);
};

enum struct LightingInfoFlags : u8 {
	LightEnabled    = 0x1,
	SpecularEnabled = 0x2,
	AlphaEnabled    = 0x4,
};

struct LightingInfo {
	u32 mFlags   = 0; // see LightingInfoFlags
	f32 mUnknown = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, LightingInfo const&);
};

struct PeInfo {
	u32 mFlags;

	union AlphaCompareFunction {
		struct {
			unsigned int comp0 : 4;
			unsigned int ref0 : 8;
			unsigned int op : 4;
			unsigned int comp1 : 4;
			unsigned int ref1 : 8;
		} bits;
		u32 value;
	} mAlphaCompareFunction;

	u32 mZModeFunction;

	union BlendMode {
		struct {
			unsigned int mType : 4;
			unsigned int mSrcFactor : 4;
			unsigned int mDstFactor : 4;
			unsigned int mLogicOp : 4;
		} bits;
		u32 value;
	} mBlendMode;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, PeInfo const&);
};

struct TexGenData {
	u8 mDestinationCoords = 0;
	u8 mFunc              = 0;
	u8 mSourceParam       = 0;
	u8 mTexMtx           = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, TexGenData const& t);
};

struct TextureAnimData {
	s32 mAnimationFrame = 0;
	KeyInfoF32 mValueX;
	KeyInfoF32 mValueY;
	KeyInfoF32 mValueZ;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, TextureAnimData const& t);
};

struct TextureData {
	s32 m_texAttrIndex = 0;
	s16 m_unknown2     = 0;
	s16 m_unknown3     = 0;

	u8 m_unknown4 = 0;
	u8 m_unknown5 = 0;
	u8 m_unknown6 = 0;
	u8 m_unknown7 = 0;

	u32 mTextureMtxId = 0;

	s32 mAnimLength = 0;
	f32 mAnimSpeed  = 0;

	Vector2f mUv;
	f32 mRotation = 0;
	Vector2f mPivot;
	Vector2f mPosition;

	std::vector<TextureAnimData> mPositionAnimData;
	std::vector<TextureAnimData> mRotationAnimData;
	std::vector<TextureAnimData> mScaleAnimData;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, TextureData const& t);
};

struct TextureInfo {
	s32 m_unknown1 = 0;
	Vector3f m_unknown2;
	std::vector<TexGenData> m_textureGenData;
	std::vector<TextureData> m_textureData;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, TextureInfo const& ti);
};

enum class MaterialFlags : u32 {
	IsEnabled          = 0x1,
	Opaque             = 0x100,
	AlphaClip          = 0x200,
	TransparentBlend   = 0x400,
	InvertSpecialBlend = 0x8000,
	Hidden             = 0x10000,
};

struct Material {
	u32 m_flags     = 0;
	u32 m_unknown1  = 0;
	u32 mTevGroupId = 0;
	PolygonColourInfo m_colourInfo;
	LightingInfo m_lightingInfo;
	PeInfo m_peInfo;
	TextureInfo m_texInfo;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, Material const&);
};

struct PVWAnimInfo_3_S10 {
	s32 m_unknown1 = 0;
	KeyInfoS10 m_unknown2;
	KeyInfoS10 m_unknown3;
	KeyInfoS10 m_unknown4;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, PVWAnimInfo_3_S10 const&);
};

struct PVWAnimInfo_1_S10 {
	s32 mKeyframeCount = 0;
	KeyInfoS10 mKeyframeInfo;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, PVWAnimInfo_1_S10 const&);
};

struct TEVColReg {
	ColourU16 mColour;
	s32 mAnimLength = 0;
	f32 mAnimSpeed  = 0;
	std::vector<PVWAnimInfo_3_S10> mColorAnimInfo;
	std::vector<PVWAnimInfo_1_S10> mAlphaAnimInfo;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, TEVColReg const&);
};

struct PVWCombiner {
	char mInputABCD[4];
	char mOp;
	char mBias;
	char mScale;
	char mClamp;
	char mOutReg;
	char _unused[3];

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, PVWCombiner const&);
};

struct TEVStage {
	u8 mTevOrders[4];
	u8 mKSelections[2];
	PVWCombiner mTevColorCombiner;
	PVWCombiner mTevAlphaCombiner;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, TEVStage const&);
};

struct TEVInfo {
	// Probably RGB
	TEVColReg mTevColourRegA;
	TEVColReg mTevColourRegB;
	TEVColReg mTevColourRegC;

	ColourU8 mKonstColourA;
	ColourU8 mKonstColourB;
	ColourU8 mKonstColourC;
	ColourU8 mKonstColourD;

	std::vector<TEVStage> mTevStages;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, TEVInfo const&);
};
} // namespace mat

struct MaterialContainer {
	std::vector<mat::Material> m_materials;
	std::vector<mat::TEVInfo> m_tevEnvInfo;
};

#endif