#include <common/material.hpp>
#include <iostream>
#include <string>

//================================================================================
// TILITY HELPERS
//================================================================================

#define PrintIndent(os, tabCnt, item)                            \
	for (std::size_t idntIdx = 0; idntIdx < tabCnt; idntIdx++) { \
		os << '\t';                                              \
	}                                                            \
	os << item << std::endl;

template <typename T>
static inline void PrintList(std::ostream& os, const T& vector, const std::string& size_msg, const std::string& itemIdx_msg,
                             const u32 itemTabCount = 1)
{
	PrintIndent(os, itemTabCount - 1, size_msg << " " << vector.size());

	std::size_t index = 0;
	for (const auto& item : vector) {
		PrintIndent(os, itemTabCount, itemIdx_msg << " " << index++);
		PrintIndent(os, itemTabCount, item);
	}
}

namespace mat {

//================================================================================
// KEYFRAME PRIMITIVES
//================================================================================

void KeyInfoU8::read(util::fstream_reader& reader)
{
	mTime = reader.readU8();
	// Read 3 padding bytes to match the original source's four separate byte reads
	reader.readU8();
	reader.readU8();
	reader.readU8();

	mValue   = reader.readF32();
	mTangent = reader.readF32();
}

void KeyInfoU8::write(util::fstream_writer& writer) const
{
	writer.writeU8(mTime);
	writer.writeU8(0);
	writer.writeU8(0);
	writer.writeU8(0);
	writer.writeF32(mValue);
	writer.writeF32(mTangent);
}

std::ostream& operator<<(std::ostream& os, KeyInfoU8 const& k)
{
	os << "TIME[" << (u32)k.mTime << "] " << k.mValue << " " << k.mTangent;
	return os;
}

void KeyInfoF32::read(util::fstream_reader& reader)
{
	mTime    = reader.readF32();
	mValue   = reader.readF32();
	mTangent = reader.readF32();
}

void KeyInfoF32::write(util::fstream_writer& writer) const
{
	writer.writeF32(mTime);
	writer.writeF32(mValue);
	writer.writeF32(mTangent);
}

std::ostream& operator<<(std::ostream& os, KeyInfoF32 const& k)
{
	os << k.mTime << " " << k.mValue << " " << k.mTangent;
	return os;
}

void KeyInfoS10::read(util::fstream_reader& reader)
{
	mTime = reader.readS16();
	reader.readS16(); // Padding
	mValue   = reader.readF32();
	mTangent = reader.readF32();
}

void KeyInfoS10::write(util::fstream_writer& writer) const
{
	writer.writeS16(mTime);
	writer.writeS16(0);
	writer.writeF32(mValue);
	writer.writeF32(mTangent);
}

std::ostream& operator<<(std::ostream& os, KeyInfoS10 const& k)
{
	os << "TIME[" << k.mTime << "] " << k.mValue << " " << k.mTangent;
	return os;
}

//================================================================================
// ANIMATION INFO STRUCTURES
//================================================================================

void ColourAnimInfo::read(util::fstream_reader& reader)
{
	mIndex = reader.readS32();
	mKeyDataR.read(reader);
	mKeyDataG.read(reader);
	mKeyDataB.read(reader);
}

void ColourAnimInfo::write(util::fstream_writer& writer) const
{
	writer.writeS32(mIndex);
	mKeyDataR.write(writer);
	mKeyDataG.write(writer);
	mKeyDataB.write(writer);
}

std::ostream& operator<<(std::ostream& os, ColourAnimInfo const& p)
{
	PrintIndent(os, 2, "COLOUR_ANIM");
	PrintIndent(os, 3, "INDEX " << p.mIndex);
	PrintIndent(os, 3, "RED_KEYFRAME " << p.mKeyDataR);
	PrintIndent(os, 3, "GREEN_KEYFRAME " << p.mKeyDataG);
	PrintIndent(os, 3, "BLUE_KEYFRAME " << p.mKeyDataB);
	return os;
}

void AlphaAnimInfo::read(util::fstream_reader& reader)
{
	mIndex = reader.readS32();
	mKeyData.read(reader);
}

void AlphaAnimInfo::write(util::fstream_writer& writer) const
{
	writer.writeS32(mIndex);
	mKeyData.write(writer);
}

std::ostream& operator<<(std::ostream& os, AlphaAnimInfo const& p)
{
	PrintIndent(os, 2, "ALPHA_ANIM");
	PrintIndent(os, 3, "INDEX " << p.mIndex);
	PrintIndent(os, 3, "ALPHA_KEYFRAME " << p.mKeyData);
	return os;
}

void TextureAnimData::read(util::fstream_reader& reader)
{
	mAnimationFrame = reader.readS32();
	mValueX.read(reader);
	mValueY.read(reader);
	mValueZ.read(reader);
}

void TextureAnimData::write(util::fstream_writer& writer) const
{
	writer.writeS32(mAnimationFrame);
	mValueX.write(writer);
	mValueY.write(writer);
	mValueZ.write(writer);
}

std::ostream& operator<<(std::ostream& os, TextureAnimData const& t)
{
	PrintIndent(os, 6, "ANIMATION_FRAME " << t.mAnimationFrame);
	PrintIndent(os, 7, "VALUE_X " << t.mValueX);
	PrintIndent(os, 7, "VALUE_Y " << t.mValueY);
	PrintIndent(os, 7, "VALUE_Z " << t.mValueZ);
	return os;
}

void PVWAnimInfo_1_S10::read(util::fstream_reader& reader)
{
	mKeyframeCount = reader.readS32();
	mKeyframeInfo.read(reader);
}

void PVWAnimInfo_1_S10::write(util::fstream_writer& writer) const
{
	writer.writeS32(mKeyframeCount);
	mKeyframeInfo.write(writer);
}

std::ostream& operator<<(std::ostream& os, PVWAnimInfo_1_S10 const& t)
{
	PrintIndent(os, 3, "KEYFRAME_COUNT " << t.mKeyframeCount);
	PrintIndent(os, 3, "KEYFRAME_INFO " << t.mKeyframeInfo);
	return os;
}

void PVWAnimInfo_3_S10::read(util::fstream_reader& reader)
{
	mKeyframeCount = reader.readS32();
	mKeyframeA.read(reader);
	mKeyframeB.read(reader);
	mKeyframeC.read(reader);
}

void PVWAnimInfo_3_S10::write(util::fstream_writer& writer) const
{
	writer.writeS32(mKeyframeCount);
	mKeyframeA.write(writer);
	mKeyframeB.write(writer);
	mKeyframeC.write(writer);
}

std::ostream& operator<<(std::ostream& os, PVWAnimInfo_3_S10 const& t)
{
	PrintIndent(os, 3, "KEYFRAME_COUNT " << t.mKeyframeCount);
	PrintIndent(os, 3, "KEYFRAME_A " << t.mKeyframeA);
	PrintIndent(os, 3, "KEYFRAME_B " << t.mKeyframeB);
	PrintIndent(os, 3, "KEYFRAME_C " << t.mKeyframeC);
	return os;
}

//================================================================================
// MATERIAL COMPONENT STRUCTURES
//================================================================================

void PolygonColourInfo::read(util::fstream_reader& reader)
{
	mDiffuseColour.read(reader);
	mAnimLength = reader.readS32();
	mAnimSpeed  = reader.readF32();

	mColourAnimInfo.resize(reader.readU32());
	for (mat::ColourAnimInfo& info : mColourAnimInfo) {
		info.read(reader);
	}

	mAlphaAnimInfo.resize(reader.readU32());
	for (mat::AlphaAnimInfo& info : mAlphaAnimInfo) {
		info.read(reader);
	}
}

void PolygonColourInfo::write(util::fstream_writer& writer)
{
	mDiffuseColour.write(writer);
	writer.writeS32(mAnimLength);
	writer.writeF32(mAnimSpeed);

	writer.writeU32(static_cast<u32>(mColourAnimInfo.size()));
	for (mat::ColourAnimInfo& info : mColourAnimInfo) {
		info.write(writer);
	}

	writer.writeU32(static_cast<u32>(mAlphaAnimInfo.size()));
	for (mat::AlphaAnimInfo& info : mAlphaAnimInfo) {
		info.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, PolygonColourInfo const& p)
{
	PrintIndent(os, 1, "POLYGON_COLOUR_INFO");
	PrintIndent(os, 2, "DIFFUSE_COLOUR " << p.mDiffuseColour);
	PrintIndent(os, 2, "ANIM_LENGTH " << p.mAnimLength);
	PrintIndent(os, 2, "ANIM_SPEED " << p.mAnimSpeed);
	PrintList(os, p.mColourAnimInfo, "COLOUR_ANIM_COUNT", "COLOUR_ANIM_INDEX", 3);
	PrintList(os, p.mAlphaAnimInfo, "ALPHA_ANIM_COUNT", "ALPHA_ANIM_INDEX", 3);

	return os;
}

void LightingInfo::read(util::fstream_reader& reader)
{
	mFlags   = reader.readU32();
	mUnknown = reader.readF32();
}

void LightingInfo::write(util::fstream_writer& writer) const
{
	writer.writeU32(mFlags);
	writer.writeF32(mUnknown);
}

std::ostream& operator<<(std::ostream& os, LightingInfo const& l)
{
	PrintIndent(os, 1, "LIGHTING_INFO");
	PrintIndent(os, 2, "FLAGS " << LightingInfoFlagsToString(l.mFlags));
	PrintIndent(os, 2, "UNK " << l.mUnknown);
	return os;
}

void PeInfo::read(util::fstream_reader& reader)
{
	mFlags                      = reader.readS32();
	mAlphaCompareFunction.value = reader.readS32();
	mZModeFunction              = reader.readS32();
	mBlendMode.value            = reader.readS32();
}

void PeInfo::write(util::fstream_writer& writer) const
{
	writer.writeS32(mFlags);
	writer.writeS32(mAlphaCompareFunction.value);
	writer.writeS32(mZModeFunction);
	writer.writeS32(mBlendMode.value);
}

std::ostream& operator<<(std::ostream& os, PeInfo const& p)
{
	PrintIndent(os, 1, "PE_INFO");
	PrintIndent(os, 2, "FLAGS " << p.mFlags);
	PrintIndent(os, 2, "ALPHA_COMPARE_FUNCTION");
	PrintIndent(os, 3, "COMP0 " << p.mAlphaCompareFunction.bits.comp0);
	PrintIndent(os, 3, "REF0 " << p.mAlphaCompareFunction.bits.ref0);
	PrintIndent(os, 3, "OP " << p.mAlphaCompareFunction.bits.op);
	PrintIndent(os, 3, "COMP1 " << p.mAlphaCompareFunction.bits.comp1);
	PrintIndent(os, 3, "REF1 " << p.mAlphaCompareFunction.bits.ref1);
	PrintIndent(os, 2, "Z_MODE_FUNCTION " << p.mZModeFunction);
	PrintIndent(os, 2, "BLEND_MODE");
	PrintIndent(os, 3, "TYPE " << p.mBlendMode.bits.mType);
	PrintIndent(os, 3, "SRC_FACTOR " << p.mBlendMode.bits.mSrcFactor);
	PrintIndent(os, 3, "DST_FACTOR " << p.mBlendMode.bits.mDstFactor);
	PrintIndent(os, 3, "LOGIC_OP " << p.mBlendMode.bits.mLogicOp);
	return os;
}

//================================================================================
// TEXTURE STRUCTURES
//================================================================================

void TexGenData::read(util::fstream_reader& reader)
{
	mDestinationCoords = reader.readU8();
	mFunc              = reader.readU8();
	mSourceParam       = reader.readU8();
	mTexMtx            = reader.readU8();
}

void TexGenData::write(util::fstream_writer& writer) const
{
	writer.writeU8(mDestinationCoords);
	writer.writeU8(mFunc);
	writer.writeU8(mSourceParam);
	writer.writeU8(mTexMtx);
}

std::ostream& operator<<(std::ostream& os, TexGenData const& t)
{
	PrintIndent(os, 4, "DESTINATION_COORDS " << GXTexCoordIDToStringConverter(static_cast<GXTexCoordID>(t.mDestinationCoords)));
	PrintIndent(os, 4, "FUNC " << GXTexGenTypeToStringConverter(static_cast<GXTexGenType>(t.mFunc)));
	PrintIndent(os, 4, "SOURCE_PARAM " << GXTexGenSrcToStringConverter(static_cast<GXTexGenSrc>(t.mSourceParam)));
	PrintIndent(os, 4, "TEXTURE_MTX " << GXTexMtxToStringConverter(static_cast<GXTexMtx>(t.mTexMtx)));
	return os;
}

void TextureData::read(util::fstream_reader& reader)
{
	mTextureAttributeIndex = reader.readS32();
	mWrapModeS             = reader.readS16();
	mWrapModeT             = reader.readS16();
	mUnknown3              = reader.readU8();
	mUnknown4              = reader.readU8();
	mUnknown5              = reader.readU8();
	mUnknown6              = reader.readU8();
	mAnimationFactor       = reader.readS32();
	mAnimLength            = reader.readS32();
	mAnimSpeed             = reader.readF32();
	mScale.read(reader);
	mRotation = reader.readF32();
	mPivot.read(reader);
	mPosition.read(reader);

	mScaleInfo.resize(reader.readU32());
	for (mat::TextureAnimData& info : mScaleInfo) {
		info.read(reader);
	}

	mRotationInfo.resize(reader.readU32());
	for (mat::TextureAnimData& info : mRotationInfo) {
		info.read(reader);
	}

	mTranslationInfo.resize(reader.readU32());
	for (mat::TextureAnimData& info : mTranslationInfo) {
		info.read(reader);
	}
}

void TextureData::write(util::fstream_writer& writer)
{
	writer.writeS32(mTextureAttributeIndex);
	writer.writeS16(mWrapModeS);
	writer.writeS16(mWrapModeT);
	writer.writeU8(mUnknown3);
	writer.writeU8(mUnknown4);
	writer.writeU8(mUnknown5);
	writer.writeU8(mUnknown6);
	writer.writeS32(mAnimationFactor);
	writer.writeS32(mAnimLength);
	writer.writeF32(mAnimSpeed);
	mScale.write(writer);
	writer.writeF32(mRotation);
	mPivot.write(writer);
	mPosition.write(writer);

	writer.writeU32(static_cast<u32>(mScaleInfo.size()));
	for (TextureAnimData& info : mScaleInfo) {
		info.write(writer);
	}

	writer.writeU32(static_cast<u32>(mRotationInfo.size()));
	for (TextureAnimData& info : mRotationInfo) {
		info.write(writer);
	}

	writer.writeU32(static_cast<u32>(mTranslationInfo.size()));
	for (TextureAnimData& info : mTranslationInfo) {
		info.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TextureData const& t)
{
	PrintIndent(os, 5, "TEX_ATTR_IDX " << t.mTextureAttributeIndex);
	PrintIndent(os, 5, "WRAP_MODE_S " << GXTexWrapModeToStringConverter(static_cast<GXTexWrapMode>(t.mWrapModeS)));
	PrintIndent(os, 5, "WRAP_MODE_T " << GXTexWrapModeToStringConverter(static_cast<GXTexWrapMode>(t.mWrapModeT)));
	PrintIndent(os, 5, "UNK4 " << (u32)t.mUnknown3);
	PrintIndent(os, 5, "UNK5 " << (u32)t.mUnknown4);
	PrintIndent(os, 5, "UNK6 " << (u32)t.mUnknown5);
	PrintIndent(os, 5, "UNK7 " << (u32)t.mUnknown6);
	PrintIndent(os, 5, "ANIMATION_FACTOR " << t.mAnimationFactor);
	PrintIndent(os, 5, "ANIM_LENGTH " << (u32)t.mAnimLength);
	PrintIndent(os, 5, "ANIM_SPEED " << t.mAnimSpeed);
	PrintIndent(os, 5, "SCALE " << t.mScale);
	PrintIndent(os, 5, "ROTATION " << t.mRotation);
	PrintIndent(os, 5, "PIVOT " << t.mPivot);
	PrintIndent(os, 5, "POSITION " << t.mPosition);

	PrintList(os, t.mScaleInfo, "SCALE_FRAME_COUNT", "ENTRY", 6);
	PrintList(os, t.mRotationInfo, "ROTATION_FRAME_COUNT", "ENTRY", 6);
	PrintList(os, t.mTranslationInfo, "TRANSLATION_FRAME_COUNT", "ENTRY", 6);

	return os;
}

void TextureInfo::read(util::fstream_reader& reader)
{
	mUseScale = reader.readS32();
	mScale.read(reader);

	mTextureGenData.resize(reader.readU32());
	for (mat::TexGenData& info : mTextureGenData) {
		info.read(reader);
	}

	mTextureData.resize(reader.readU32());
	for (mat::TextureData& info : mTextureData) {
		info.read(reader);
	}
}

void TextureInfo::write(util::fstream_writer& writer)
{
	writer.writeS32(mUseScale);
	mScale.write(writer);

	writer.writeU32(static_cast<u32>(mTextureGenData.size()));
	for (mat::TexGenData& info : mTextureGenData) {
		info.write(writer);
	}

	writer.writeU32(static_cast<u32>(mTextureData.size()));
	for (mat::TextureData& info : mTextureData) {
		info.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TextureInfo const& ti)
{
	PrintIndent(os, 1, "TEXTURE_INFO");
	PrintIndent(os, 2, "USE_SCALE " << ti.mUseScale);
	PrintIndent(os, 2, "SCALE " << ti.mScale);
	PrintList(os, ti.mTextureGenData, "TEXTURE_GEN_DATA_COUNT", "ENTRY", 3);
	PrintList(os, ti.mTextureData, "TEXDATA_COUNT", "TEXDATA_ENTRY", 3);
	return os;
}

//================================================================================
// TEV (TEXTURE ENVIRONMENT) STRUCTURES
//================================================================================

void PVWCombiner::read(util::fstream_reader& reader)
{
	for (int i = 0; i < 4; ++i) {
		mInputABCD[i] = reader.readU8();
	}
	mOp     = reader.readU8();
	mBias   = reader.readU8();
	mScale  = reader.readU8();
	mClamp  = reader.readU8();
	mOutReg = reader.readU8();
	for (int i = 0; i < 3; ++i) {
		_unused[i] = reader.readU8();
	}
}

void PVWCombiner::write(util::fstream_writer& writer) const
{
	for (int i = 0; i < 4; ++i) {
		writer.writeU8(mInputABCD[i]);
	}
	writer.writeU8(mOp);
	writer.writeU8(mBias);
	writer.writeU8(mScale);
	writer.writeU8(mClamp);
	writer.writeU8(mOutReg);
	for (int i = 0; i < 3; ++i) {
		writer.writeU8(_unused[i]);
	}
}

std::ostream& operator<<(std::ostream& os, PVWCombiner const& i)
{
	PrintIndent(os, 4, "INPUT_A " << GXTevColorArgToStringConverter((GXTevColorArg)i.mInputABCD[0]));
	PrintIndent(os, 4, "INPUT_B " << GXTevColorArgToStringConverter((GXTevColorArg)i.mInputABCD[1]));
	PrintIndent(os, 4, "INPUT_C " << GXTevColorArgToStringConverter((GXTevColorArg)i.mInputABCD[2]));
	PrintIndent(os, 4, "INPUT_D " << GXTevColorArgToStringConverter((GXTevColorArg)i.mInputABCD[3]));
	PrintIndent(os, 4, "OP " << (u32)i.mOp);
	PrintIndent(os, 4, "BIAS " << (u32)i.mBias);
	PrintIndent(os, 4, "SCALE " << (u32)i.mScale);
	PrintIndent(os, 4, "CLAMP " << (u32)i.mClamp);
	PrintIndent(os, 4, "OUT_REG " << (u32)i.mOutReg);
	return os;
}

void TEVStage::read(util::fstream_reader& reader)
{
	mUnknown     = reader.readU8();
	mTexCoordID  = reader.readU8();
	mTexMapID    = reader.readU8();
	mGXChannelID = reader.readU8();
	mKColorSel   = reader.readU8();
	mKAlphaSel   = reader.readU8();

	reader.readU16(); // Padding
	mTevColorCombiner.read(reader);
	mTevAlphaCombiner.read(reader);
}

void TEVStage::write(util::fstream_writer& writer) const
{
	writer.writeU8(mUnknown);
	writer.writeU8(mTexCoordID);
	writer.writeU8(mTexMapID);
	writer.writeU8(mGXChannelID);
	writer.writeU8(mKColorSel);
	writer.writeU8(mKAlphaSel);
	writer.writeU16(0);
	mTevColorCombiner.write(writer);
	mTevAlphaCombiner.write(writer);
}

std::ostream& operator<<(std::ostream& os, TEVStage const& i)
{
	PrintIndent(os, 3, "UNKNOWN " << (u32)i.mUnknown);
	PrintIndent(os, 3, "TEX_COORD_ID " << GXTexCoordIDToStringConverter(static_cast<GXTexCoordID>(i.mTexCoordID)));
	PrintIndent(os, 3, "TEX_MAP_ID " << GXTexMapIDToStringConverter(static_cast<GXTexMapID>(i.mTexMapID)));
	PrintIndent(os, 3, "GX_CHANNEL_ID " << GXChannelIDToStringConverter(static_cast<GXChannelID>(i.mGXChannelID)));
	PrintIndent(os, 3, "K_COLOR_SEL " << GXTevKColorSelToStringConverter(static_cast<GXTevKColorSel>(i.mKColorSel)));
	PrintIndent(os, 3, "K_ALPHA_SEL " << GXTevKAlphaSelToStringConverter(static_cast<GXTevKAlphaSel>(i.mKAlphaSel)));
	PrintIndent(os, 3, "TEV_COLOR_COMBINER");
	os << i.mTevColorCombiner;
	PrintIndent(os, 3, "TEV_ALPHA_COMBINER");
	os << i.mTevAlphaCombiner;
	return os;
}

void TEVColReg::read(util::fstream_reader& reader)
{
	mColour.read(reader);
	mAnimLength = reader.readS32();
	mAnimSpeed  = reader.readF32();

	mColorAnimInfo.resize(reader.readU32());
	for (mat::PVWAnimInfo_3_S10& info : mColorAnimInfo) {
		info.read(reader);
	}

	mAlphaAnimInfo.resize(reader.readU32());
	for (mat::PVWAnimInfo_1_S10& info : mAlphaAnimInfo) {
		info.read(reader);
	}
}

void TEVColReg::write(util::fstream_writer& writer)
{
	mColour.write(writer);
	writer.writeS32(mAnimLength);
	writer.writeF32(mAnimSpeed);

	writer.writeU32(static_cast<u32>(mColorAnimInfo.size()));
	for (mat::PVWAnimInfo_3_S10& info : mColorAnimInfo) {
		info.write(writer);
	}

	writer.writeU32(static_cast<u32>(mAlphaAnimInfo.size()));
	for (mat::PVWAnimInfo_1_S10& info : mAlphaAnimInfo) {
		info.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TEVColReg const& t)
{
	PrintIndent(os, 2, "COLOUR " << t.mColour);
	PrintIndent(os, 2, "ANIM_LENGTH " << t.mAnimLength);
	PrintIndent(os, 2, "ANIM_SPEED " << t.mAnimSpeed);
	PrintList(os, t.mColorAnimInfo, "COLOR_ANIM_COUNT", "COLOR_ANIM_ENTRY", 3);
	PrintList(os, t.mAlphaAnimInfo, "ALPHA_ANIM_COUNT", "ALPHA_ANIM_ENTRY", 3);
	return os;
}

void TEVInfo::read(util::fstream_reader& reader)
{
	mTevColourRegA.read(reader);
	mTevColourRegB.read(reader);
	mTevColourRegC.read(reader);

	mKonstColourA.read(reader);
	mKonstColourB.read(reader);
	mKonstColourC.read(reader);
	mKonstColourD.read(reader);

	mTevStages.resize(reader.readU32());
	for (mat::TEVStage& stage : mTevStages) {
		stage.read(reader);
	}
}

void TEVInfo::write(util::fstream_writer& writer)
{
	mTevColourRegA.write(writer);
	mTevColourRegB.write(writer);
	mTevColourRegC.write(writer);

	mKonstColourA.write(writer);
	mKonstColourB.write(writer);
	mKonstColourC.write(writer);
	mKonstColourD.write(writer);

	writer.writeU32(static_cast<u32>(mTevStages.size()));
	for (mat::TEVStage& stage : mTevStages) {
		stage.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TEVInfo const& i)
{
	PrintIndent(os, 1, "TEVCOLREG_A");
	os << i.mTevColourRegA;
	PrintIndent(os, 1, "TEVCOLREG_B");
	os << i.mTevColourRegB;
	PrintIndent(os, 1, "TEVCOLREG_C");
	os << i.mTevColourRegC;
	PrintIndent(os, 1, "KONST_COL_A " << i.mKonstColourA);
	PrintIndent(os, 1, "KONST_COL_B " << i.mKonstColourB);
	PrintIndent(os, 1, "KONST_COL_C " << i.mKonstColourC);
	PrintIndent(os, 1, "KONST_COL_D " << i.mKonstColourD);

	PrintList(os, i.mTevStages, "TEVSTAGE_COUNT", "TEVSTAGE_ENTRY", 2);

	return os;
}

#pragma region Material

void Material::read(util::fstream_reader& reader)
{
	mFlags        = reader.readU32();
	mTextureIndex = reader.readU32();
	mColourInfo.mDiffuseColour.read(reader);

	if (mFlags & static_cast<u32>(MaterialFlags::IsEnabled)) {
		mTevGroupId = reader.readU32();
		mColourInfo.read(reader);
		mLightingInfo.read(reader);
		mPeInfo.read(reader);
		mTexInfo.read(reader);
	}
}

void Material::write(util::fstream_writer& writer)
{
	writer.writeU32(mFlags);
	writer.writeU32(mTextureIndex);
	mColourInfo.mDiffuseColour.write(writer);

	if (mFlags & static_cast<u32>(MaterialFlags::IsEnabled)) {
		writer.writeU32(mTevGroupId);
		mColourInfo.write(writer);
		mLightingInfo.write(writer);
		mPeInfo.write(writer);
		mTexInfo.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, Material const& m)
{
	os << "\tFLAGS " << MaterialFlagsToString(m.mFlags) << std::endl;
	os << "\tTEXTURE_INDEX " << m.mTextureIndex << std::endl;
	os << "\tCOLOUR " << m.mColourInfo.mDiffuseColour << std::endl;

	if (m.mFlags & static_cast<u32>(mat::MaterialFlags::IsEnabled)) {
		os << "\tTEV_GROUP_ID " << m.mTevGroupId << std::endl;
		os << m.mColourInfo;
		os << m.mLightingInfo;
		os << m.mPeInfo;
		os << m.mTexInfo;
	}
	os << std::endl;

	return os;
}

#pragma endregion

} // namespace mat
