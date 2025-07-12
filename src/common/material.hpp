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
#include <iostream>

namespace mat {
struct KeyInfoU8 {
	u8 mTime     = 0;
	f32 mValue   = 0;
	f32 mTangent = 0;

	bool operator==(const KeyInfoU8& other) const
	{
		return mTime == other.mTime && nearly_equal(mValue, other.mValue) && nearly_equal(mTangent, other.mTangent);
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, KeyInfoU8 const& k);
};

struct KeyInfoF32 {
	f32 mTime    = 0;
	f32 mValue   = 0;
	f32 mTangent = 0;

	bool operator==(const KeyInfoF32& other) const
	{
		return nearly_equal(mTime, other.mTime) && nearly_equal(mValue, other.mValue) && nearly_equal(mTangent, other.mTangent);
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, KeyInfoF32 const& k);
};

struct KeyInfoS10 {
	s16 mTime    = 0;
	f32 mValue   = 0;
	f32 mTangent = 0;

	bool operator==(const KeyInfoS10& other) const
	{
		return mTime == other.mTime && nearly_equal(mValue, other.mValue) && nearly_equal(mTangent, other.mTangent);
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, KeyInfoS10 const& k);
};

struct ColourAnimInfo {
	s32 mIndex = 0;
	KeyInfoU8 mKeyDataR;
	KeyInfoU8 mKeyDataG;
	KeyInfoU8 mKeyDataB;

	bool operator==(const ColourAnimInfo& other) const
	{
		return mIndex == other.mIndex && mKeyDataR == other.mKeyDataR && mKeyDataG == other.mKeyDataG && mKeyDataB == other.mKeyDataB;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, ColourAnimInfo const&);
};

struct AlphaAnimInfo {
	s32 mIndex = 0;
	KeyInfoU8 mKeyData;

	bool operator==(const AlphaAnimInfo& other) const { return mIndex == other.mIndex && mKeyData == other.mKeyData; }

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

	bool operator==(const PolygonColourInfo& other) const
	{
		return mDiffuseColour == other.mDiffuseColour && mAnimLength == other.mAnimLength && nearly_equal(mAnimSpeed, other.mAnimSpeed)
		    && mColourAnimInfo == other.mColourAnimInfo && mAlphaAnimInfo == other.mAlphaAnimInfo;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, PolygonColourInfo const&);
};

enum struct LightingInfoFlags : u32 {
	EnableColor0    = 0x0001, // Enable lighting for color channel 0
	EnableSpecular  = 0x0002, // Enable specular lighting (color channel 1)
	EnableAlpha0    = 0x0004, // Enable lighting for alpha channel 0
	Unknown8        = 0x0008,
	Unknown10       = 0x0010,
	Unknown20       = 0x0020,
	Unknown40       = 0x0040,
	Unknown80       = 0x0080,
	Unknown100      = 0x0100,
	AmbSrcColor0Vtx = 0x0200, // Use vertex color for ambient color 0 (vs register)
	AmbSrcAlpha0Vtx = 0x0400, // Use vertex alpha for ambient alpha 0 (vs register)
	MatSrcColor0Vtx = 0x0800, // Use vertex color for material color 0 (vs register)
	MatSrcAlpha0Vtx = 0x1000, // Use vertex alpha for material alpha 0 (vs register)
};

struct LightingInfo {
	u32 mFlags   = static_cast<u32>(LightingInfoFlags::EnableColor0); // see LightingInfoFlags
	f32 mUnknown = 0;

	bool operator==(const LightingInfo& other) const { return mFlags == other.mFlags && nearly_equal(mUnknown, other.mUnknown); }

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
	} mAlphaCompareFunction = {};

	u32 mZModeFunction = 0;

	union BlendMode {
		struct {
			unsigned int mType : 4;
			unsigned int mSrcFactor : 4;
			unsigned int mDstFactor : 4;
			unsigned int mLogicOp : 4;
		} bits;
		u32 value;
	} mBlendMode = {};

	bool operator==(const PeInfo& other) const
	{
		return mFlags == other.mFlags && mAlphaCompareFunction.value == other.mAlphaCompareFunction.value
		    && mZModeFunction == other.mZModeFunction && mBlendMode.value == other.mBlendMode.value;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, PeInfo const&);
};

struct TexGenData {
	u8 mDestinationCoords = 0;
	u8 mFunc              = 0;
	u8 mSourceParam       = 0;
	u8 mTexMtx            = 0;

	bool operator==(const TexGenData& other) const
	{
		return mDestinationCoords == other.mDestinationCoords && mFunc == other.mFunc && mSourceParam == other.mSourceParam
		    && mTexMtx == other.mTexMtx;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, TexGenData const& t);
};

