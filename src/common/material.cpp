#include "material.hpp"
#include <iostream>
#include <string>

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

//================================================================================
// TEV (TEXTURE ENVIRONMENT) STRUCTURES
//================================================================================

void PVWCombiner::read(util::fstream_reader& reader)
{
	for (unsigned char& i : mInputABCD) {
		i = reader.readU8();
	}
	mOp     = reader.readU8();
	mBias   = reader.readU8();
	mScale  = reader.readU8();
	mClamp  = reader.readU8();
	mOutReg = reader.readU8();
	for (unsigned char& i : _unused) {
		i = reader.readU8();
	}
}

void PVWCombiner::write(util::fstream_writer& writer) const
{
	for (unsigned char i : mInputABCD) {
		writer.writeU8(i);
	}
	writer.writeU8(mOp);
	writer.writeU8(mBias);
	writer.writeU8(mScale);
	writer.writeU8(mClamp);
	writer.writeU8(mOutReg);
	for (unsigned char i : _unused) {
		writer.writeU8(i);
	}
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

#pragma region Material

void Material::read(util::fstream_reader& reader)
{
	mFlags        = reader.readU32();
	mTextureIndex = reader.readS32();
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
	writer.writeS32(mTextureIndex);
	mColourInfo.mDiffuseColour.write(writer);

	if (mFlags & static_cast<u32>(MaterialFlags::IsEnabled)) {
		writer.writeU32(mTevGroupId);
		mColourInfo.write(writer);
		mLightingInfo.write(writer);
		mPeInfo.write(writer);
		mTexInfo.write(writer);
	}
}

#pragma endregion

} // namespace mat
