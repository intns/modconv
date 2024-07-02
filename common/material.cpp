#include <common/material.hpp>
#include <iostream>
#include <string>

#define PrintIndent(os, tabCnt, item)                            \
	for (std::size_t idntIdx = 0; idntIdx < tabCnt; idntIdx++) { \
		os << '\t';                                              \
	}                                                            \
	os << item << std::endl;

template <typename T>
static inline void PrintList(std::ostream& os, T& vector, const std::string& size_msg, const std::string& itemIdx_msg,
                             const u32 itemTabCount = 1)
{
	// EXAMPLE:
	// TEXGEN_SIZE 2
	PrintIndent(os, itemTabCount - 1, size_msg << " " << vector.size());

	std::size_t index = 0;
	for (auto& item : vector) {
		PrintIndent(os, itemTabCount, itemIdx_msg << " " << index++);
		PrintIndent(os, itemTabCount, item);
	}
}

namespace mat {
void KeyInfoU8::read(util::fstream_reader& reader)
{
	mAnimationFrame = reader.readU8();
	reader.readU8();
	reader.readU16();

	mStartValue = reader.readF32();
	mEndValue   = reader.readF32();
}

void KeyInfoU8::write(util::fstream_writer& writer)
{
	writer.writeU8(mAnimationFrame);
	writer.writeU8(0);
	writer.writeU8(0);
	writer.writeU8(0);

	writer.writeF32(mStartValue);
	writer.writeF32(mEndValue);
}

std::ostream& operator<<(std::ostream& os, KeyInfoU8 const& k)
{
	os << "FRAME[" << (u32)k.mAnimationFrame << "] " << k.mStartValue << " " << k.mEndValue;
	return os;
}

void KeyInfoF32::read(util::fstream_reader& reader)
{
	mKeyDataA = reader.readF32();
	mKeyDataB = reader.readF32();
	mKeyDataC = reader.readF32();
}

void KeyInfoF32::write(util::fstream_writer& writer)
{
	writer.writeF32(mKeyDataA);
	writer.writeF32(mKeyDataB);
	writer.writeF32(mKeyDataC);
}

std::ostream& operator<<(std::ostream& os, KeyInfoF32 const& k)
{
	os << k.mKeyDataA << " " << k.mKeyDataB << " " << k.mKeyDataC;
	return os;
}

void KeyInfoS10::read(util::fstream_reader& reader)
{
	mAnimationFrame = reader.readS16();
	reader.readS16();
	mStartValue = reader.readF32();
	mEndValue   = reader.readF32();
}

void KeyInfoS10::write(util::fstream_writer& writer)
{
	writer.writeS16(mAnimationFrame);
	writer.writeS16(0);
	writer.writeF32(mStartValue);
	writer.writeF32(mEndValue);
}

std::ostream& operator<<(std::ostream& os, KeyInfoS10 const& k)
{
	os << "FRAME[" << k.mAnimationFrame << "] " << k.mStartValue << " " << k.mEndValue;
	return os;
}

void ColourAnimInfo::read(util::fstream_reader& reader)
{
	mIndex = reader.readS32();
	mKeyDataR.read(reader);
	mKeyDataG.read(reader);
	mKeyDataB.read(reader);
}

void ColourAnimInfo::write(util::fstream_writer& writer)
{
	writer.writeS32(mIndex);
	mKeyDataR.write(writer);
	mKeyDataG.write(writer);
	mKeyDataB.write(writer);
}

std::ostream& operator<<(std::ostream& os, ColourAnimInfo const& p)
{
	os << "\t\tCOLOUR_ANIM" << std::endl;
	os << "\t\t\tINDEX " << p.mIndex << std::endl;
	os << "\t\t\tRED_KEYFRAME " << p.mKeyDataR << std::endl;
	os << "\t\t\tGREEN_KEYFRAME " << p.mKeyDataG << std::endl;
	os << "\t\t\tBLUE_KEYFRAME " << p.mKeyDataB << std::endl;
	return os;
}

void AlphaAnimInfo::read(util::fstream_reader& reader)
{
	mIndex = reader.readS32();
	mKeyData.read(reader);
}

void AlphaAnimInfo::write(util::fstream_writer& writer)
{
	writer.writeS32(mIndex);
	mKeyData.write(writer);
}

std::ostream& operator<<(std::ostream& os, AlphaAnimInfo const& p)
{
	os << "\t\tALPHA_ANIM" << std::endl;
	os << "\t\t\INDEX " << p.mIndex << std::endl;
	os << "\t\t\tALPHA_KEYFRAME " << p.mKeyData << std::endl;
	return os;
}

void PolygonColourInfo::read(util::fstream_reader& reader)
{
	mDiffuseColour.read(reader);
	mAnimLength = reader.readS32();
	mAnimSpeed  = reader.readF32();

	mColourAnimInfo.resize(reader.readU32());
	for (mat::ColourAnimInfo& unk : mColourAnimInfo) {
		unk.read(reader);
	}

	mAlphaAnimInfo.resize(reader.readU32());
	for (mat::AlphaAnimInfo& unk : mAlphaAnimInfo) {
		unk.read(reader);
	}
}

void PolygonColourInfo::write(util::fstream_writer& writer)
{
	mDiffuseColour.write(writer);
	writer.writeS32(mAnimLength);
	writer.writeF32(mAnimSpeed);

	writer.writeU32(mColourAnimInfo.size());
	for (mat::ColourAnimInfo& unk : mColourAnimInfo) {
		unk.write(writer);
	}

	writer.writeU32(mAlphaAnimInfo.size());
	for (mat::AlphaAnimInfo& unk : mAlphaAnimInfo) {
		unk.write(writer);
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

void LightingInfo::write(util::fstream_writer& writer)
{
	writer.writeU32(mFlags);
	writer.writeF32(mUnknown);
}

std::string LightingInfoFlagsToString(u32 flags)
{
	std::vector<std::string> flagNames;

	if (flags & static_cast<u32>(LightingInfoFlags::LightEnabled)) {
		flagNames.push_back("LightEnabled");
	}

	if (flags & static_cast<u32>(LightingInfoFlags::SpecularEnabled)) {
		flagNames.push_back("SpecularEnabled");
	}

	if (flags & static_cast<u32>(LightingInfoFlags::AlphaEnabled)) {
		flagNames.push_back("AlphaEnabled");
	}

	std::string result;
	for (size_t i = 0; i < flagNames.size(); ++i) {
		if (i > 0) {
			result += ", ";
		}
		result += flagNames[i];
	}

	return result;
}

std::ostream& operator<<(std::ostream& os, LightingInfo const& l)
{
	os << "\tLIGHTING_INFO" << std::endl;
	os << "\t\tFLAGS " << LightingInfoFlagsToString(l.mFlags) << std::endl;
	os << "\t\tUNK " << l.mUnknown << std::endl;
	return os;
}

void PeInfo::read(util::fstream_reader& reader)
{
	mFlags                      = reader.readS32();
	mAlphaCompareFunction.value = reader.readS32();
	mZModeFunction              = reader.readS32();
	mBlendMode.value            = reader.readS32();
}

void PeInfo::write(util::fstream_writer& writer)
{
	writer.writeS32(mFlags);
	writer.writeS32(mAlphaCompareFunction.value);
	writer.writeS32(mZModeFunction);
	writer.writeS32(mBlendMode.value);
}

std::ostream& operator<<(std::ostream& os, PeInfo const& p)
{
	os << "\tPE_INFO" << std::endl;
	os << "\t\tFLAGS " << p.mFlags << std::endl;
	os << "\t\tALPHA_COMPARE_FUNCTION " << std::endl;
	os << "\t\t\tCOMP0 " << p.mAlphaCompareFunction.bits.comp0 << std::endl;
	os << "\t\t\tREF0 " << p.mAlphaCompareFunction.bits.ref0 << std::endl;
	os << "\t\t\tOP " << p.mAlphaCompareFunction.bits.op << std::endl;
	os << "\t\t\tCOMP1 " << p.mAlphaCompareFunction.bits.comp1 << std::endl;
	os << "\t\t\tREF1 " << p.mAlphaCompareFunction.bits.ref1 << std::endl;
	os << "\t\tZ_MODE_FUNCTION " << p.mZModeFunction << std::endl;
	os << "\t\tBLEND_MODE " << std::endl;
	os << "\t\t\tTYPE " << p.mBlendMode.bits.mType << std::endl;
	os << "\t\t\tSRC_FACTOR " << p.mBlendMode.bits.mSrcFactor << std::endl;
	os << "\t\t\tDST_FACTOR " << p.mBlendMode.bits.mDstFactor << std::endl;
	os << "\t\t\tLOGIC_OP " << p.mBlendMode.bits.mLogicOp << std::endl;
	return os;
}

void TexGenData::read(util::fstream_reader& reader)
{
	mDestinationCoords = reader.readU8();
	mFunc              = reader.readU8();
	mSourceParam       = reader.readU8();
	mTexMtx            = reader.readU8();
}

void TexGenData::write(util::fstream_writer& writer)
{
	writer.writeU8(mDestinationCoords);
	writer.writeU8(mFunc);
	writer.writeU8(mSourceParam);
	writer.writeU8(mTexMtx);
}

std::string GetTexMtxName(u32 texMtxValue)
{
	if (texMtxValue <= 9) {
		return "GX_TEXMTX" + std::to_string(texMtxValue);
	}

	return texMtxValue == 10 ? "GX_IDENTITY" : "UNKNOWN_WTF_LOL(" + std::to_string(texMtxValue) + ")";
}

std::ostream& operator<<(std::ostream& os, TexGenData const& t)
{
	PrintIndent(os, 4, "DESTINATION_COORDS " << (u32)t.mDestinationCoords);
	PrintIndent(os, 4, "FUNC " << (u32)t.mFunc);
	PrintIndent(os, 4, "SOURCE_PARAM " << (u32)t.mSourceParam);
	PrintIndent(os, 4, "TEXTURE_MTX " << GetTexMtxName(t.mTexMtx));
	return os;
}

void TextureAnimData::read(util::fstream_reader& reader)
{
	mAnimationFrame = reader.readS32();
	mValueX.read(reader);
	mValueY.read(reader);
	mValueZ.read(reader);
}

void TextureAnimData::write(util::fstream_writer& writer)
{
	writer.writeS32(mAnimationFrame);
	mValueX.write(writer);
	mValueY.write(writer);
	mValueZ.write(writer);
}

std::ostream& operator<<(std::ostream& os, TextureAnimData const& t)
{
	PrintIndent(os, 1, "ANIMATION_FRAME " << t.mAnimationFrame);
	PrintIndent(os, 7, "VALUE_X " << t.mValueX);
	PrintIndent(os, 7, "VALUE_Y " << t.mValueY);
	PrintIndent(os, 7, "VALUE_Z " << t.mValueZ);
	return os;
}

void TextureData::read(util::fstream_reader& reader)
{
	m_texAttrIndex = reader.readS32();

	m_unknown2 = reader.readS16();
	m_unknown3 = reader.readS16();

	m_unknown4 = reader.readU8();
	m_unknown5 = reader.readU8();
	m_unknown6 = reader.readU8();
	m_unknown7 = reader.readU8();

	mTextureMtxId = reader.readU32();

	mAnimLength = reader.readS32();
	mAnimSpeed  = reader.readF32();

	mUv.read(reader);
	mRotation = reader.readF32();
	mPivot.read(reader);
	mPosition.read(reader);

	mPositionAnimData.resize(reader.readU32());
	for (mat::TextureAnimData& posData : mPositionAnimData) {
		posData.read(reader);
	}

	mRotationAnimData.resize(reader.readU32());
	for (mat::TextureAnimData& rotData : mRotationAnimData) {
		rotData.read(reader);
	}

	mScaleAnimData.resize(reader.readU32());
	for (mat::TextureAnimData& scaleData : mScaleAnimData) {
		scaleData.read(reader);
	}
}

void TextureData::write(util::fstream_writer& writer)
{
	writer.writeS32(m_texAttrIndex);

	writer.writeS16(m_unknown2);
	writer.writeS16(m_unknown3);

	writer.writeU8(m_unknown4);
	writer.writeU8(m_unknown5);
	writer.writeU8(m_unknown6);
	writer.writeU8(m_unknown7);

	writer.writeU32(mTextureMtxId);

	writer.writeS32(mAnimLength);
	writer.writeF32(mAnimSpeed);

	mUv.write(writer);
	writer.writeF32(mRotation);
	mPivot.write(writer);
	mPosition.write(writer);

	writer.writeU32(mPositionAnimData.size());
	for (TextureAnimData& posData : mPositionAnimData) {
		posData.write(writer);
	}

	writer.writeU32(mRotationAnimData.size());
	for (TextureAnimData& rotData : mRotationAnimData) {
		rotData.write(writer);
	}

	writer.writeU32(mScaleAnimData.size());
	for (TextureAnimData& scaleData : mScaleAnimData) {
		scaleData.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TextureData const& t)
{
	os << "\tTEX_ATTR_IDX " << t.m_texAttrIndex << std::endl;
	PrintIndent(os, 5, "UNK2 " << t.m_unknown2);
	PrintIndent(os, 5, "UNK3 " << t.m_unknown3);
	PrintIndent(os, 5, "UNK4 " << (u32)t.m_unknown4);
	PrintIndent(os, 5, "UNK5 " << (u32)t.m_unknown5);
	PrintIndent(os, 5, "UNK6 " << (u32)t.m_unknown6);
	PrintIndent(os, 5, "UNK7 " << (u32)t.m_unknown7);
	PrintIndent(os, 5, "TEXTURE_MATRIX_ID " << (u32)t.mTextureMtxId);
	PrintIndent(os, 5, "ANIM_LENGTH " << (u32)t.mAnimLength);
	PrintIndent(os, 5, "ANIM_SPEED " << t.mAnimSpeed);
	PrintIndent(os, 5, "UV " << t.mUv);
	PrintIndent(os, 5, "ROTATION " << t.mRotation);
	PrintIndent(os, 5, "PIVOT " << t.mPivot);
	PrintIndent(os, 5, "POSITION " << t.mPosition);

	PrintList(os, t.mPositionAnimData, "POS_ANIM_FRAME_COUNT", "ENTRY", 6);
	PrintList(os, t.mRotationAnimData, "ROT_ANIM_FRAME_COUNT", "ENTRY", 6);
	PrintList(os, t.mScaleAnimData, "SCALE_ANIM_FRAME_COUNT", "ENTRY", 6);

	return os;
}

void TextureInfo::read(util::fstream_reader& reader)
{
	m_unknown1 = reader.readS32();
	m_unknown2.read(reader);

	m_textureGenData.resize(reader.readU32());
	for (mat::TexGenData& genData : m_textureGenData) {
		genData.read(reader);
	}

	m_textureData.resize(reader.readU32());
	for (mat::TextureData& texData : m_textureData) {
		texData.read(reader);
	}
}

void TextureInfo::write(util::fstream_writer& writer)
{
	writer.writeS32(m_unknown1);
	m_unknown2.write(writer);

	writer.writeU32(m_textureGenData.size());
	for (mat::TexGenData& genData : m_textureGenData) {
		genData.write(writer);
	}

	writer.writeU32(m_textureData.size());
	for (mat::TextureData& texData : m_textureData) {
		texData.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TextureInfo const& ti)
{
	os << "\tTEXTURE_INFO" << std::endl;
	os << "\t\tUNK1 " << ti.m_unknown1 << std::endl;
	os << "\t\tUNK2 " << ti.m_unknown2 << std::endl;

	u32 idx = 0;
	os << "\t\tTEXTURE_GEN_DATA_COUNT " << ti.m_textureGenData.size() << std::endl;
	for (const TexGenData& txgen : ti.m_textureGenData) {
		os << "\t\t\t"
		   << "ENTRY" << idx << std::endl;
		os << txgen;
		idx++;
	}

	PrintList(os, ti.m_textureData, "TEXDATA_SIZE", "TEXDATA_IDX", 4);
	return os;
}

void Material::read(util::fstream_reader& reader)
{
	m_flags    = reader.readU32();
	m_unknown1 = reader.readU32();
	m_colourInfo.mDiffuseColour.read(reader);

	if (m_flags & static_cast<u32>(MaterialFlags::IsEnabled)) {
		mTevGroupId = reader.readU32();
		m_colourInfo.read(reader);
		m_lightingInfo.read(reader);
		m_peInfo.read(reader);
		m_texInfo.read(reader);
	}
}

void Material::write(util::fstream_writer& writer)
{
	writer.writeU32(m_flags);
	writer.writeU32(m_unknown1);
	m_colourInfo.mDiffuseColour.write(writer);

	if (m_flags & static_cast<u32>(MaterialFlags::IsEnabled)) {
		writer.writeS32(mTevGroupId);
		m_colourInfo.write(writer);
		m_lightingInfo.write(writer);
		m_peInfo.write(writer);
		m_texInfo.write(writer);
	}
}

std::string MaterialFlagsToString(u32 flags)
{
	std::vector<std::string> flagNames;

	if (flags & static_cast<u32>(MaterialFlags::IsEnabled)) {
		flagNames.push_back("IsEnabled");
	}
	if (flags & static_cast<u32>(MaterialFlags::Opaque)) {
		flagNames.push_back("Opaque");
	}
	if (flags & static_cast<u32>(MaterialFlags::AlphaClip)) {
		flagNames.push_back("AlphaClip");
	}
	if (flags & static_cast<u32>(MaterialFlags::TransparentBlend)) {
		flagNames.push_back("TransparentBlend");
	}
	if (flags & static_cast<u32>(MaterialFlags::InvertSpecialBlend)) {
		flagNames.push_back("InvertSpecialBlend");
	}
	if (flags & static_cast<u32>(MaterialFlags::Hidden)) {
		flagNames.push_back("Hidden");
	}

	std::string result;
	for (size_t i = 0; i < flagNames.size(); ++i) {
		if (i > 0) {
			result += ", ";
		}
		result += flagNames[i];
	}

	return result;
}

std::ostream& operator<<(std::ostream& os, Material const& m)
{
	os << "\tFLAGS " << MaterialFlagsToString(m.m_flags) << std::endl;
	os << "\tUNK1 " << std::hex << "0x" << m.m_unknown1 << std::dec << std::endl;
	os << "\tCOLOUR " << m.m_colourInfo.mDiffuseColour << std::endl;

	if (m.m_flags & static_cast<u32>(mat::MaterialFlags::IsEnabled)) {
		os << "\tTEV_GROUP_ID " << m.mTevGroupId << std::endl;
		os << m.m_colourInfo;
		os << m.m_lightingInfo;
		os << m.m_peInfo;
		os << m.m_texInfo;
	}
	os << std::endl;

	return os;
}

void PVWAnimInfo_3_S10::read(util::fstream_reader& reader)
{
	m_unknown1 = reader.readS32();
	m_unknown2.read(reader);
	m_unknown3.read(reader);
	m_unknown4.read(reader);
}

void PVWAnimInfo_3_S10::write(util::fstream_writer& writer)
{
	writer.writeS32(m_unknown1);
	m_unknown2.write(writer);
	m_unknown3.write(writer);
	m_unknown4.write(writer);
}

std::ostream& operator<<(std::ostream& os, PVWAnimInfo_3_S10 const& t)
{
	os << "\t\t\tUNK1 " << t.m_unknown1 << std::endl;
	os << "\t\t\tUNK2 " << t.m_unknown2 << std::endl;
	os << "\t\t\tUNK3 " << t.m_unknown3 << std::endl;
	os << "\t\t\tUNK4 " << t.m_unknown3 << std::endl;
	return os;
}

void PVWAnimInfo_1_S10::read(util::fstream_reader& reader)
{
	mKeyframeCount = reader.readS32();
	mKeyframeInfo.read(reader);
}

void PVWAnimInfo_1_S10::write(util::fstream_writer& writer)
{
	writer.writeS32(mKeyframeCount);
	mKeyframeInfo.write(writer);
}

std::ostream& operator<<(std::ostream& os, PVWAnimInfo_1_S10 const& t)
{
	os << "\t\t\tKEYFRAME_COUNT " << t.mKeyframeCount << std::endl;
	os << "\t\t\tKEYFRAME_INFO " << t.mKeyframeInfo << std::endl;
	return os;
}

void TEVColReg::read(util::fstream_reader& reader)
{
	mColour.read(reader);
	mAnimLength = reader.readS32();
	mAnimSpeed  = reader.readF32();

	mColorAnimInfo.resize(reader.readU32());
	for (mat::PVWAnimInfo_3_S10& unk : mColorAnimInfo) {
		unk.read(reader);
	}

	mAlphaAnimInfo.resize(reader.readU32());
	for (mat::PVWAnimInfo_1_S10& unk : mAlphaAnimInfo) {
		unk.read(reader);
	}
}

void TEVColReg::write(util::fstream_writer& writer)
{
	mColour.write(writer);
	writer.writeS32(mAnimLength);
	writer.writeF32(mAnimSpeed);

	writer.writeU32(mColorAnimInfo.size());
	for (mat::PVWAnimInfo_3_S10& unk : mColorAnimInfo) {
		unk.write(writer);
	}

	writer.writeU32(mAlphaAnimInfo.size());
	for (mat::PVWAnimInfo_1_S10& unk : mAlphaAnimInfo) {
		unk.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TEVColReg const& t)
{
	os << "\t\tCOLOUR " << t.mColour << std::endl;
	os << "\t\tANIM_LENGTH " << t.mAnimLength << std::endl;
	os << "\t\tANIM_SPEED " << t.mAnimSpeed << std::endl;

	os << "\t\tCOLOR_ANIM_COUNT " << t.mColorAnimInfo.size() << std::endl;
	for (const PVWAnimInfo_3_S10& elem : t.mColorAnimInfo) {
		os << elem;
	}

	os << "\t\tALPHA_ANIM_COUNT " << t.mAlphaAnimInfo.size() << std::endl;
	for (const PVWAnimInfo_1_S10& elem : t.mAlphaAnimInfo) {
		os << elem;
	}

	os << std::endl;

	return os;
}

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

void PVWCombiner::write(util::fstream_writer& writer)
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
	PrintIndent(os, 4, "INPUT_A " << (u32)i.mInputABCD[0]);
	PrintIndent(os, 4, "INPUT_B " << (u32)i.mInputABCD[1]);
	PrintIndent(os, 4, "INPUT_C " << (u32)i.mInputABCD[2]);
	PrintIndent(os, 4, "INPUT_D " << (u32)i.mInputABCD[3]);
	PrintIndent(os, 4, "OP " << (u32)i.mOp);
	PrintIndent(os, 4, "BIAS " << (u32)i.mBias);
	PrintIndent(os, 4, "SCALE " << (u32)i.mScale);
	PrintIndent(os, 4, "CLAMP " << (u32)i.mClamp);
	PrintIndent(os, 4, "OUT_REG " << (u32)i.mOutReg << std::endl);
	return os;
}

void TEVStage::read(util::fstream_reader& reader)
{
	for (u32 i = 0; i < 4; i++) {
		mTevOrders[i] = reader.readU8();
	}

	for (u32 i = 0; i < 2; i++) {
		mKSelections[i] = reader.readU8();
	}

	reader.readU16();
	mTevColorCombiner.read(reader);
	mTevAlphaCombiner.read(reader);
}

void TEVStage::write(util::fstream_writer& writer)
{
	for (u32 i = 0; i < 4; i++) {
		writer.writeU8(mTevOrders[i]);
	}

	for (u32 i = 0; i < 2; i++) {
		writer.writeU8(mKSelections[i]);
	}

	writer.writeU16(0);
	mTevColorCombiner.write(writer);
	mTevAlphaCombiner.write(writer);
}

std::ostream& operator<<(std::ostream& os, TEVStage const& i)
{
	// TEV_ORDERS
	os << "\t\t\tTEV_ORDER_A " << (u32)i.mTevOrders[0] << std::endl;
	os << "\t\t\tTEV_ORDER_B " << (u32)i.mTevOrders[1] << std::endl;
	os << "\t\t\tTEV_ORDER_C " << (u32)i.mTevOrders[2] << std::endl;
	os << "\t\t\tTEV_ORDER_D " << (u32)i.mTevOrders[3] << std::endl;

	// K_SELECTIONS
	os << "\t\t\tK_SELECTION_A " << (u32)i.mKSelections[0] << std::endl;
	os << "\t\t\tK_SELECTION_B " << (u32)i.mKSelections[1] << std::endl;

	// TEV_COLOR_COMBINER
	os << "\t\t\tTEV_COLOR_COMBINER " << std::endl << i.mTevColorCombiner;

	// TEV_ALPHA_COMBINER
	os << "\t\t\tTEV_ALPHA_COMBINER " << std::endl << i.mTevAlphaCombiner;

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

	writer.writeU32(mTevStages.size());
	for (mat::TEVStage& stage : mTevStages) {
		stage.write(writer);
	}
}

std::ostream& operator<<(std::ostream& os, TEVInfo const& i)
{
	os << "\tTEVCOLREG1" << std::endl << i.mTevColourRegA;
	os << "\tTEVCOLREG2" << std::endl << i.mTevColourRegB;
	os << "\tTEVCOLREG3" << std::endl << i.mTevColourRegC;
	os << "\tKONST_COL_A " << i.mKonstColourA << std::endl;
	os << "\tKONST_COL_B " << i.mKonstColourB << std::endl;
	os << "\tKONST_COL_C " << i.mKonstColourC << std::endl;
	os << "\tKONST_COL_D " << i.mKonstColourD << std::endl << std::endl;

	os << "\tTEV_STAGE_COUNT " << i.mTevStages.size() << std::endl;
	u32 tevIdx = 0;
	for (const TEVStage& elem : i.mTevStages) {
		os << "\t\tTEVSTAGE " << tevIdx++ << std::endl;
		os << elem;
	}

	os << std::endl;

	return os;
}

} // namespace mat