struct TextureAnimData {
	s32 mAnimationFrame = 0;
	KeyInfoF32 mValueX;
	KeyInfoF32 mValueY;
	KeyInfoF32 mValueZ;

	bool operator==(const TextureAnimData& other) const
	{
		return mAnimationFrame == other.mAnimationFrame && mValueX == other.mValueX && mValueY == other.mValueY && mValueZ == other.mValueZ;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream& os, TextureAnimData const& t);
};

struct TextureData {
	s32 mTextureAttributeIndex = 0;
	s16 mWrapModeS             = 0;
	s16 mWrapModeT             = 0;

	u8 mUnknown3 = 0;
	u8 mUnknown4 = 0;
	u8 mUnknown5 = 0;
	u8 mUnknown6 = 0;

	s32 mAnimationFactor = 0;

	s32 mAnimLength = 0;
	f32 mAnimSpeed  = 0;

	Vector2f mScale;
	f32 mRotation = 0;
	Vector2f mPivot;
	Vector2f mPosition;

	std::vector<TextureAnimData> mScaleInfo;
	std::vector<TextureAnimData> mRotationInfo;
	std::vector<TextureAnimData> mTranslationInfo;

	bool operator==(const TextureData& other) const
	{
		return mTextureAttributeIndex == other.mTextureAttributeIndex && mWrapModeS == other.mWrapModeS && mWrapModeT == other.mWrapModeT
		    && mUnknown3 == other.mUnknown3 && mUnknown4 == other.mUnknown4 && mUnknown5 == other.mUnknown5 && mUnknown6 == other.mUnknown6
		    && mAnimationFactor == other.mAnimationFactor && mAnimLength == other.mAnimLength && nearly_equal(mAnimSpeed, other.mAnimSpeed)
		    && mScale == other.mScale && nearly_equal(mRotation, other.mRotation) && mPivot == other.mPivot && mPosition == other.mPosition
		    && mScaleInfo == other.mScaleInfo && mRotationInfo == other.mRotationInfo && mTranslationInfo == other.mTranslationInfo;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream& os, TextureData const& t);
};

struct TextureInfo {
	u32 mUseScale = 0; // Is treated as a boolean
	Vector3f mScale;
	std::vector<TexGenData> mTextureGenData;
	std::vector<TextureData> mTextureData;

	bool operator==(const TextureInfo& other) const
	{
		return mUseScale == other.mUseScale && mScale == other.mScale && mTextureGenData == other.mTextureGenData
		    && mTextureData == other.mTextureData;
	}

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
	u32 mFlags        = 0;
	s32 mTextureIndex = 0;
	u32 mTevGroupId   = 0;
	PolygonColourInfo mColourInfo;
	LightingInfo mLightingInfo;
	PeInfo mPeInfo;
	TextureInfo mTexInfo;

	bool operator==(const Material& other) const
	{
		return mFlags == other.mFlags && mTextureIndex == other.mTextureIndex && mTevGroupId == other.mTevGroupId
		    && mColourInfo == other.mColourInfo && mLightingInfo == other.mLightingInfo && mPeInfo == other.mPeInfo
		    && mTexInfo == other.mTexInfo;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, Material const&);
};

struct PVWAnimInfo_3_S10 {
	s32 mKeyframeCount = 0;
	KeyInfoS10 mKeyframeA;
	KeyInfoS10 mKeyframeB;
	KeyInfoS10 mKeyframeC;

	bool operator==(const PVWAnimInfo_3_S10& other) const
	{
		return mKeyframeCount == other.mKeyframeCount && mKeyframeA == other.mKeyframeA && mKeyframeB == other.mKeyframeB
		    && mKeyframeC == other.mKeyframeC;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, PVWAnimInfo_3_S10 const&);
};

struct PVWAnimInfo_1_S10 {
	s32 mKeyframeCount = 0;
	KeyInfoS10 mKeyframeInfo;

	bool operator==(const PVWAnimInfo_1_S10& other) const
	{
		return mKeyframeCount == other.mKeyframeCount && mKeyframeInfo == other.mKeyframeInfo;
	}

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

	bool operator==(const TEVColReg& other) const
	{
		return mColour == other.mColour && mAnimLength == other.mAnimLength && nearly_equal(mAnimSpeed, other.mAnimSpeed)
		    && mColorAnimInfo == other.mColorAnimInfo && mAlphaAnimInfo == other.mAlphaAnimInfo;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, TEVColReg const&);
};

struct PVWCombiner {
	u8 mInputABCD[4] = { 0 };
	u8 mOp;
	u8 mBias;
	u8 mScale;
	u8 mClamp;
	u8 mOutReg;
	u8 _unused[3] = { 0 };

