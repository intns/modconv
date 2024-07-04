#pragma once

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <common/colour.hpp>
#include <common/vector3.hpp>
#include <common/vector2.hpp>
#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>
#include <common/gxdefines.hpp>

namespace mat {
struct KeyInfoU8 {
	u8 mAnimationFrame = 0;
	f32 mStartValue    = 0;
	f32 mEndValue      = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, KeyInfoU8 const& k);
};

struct KeyInfoF32 {
	f32 mKeyDataA = 0;
	f32 mKeyDataB = 0;
	f32 mKeyDataC = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, KeyInfoF32 const& k);
};

struct KeyInfoS10 {
	s16 mAnimationFrame = 0;
	f32 mStartValue     = 0;
	f32 mEndValue       = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, KeyInfoS10 const& k);
};

struct ColourAnimInfo {
	s32 mIndex = 0;
	KeyInfoU8 mKeyDataR;
	KeyInfoU8 mKeyDataG;
	KeyInfoU8 mKeyDataB;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, ColourAnimInfo const&);
};

struct AlphaAnimInfo {
	s32 mIndex = 0;
	KeyInfoU8 mKeyData;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
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
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, LightingInfo const&);
};

struct PeInfo {
	u32 mFlags = 0;

	union AlphaCompareFunction {
		struct {
			unsigned int comp0 : 4;
			unsigned int ref0 : 8;
			unsigned int padding : 4;
			unsigned int op : 4;
			unsigned int comp1 : 4;
			unsigned int ref1 : 8;
		} bits;
		u32 value;
	} mAlphaCompareFunction = { 0 };

	u32 mZModeFunction = 0;

	union BlendMode {
		struct {
			unsigned int mType : 4;
			unsigned int mSrcFactor : 4;
			unsigned int mDstFactor : 4;
			unsigned int mLogicOp : 4;
		} bits;
		u32 value;
	} mBlendMode = { 0 };

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, PeInfo const&);
};

struct TexGenData {
	u8 mDestinationCoords = 0;
	u8 mFunc              = 0;
	u8 mSourceParam       = 0;
	u8 mTexMtx            = 0;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, TexGenData const& t);
};

struct TextureAnimData {
	s32 mAnimationFrame = 0;
	KeyInfoF32 mValueX;
	KeyInfoF32 mValueY;
	KeyInfoF32 mValueZ;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, TextureAnimData const& t);
};

struct TextureData {
	s32 mTextureAttributeIndex = 0;
	s16 mUnknown               = 0;
	s16 mUnknown2              = 0;

	u8 mUnknown3 = 0;
	u8 mUnknown4 = 0;
	u8 mUnknown5 = 0;
	u8 mUnknown6 = 0;

	u32 mTextureMtxId = 0;

	s32 mAnimLength = 0;
	f32 mAnimSpeed  = 0;

	Vector2f mScale;
	f32 mRotation = 0;
	Vector2f mPivot;
	Vector2f mPosition;

	std::vector<TextureAnimData> mScaleAnimData;
	std::vector<TextureAnimData> mRotationAnimData;
	std::vector<TextureAnimData> mPivotAnimData;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, TextureData const& t);
};

struct TextureInfo {
	s32 mUnknown1 = 0;
	Vector3f mUnknown2;
	std::vector<TexGenData> mTextureGenData;
	std::vector<TextureData> mTextureData;

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
	u32 mFlags      = 0;
	u32 mUnknown    = 0;
	u32 mTevGroupId = 0;
	PolygonColourInfo mColourInfo;
	LightingInfo mLightingInfo;
	PeInfo mPeInfo;
	TextureInfo mTexInfo;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, Material const&);
};

struct PVWAnimInfo_3_S10 {
	s32 mKeyframeCount = 0;
	KeyInfoS10 mKeyframeA;
	KeyInfoS10 mKeyframeB;
	KeyInfoS10 mKeyframeC;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, PVWAnimInfo_3_S10 const&);
};

struct PVWAnimInfo_1_S10 {
	s32 mKeyframeCount = 0;
	KeyInfoS10 mKeyframeInfo;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
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
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, PVWCombiner const&);
};

struct TEVStage {
	u8 mUnknown;
	u8 mTexCoordID;
	u8 mTexMapID;
	u8 mGXChannelID;
	u8 mKColorSel;
	u8 mKAlphaSel;
	PVWCombiner mTevColorCombiner;
	PVWCombiner mTevAlphaCombiner;

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
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
	std::vector<mat::Material> mMaterials;
	std::vector<mat::TEVInfo> mTevEnvironmentInfo;
};

#endif