	bool operator==(const PVWCombiner& other) const
	{
		for (int i = 0; i < 4; ++i) {
			if (mInputABCD[i] != other.mInputABCD[i])
				return false;
		}
		return mOp == other.mOp && mBias == other.mBias && mScale == other.mScale && mClamp == other.mClamp && mOutReg == other.mOutReg;
	}

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

	bool operator==(const TEVStage& other) const
	{
		return mUnknown == other.mUnknown && mTexCoordID == other.mTexCoordID && mTexMapID == other.mTexMapID
		    && mGXChannelID == other.mGXChannelID && mKColorSel == other.mKColorSel && mKAlphaSel == other.mKAlphaSel
		    && mTevColorCombiner == other.mTevColorCombiner && mTevAlphaCombiner == other.mTevAlphaCombiner;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer) const;
	friend std::ostream& operator<<(std::ostream&, TEVStage const&);
};

struct TEVInfo {
	TEVColReg mTevColourRegA;
	TEVColReg mTevColourRegB;
	TEVColReg mTevColourRegC;

	ColourU8 mKonstColourA;
	ColourU8 mKonstColourB;
	ColourU8 mKonstColourC;
	ColourU8 mKonstColourD;

	std::vector<TEVStage> mTevStages;

	bool operator==(const TEVInfo& other) const
	{
		return mTevColourRegA == other.mTevColourRegA && mTevColourRegB == other.mTevColourRegB && mTevColourRegC == other.mTevColourRegC
		    && mKonstColourA == other.mKonstColourA && mKonstColourB == other.mKonstColourB && mKonstColourC == other.mKonstColourC
		    && mKonstColourD == other.mKonstColourD && mTevStages == other.mTevStages;
	}

	void read(util::fstream_reader& reader);
	void write(util::fstream_writer& writer);
	friend std::ostream& operator<<(std::ostream&, TEVInfo const&);
};
} // namespace mat

struct MaterialContainer {
	std::vector<mat::Material> mMaterials;
	std::vector<mat::TEVInfo> mTevEnvironmentInfo;
};

inline std::string MaterialFlagsToString(u32 flags)
{
	using mat::MaterialFlags;

	static const std::pair<u32, const char*> flagMap[] = {
		{ static_cast<u32>(MaterialFlags::IsEnabled), "IsEnabled" },
		{ static_cast<u32>(MaterialFlags::Opaque), "Opaque" },
		{ static_cast<u32>(MaterialFlags::AlphaClip), "AlphaClip" },
		{ static_cast<u32>(MaterialFlags::TransparentBlend), "TransparentBlend" },
		{ static_cast<u32>(MaterialFlags::InvertSpecialBlend), "InvertSpecialBlend" },
		{ static_cast<u32>(MaterialFlags::Hidden), "Hidden" },
	};

	std::string result;
	bool first = true;

	for (const auto& [flag, name] : flagMap) {
		if ((flags & flag) != 0) {
			if (!first)
				result += ", ";
			result += name;
			first = false;
		}
	}

	return result;
}

inline std::string LightingInfoFlagsToString(u32 flags)
{
	using mat::LightingInfoFlags;

	static const std::pair<u32, const char*> flagMap[] = {
		{ static_cast<u32>(LightingInfoFlags::EnableColor0), "EnableColor0" },
		{ static_cast<u32>(LightingInfoFlags::EnableSpecular), "EnableSpecular" },
		{ static_cast<u32>(LightingInfoFlags::EnableAlpha0), "EnableAlpha0" },
		{ static_cast<u32>(LightingInfoFlags::Unknown8), "Unknown8" },
		{ static_cast<u32>(LightingInfoFlags::Unknown10), "Unknown10" },
		{ static_cast<u32>(LightingInfoFlags::Unknown20), "Unknown20" },
		{ static_cast<u32>(LightingInfoFlags::Unknown40), "Unknown40" },
		{ static_cast<u32>(LightingInfoFlags::Unknown80), "Unknown80" },
		{ static_cast<u32>(LightingInfoFlags::Unknown100), "Unknown100" },
		{ static_cast<u32>(LightingInfoFlags::AmbSrcColor0Vtx), "AmbSrcColor0Vtx" },
		{ static_cast<u32>(LightingInfoFlags::AmbSrcAlpha0Vtx), "AmbSrcAlpha0Vtx" },
		{ static_cast<u32>(LightingInfoFlags::MatSrcColor0Vtx), "MatSrcColor0Vtx" },
		{ static_cast<u32>(LightingInfoFlags::MatSrcAlpha0Vtx), "MatSrcAlpha0Vtx" },
	};

	std::string result;
	bool first = true;

	for (const auto& [flag, name] : flagMap) {
		if ((flags & flag) != 0) {
			if (!first)
				result += ", ";
			result += name;
			first = false;
		}
	}

	return result;
}

#endif