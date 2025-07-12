#ifndef MATERIAL_WRITER_HPP
#define MATERIAL_WRITER_HPP

#pragma once

#include <common.hpp>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>

using namespace mat;
inline std::string trim(const std::string& s)
{
	auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); });
	auto end   = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c) { return std::isspace(c); }).base();
	return (start < end) ? std::string(start, end) : "";
}

template <typename T>
std::string FltToStr(T value)
{
	std::ostringstream oss;
	oss << std::setprecision(9) << std::defaultfloat << value;
	return oss.str();
}

class MaterialWriter {
public:
	std::ostream& m_out;
	int m_indentLevel = 0;

	void indent()
	{
		for (int i = 0; i < m_indentLevel; ++i) {
			m_out << '\t';
		}
	}

	void writeLine(const std::string& content)
	{
		indent();
		m_out << content << '\n';
	}

	template <typename T>
	void writeValue(const std::string& label, const T& value)
	{
		indent();
		m_out << label << " " << value << '\n';
	}

	void writeColour(const std::string& label, const ColourU16& color)
	{
		indent();
		m_out << label << " " << color << '\n';
	}

	void writeColour(const std::string& label, const ColourU8& color)
	{
		indent();
		m_out << label << " " << color << '\n';
	}

	void writeVec(const std::string& label, const Vector3f& vec)
	{
		indent();
		m_out << label << " " << vec << '\n';
	}

	void writeVec(const std::string& label, const Vector2f& vec)
	{
		indent();
		m_out << label << " " << vec << '\n';
	}

public:
	MaterialWriter(std::ostream& out)
	    : m_out(out)
	{
		m_out << std::fixed << std::setprecision(9) << std::fixed;
	}

	void beginSection(const std::string& name)
	{
		writeLine(name);
		m_indentLevel++;
	}

	void endSection()
	{
		if (m_indentLevel > 0)
			m_indentLevel--;
	}

	// Write individual components
	void writeKeyInfoU8(const std::string& label, const KeyInfoU8& key)
	{
		writeValue(label, "TIME[" + std::to_string((u32)key.mTime) + "] " + FltToStr(key.mValue) + " " + FltToStr(key.mTangent));
	}

	void writeKeyInfoF32(const std::string& label, const KeyInfoF32& key)
	{
		writeValue(label, FltToStr(key.mTime) + " " + FltToStr(key.mValue) + " " + FltToStr(key.mTangent));
	}

	void writeKeyInfoS10(const std::string& label, const KeyInfoS10& key)
	{
		writeValue(label, "TIME[" + std::to_string(key.mTime) + "] " + FltToStr(key.mValue) + " " + FltToStr(key.mTangent));
	}

	void writeColourAnimInfo(const ColourAnimInfo& info)
	{
		beginSection("COLOUR_ANIM");
		writeValue("INDEX", info.mIndex);
		writeKeyInfoU8("RED_KEYFRAME", info.mKeyDataR);
		writeKeyInfoU8("GREEN_KEYFRAME", info.mKeyDataG);
		writeKeyInfoU8("BLUE_KEYFRAME", info.mKeyDataB);
		endSection();
	}

	void writeAlphaAnimInfo(const AlphaAnimInfo& info)
	{
		beginSection("ALPHA_ANIM");
		writeValue("INDEX", info.mIndex);
		writeKeyInfoU8("ALPHA_KEYFRAME", info.mKeyData);
		endSection();
	}

	void writeTextureAnimData(const TextureAnimData& data)
	{
		writeValue("ANIMATION_FRAME", data.mAnimationFrame);
		m_indentLevel++;
		writeKeyInfoF32("VALUE_X", data.mValueX);
		writeKeyInfoF32("VALUE_Y", data.mValueY);
		writeKeyInfoF32("VALUE_Z", data.mValueZ);
		m_indentLevel--;
	}

	void writePVWAnimInfo_1_S10(const PVWAnimInfo_1_S10& info)
	{
		writeValue("KEYFRAME_COUNT", info.mKeyframeCount);
		writeKeyInfoS10("KEYFRAME_INFO", info.mKeyframeInfo);
	}

	void writePVWAnimInfo_3_S10(const PVWAnimInfo_3_S10& info)
	{
		writeValue("KEYFRAME_COUNT", info.mKeyframeCount);
		writeKeyInfoS10("KEYFRAME_A", info.mKeyframeA);
		writeKeyInfoS10("KEYFRAME_B", info.mKeyframeB);
		writeKeyInfoS10("KEYFRAME_C", info.mKeyframeC);
	}

	void writePolygonColourInfo(const PolygonColourInfo& info)
	{
		beginSection("POLYGON_COLOUR_INFO");
		writeColour("DIFFUSE_COLOUR", info.mDiffuseColour);
		writeValue("ANIM_LENGTH", info.mAnimLength);
		writeValue("ANIM_SPEED", info.mAnimSpeed);

		writeValue("COLOUR_ANIM_COUNT", info.mColourAnimInfo.size());
		for (size_t i = 0; i < info.mColourAnimInfo.size(); ++i) {
			writeValue("COLOUR_ANIM_INDEX", i);
			writeColourAnimInfo(info.mColourAnimInfo[i]);
		}

		writeValue("ALPHA_ANIM_COUNT", info.mAlphaAnimInfo.size());
		for (size_t i = 0; i < info.mAlphaAnimInfo.size(); ++i) {
			writeValue("ALPHA_ANIM_INDEX", i);
			writeAlphaAnimInfo(info.mAlphaAnimInfo[i]);
		}
		endSection();
	}

	void writeLightingInfo(const LightingInfo& info)
	{
		beginSection("LIGHTING_INFO");
		writeValue("FLAGS", LightingInfoFlagsToString(info.mFlags));
		writeValue("UNK", info.mUnknown);
		endSection();
	}

	void writePeInfo(const PeInfo& info)
	{
		beginSection("PE_INFO");
		writeValue("FLAGS", info.mFlags);
		beginSection("ALPHA_COMPARE_FUNCTION");
		writeValue("COMP0", info.mAlphaCompareFunction.bits.comp0);
		writeValue("REF0", info.mAlphaCompareFunction.bits.ref0);
		writeValue("OP", info.mAlphaCompareFunction.bits.op);
		writeValue("COMP1", info.mAlphaCompareFunction.bits.comp1);
		writeValue("REF1", info.mAlphaCompareFunction.bits.ref1);
		endSection();
		writeValue("Z_MODE_FUNCTION", info.mZModeFunction);
		beginSection("BLEND_MODE");
		writeValue("TYPE", info.mBlendMode.bits.mType);
		writeValue("SRC_FACTOR", info.mBlendMode.bits.mSrcFactor);
		writeValue("DST_FACTOR", info.mBlendMode.bits.mDstFactor);
		writeValue("LOGIC_OP", info.mBlendMode.bits.mLogicOp);
		endSection();
		endSection();
	}

	void writeTexGenData(const TexGenData& data)
	{
		writeValue("DESTINATION_COORDS", GXTexCoordIDToStringConverter(static_cast<GXTexCoordID>(data.mDestinationCoords)));
		writeValue("FUNC", GXTexGenTypeToStringConverter(static_cast<GXTexGenType>(data.mFunc)));
		writeValue("SOURCE_PARAM", GXTexGenSrcToStringConverter(static_cast<GXTexGenSrc>(data.mSourceParam)));
		writeValue("TEXTURE_MTX", GXTexMtxToStringConverter(static_cast<GXTexMtx>(data.mTexMtx)));
	}

	void writeTextureData(const TextureData& data)
	{
		writeValue("TEX_ATTR_IDX", data.mTextureAttributeIndex);
		writeValue("WRAP_MODE_S", GXTexWrapModeToStringConverter(static_cast<GXTexWrapMode>(data.mWrapModeS)));
		writeValue("WRAP_MODE_T", GXTexWrapModeToStringConverter(static_cast<GXTexWrapMode>(data.mWrapModeT)));
		writeValue("UNK4", (u32)data.mUnknown3);
		writeValue("UNK5", (u32)data.mUnknown4);
		writeValue("UNK6", (u32)data.mUnknown5);
		writeValue("UNK7", (u32)data.mUnknown6);
		writeValue("ANIMATION_FACTOR", data.mAnimationFactor);
		writeValue("ANIM_LENGTH", data.mAnimLength);
		writeValue("ANIM_SPEED", data.mAnimSpeed);
		writeVec("SCALE", data.mScale);
		writeValue("ROTATION", data.mRotation);
		writeVec("PIVOT", data.mPivot);
		writeVec("POSITION", data.mPosition);

		writeValue("SCALE_FRAME_COUNT", data.mScaleInfo.size());
		for (size_t i = 0; i < data.mScaleInfo.size(); ++i) {
			beginSection("ENTRY " + std::to_string(i));
			writeTextureAnimData(data.mScaleInfo[i]);
			endSection();
		}

		writeValue("ROTATION_FRAME_COUNT", data.mRotationInfo.size());
		for (size_t i = 0; i < data.mRotationInfo.size(); ++i) {
			beginSection("ENTRY " + std::to_string(i));
			writeTextureAnimData(data.mRotationInfo[i]);
			endSection();
		}

		writeValue("TRANSLATION_FRAME_COUNT", data.mTranslationInfo.size());
		for (size_t i = 0; i < data.mTranslationInfo.size(); ++i) {
			beginSection("ENTRY " + std::to_string(i));
			writeTextureAnimData(data.mTranslationInfo[i]);
			endSection();
		}
	}

	void writeTextureInfo(const TextureInfo& info)
	{
		beginSection("TEXTURE_INFO");
		writeValue("USE_SCALE", info.mUseScale);
		writeVec("SCALE", info.mScale);

		writeValue("TEXTURE_GEN_DATA_COUNT", info.mTextureGenData.size());
		for (size_t i = 0; i < info.mTextureGenData.size(); ++i) {
			beginSection("ENTRY " + std::to_string(i));
			writeTexGenData(info.mTextureGenData[i]);
			endSection();
		}

		writeValue("TEXDATA_COUNT", info.mTextureData.size());
		for (size_t i = 0; i < info.mTextureData.size(); ++i) {
			beginSection("TEXDATA_ENTRY " + std::to_string(i));
			writeTextureData(info.mTextureData[i]);
			endSection();
		}
		endSection();
	}

	void writePVWCombiner(const std::string& label, const PVWCombiner& combiner)
	{
		beginSection(label);
		writeValue("INPUT_A", GXTevColorArgToStringConverter((GXTevColorArg)combiner.mInputABCD[0]));
		writeValue("INPUT_B", GXTevColorArgToStringConverter((GXTevColorArg)combiner.mInputABCD[1]));
		writeValue("INPUT_C", GXTevColorArgToStringConverter((GXTevColorArg)combiner.mInputABCD[2]));
		writeValue("INPUT_D", GXTevColorArgToStringConverter((GXTevColorArg)combiner.mInputABCD[3]));
		writeValue("OP", (u32)combiner.mOp);
		writeValue("BIAS", (u32)combiner.mBias);
		writeValue("SCALE", (u32)combiner.mScale);
		writeValue("CLAMP", (u32)combiner.mClamp);
		writeValue("OUT_REG", (u32)combiner.mOutReg);
		writeValue("UNKNOWN_A", (u32)combiner._unused[0]);
		writeValue("UNKNOWN_B", (u32)combiner._unused[1]);
		writeValue("UNKNOWN_C", (u32)combiner._unused[2]);
		endSection();
	}

	void writeTEVStage(const TEVStage& stage)
	{
		writeValue("UNKNOWN", (u32)stage.mUnknown);
		writeValue("TEX_COORD_ID", GXTexCoordIDToStringConverter(static_cast<GXTexCoordID>(stage.mTexCoordID)));
		writeValue("TEX_MAP_ID", GXTexMapIDToStringConverter(static_cast<GXTexMapID>(stage.mTexMapID)));
		writeValue("GX_CHANNEL_ID", GXChannelIDToStringConverter(static_cast<GXChannelID>(stage.mGXChannelID)));
		writeValue("K_COLOR_SEL", GXTevKColorSelToStringConverter(static_cast<GXTevKColorSel>(stage.mKColorSel)));
		writeValue("K_ALPHA_SEL", GXTevKAlphaSelToStringConverter(static_cast<GXTevKAlphaSel>(stage.mKAlphaSel)));
		writePVWCombiner("TEV_COLOR_COMBINER", stage.mTevColorCombiner);
		writePVWCombiner("TEV_ALPHA_COMBINER", stage.mTevAlphaCombiner);
	}

	void writeTEVColReg(const std::string& label, const TEVColReg& reg)
	{
		beginSection(label);
		writeColour("COLOUR", reg.mColour);
		writeValue("ANIM_LENGTH", reg.mAnimLength);
		writeValue("ANIM_SPEED", reg.mAnimSpeed);

		writeValue("COLOR_ANIM_COUNT", reg.mColorAnimInfo.size());
		for (size_t i = 0; i < reg.mColorAnimInfo.size(); ++i) {
			beginSection("COLOR_ANIM_ENTRY " + std::to_string(i));
			writePVWAnimInfo_3_S10(reg.mColorAnimInfo[i]);
			endSection();
		}

		writeValue("ALPHA_ANIM_COUNT", reg.mAlphaAnimInfo.size());
		for (size_t i = 0; i < reg.mAlphaAnimInfo.size(); ++i) {
			beginSection("ALPHA_ANIM_ENTRY " + std::to_string(i));
			writePVWAnimInfo_1_S10(reg.mAlphaAnimInfo[i]);
			endSection();
		}
		endSection();
	}

	void writeTEVInfo(const TEVInfo& info)
	{
		writeTEVColReg("TEVCOLREG_A", info.mTevColourRegA);
		writeTEVColReg("TEVCOLREG_B", info.mTevColourRegB);
		writeTEVColReg("TEVCOLREG_C", info.mTevColourRegC);

		writeColour("KONST_COL_A", info.mKonstColourA);
		writeColour("KONST_COL_B", info.mKonstColourB);
		writeColour("KONST_COL_C", info.mKonstColourC);
		writeColour("KONST_COL_D", info.mKonstColourD);

		writeValue("TEVSTAGE_COUNT", info.mTevStages.size());
		for (size_t i = 0; i < info.mTevStages.size(); ++i) {
			beginSection("TEVSTAGE_ENTRY " + std::to_string(i));
			writeTEVStage(info.mTevStages[i]);
			endSection();
		}
	}

	void writeMaterial(const Material& mat)
	{
		writeValue("FLAGS", MaterialFlagsToString(mat.mFlags));
		writeValue("TEXTURE_INDEX", mat.mTextureIndex);
		writeColour("COLOUR", mat.mColourInfo.mDiffuseColour);

		if (mat.mFlags & static_cast<u32>(MaterialFlags::IsEnabled)) {
			writeValue("TEV_GROUP_ID", mat.mTevGroupId);
			writePolygonColourInfo(mat.mColourInfo);
			writeLightingInfo(mat.mLightingInfo);
			writePeInfo(mat.mPeInfo);
			writeTextureInfo(mat.mTexInfo);
		}
	}
};

class MaterialReader {
private:
	std::ifstream& m_in;
	std::string m_currentLine;
	int m_currentIndent = 0;

	bool readLine()
	{
		if (!std::getline(m_in, m_currentLine))
			return false;

		// Calculate indent level
		m_currentIndent = 0;
		for (char c : m_currentLine) {
			if (c == '\t')
				m_currentIndent++;
			else
				break;
		}
		return true;
	}

	std::string trimLine() const { return trim(m_currentLine); }

	std::pair<std::string, std::string> parseKeyValue()
	{
		std::string line = trimLine();
		size_t pos       = line.find(' ');
		if (pos == std::string::npos)
			return { line, "" };
		return { line.substr(0, pos), line.substr(pos + 1) };
	}

	std::vector<std::string> split(const std::string& str, char delimiter)
	{
		std::vector<std::string> tokens;
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, delimiter)) {
			// Trim whitespace from token
			size_t start = token.find_first_not_of(" ");
			size_t end   = token.find_last_not_of(" ");
			if (start != std::string::npos) {
				tokens.push_back(token.substr(start, end - start + 1));
			}
		}
		return tokens;
	}

	void parseColour(const std::string& str, ColourU8& colour)
	{
		std::istringstream iss(str);
		u32 r, g, b, a;
		iss >> r >> g >> b >> a;
		colour.r = static_cast<u8>(r);
		colour.g = static_cast<u8>(g);
		colour.b = static_cast<u8>(b);
		colour.a = static_cast<u8>(a);
	}

	void parseColour(const std::string& str, ColourU16& colour)
	{
		std::istringstream iss(str);
		u32 r, g, b, a;
		iss >> r >> g >> b >> a;
		colour.r = static_cast<u16>(r);
		colour.g = static_cast<u16>(g);
		colour.b = static_cast<u16>(b);
		colour.a = static_cast<u16>(a);
	}

	void parseVec(const std::string& str, Vector3f& vec)
	{
		std::istringstream iss(str);
		iss >> vec.x >> vec.y >> vec.z;
	}

	void parseVec(const std::string& str, Vector2f& vec)
	{
		std::istringstream iss(str);
		iss >> vec.x >> vec.y;
	}

	u32 parseMaterialFlags(const std::string& flagStr)
	{
		u32 flags      = 0;
		auto flagNames = split(flagStr, ',');
		for (const auto& name : flagNames) {
			if (name == "IsEnabled")
				flags |= static_cast<u32>(MaterialFlags::IsEnabled);
			else if (name == "Opaque")
				flags |= static_cast<u32>(MaterialFlags::Opaque);
			else if (name == "AlphaClip")
				flags |= static_cast<u32>(MaterialFlags::AlphaClip);
			else if (name == "TransparentBlend")
				flags |= static_cast<u32>(MaterialFlags::TransparentBlend);
			else if (name == "InvertSpecialBlend")
				flags |= static_cast<u32>(MaterialFlags::InvertSpecialBlend);
			else if (name == "Hidden")
				flags |= static_cast<u32>(MaterialFlags::Hidden);
		}
		return flags;
	}

	u32 parseLightingFlags(const std::string& flagStr)
	{
		using mat::LightingInfoFlags;

		static const std::unordered_map<std::string, u32> flagMap
		    = { { "EnableColor0", static_cast<u32>(LightingInfoFlags::EnableColor0) },
			    { "EnableSpecular", static_cast<u32>(LightingInfoFlags::EnableSpecular) },
			    { "EnableAlpha0", static_cast<u32>(LightingInfoFlags::EnableAlpha0) },
			    { "Unknown8", static_cast<u32>(LightingInfoFlags::Unknown8) },
			    { "Unknown10", static_cast<u32>(LightingInfoFlags::Unknown10) },
			    { "Unknown20", static_cast<u32>(LightingInfoFlags::Unknown20) },
			    { "Unknown40", static_cast<u32>(LightingInfoFlags::Unknown40) },
			    { "Unknown80", static_cast<u32>(LightingInfoFlags::Unknown80) },
			    { "Unknown100", static_cast<u32>(LightingInfoFlags::Unknown100) },
			    { "AmbSrcColor0Vtx", static_cast<u32>(LightingInfoFlags::AmbSrcColor0Vtx) },
			    { "AmbSrcAlpha0Vtx", static_cast<u32>(LightingInfoFlags::AmbSrcAlpha0Vtx) },
			    { "MatSrcColor0Vtx", static_cast<u32>(LightingInfoFlags::MatSrcColor0Vtx) },
			    { "MatSrcAlpha0Vtx", static_cast<u32>(LightingInfoFlags::MatSrcAlpha0Vtx) } };

		u32 flags      = 0;
		auto flagNames = split(flagStr, ',');

		for (auto& rawName : flagNames) {
			auto name = trim(rawName);
			auto it   = flagMap.find(name);
			if (it != flagMap.end()) {
				flags |= it->second;
			}
		}

		return flags;
	}

	bool parseKeyInfoU8(const std::string& data, KeyInfoU8& key)
	{
		// Parse "TIME[123] value tangent"
		size_t start = data.find('[');
		size_t end   = data.find(']');
		if (start != std::string::npos && end != std::string::npos) {
			key.mTime = static_cast<u8>(std::stoi(data.substr(start + 1, end - start - 1)));
			std::istringstream iss(data.substr(end + 2));
			iss >> key.mValue >> key.mTangent;
		}
		return true;
	}

	bool parseKeyInfoF32(const std::string& data, KeyInfoF32& key)
	{
		std::istringstream iss(data);
		iss >> key.mTime >> key.mValue >> key.mTangent;
		return true;
	}

	bool parseKeyInfoS10(const std::string& data, KeyInfoS10& key)
	{
		// Parse "TIME[123] value tangent"
		size_t start = data.find('[');
		size_t end   = data.find(']');
		if (start != std::string::npos && end != std::string::npos) {
			key.mTime = static_cast<s16>(std::stoi(data.substr(start + 1, end - start - 1)));
			std::istringstream iss(data.substr(end + 2));
			iss >> key.mValue >> key.mTangent;
		}
		return true;
	}

	// GX enum reverse mappings - based on gxdefines.hpp
	u8 parseGXTexCoordID(const std::string& str)
	{
		if (str == "GX_TEXCOORD0")
			return 0x0;
		if (str == "GX_TEXCOORD1")
			return 0x1;
		if (str == "GX_TEXCOORD2")
			return 0x2;
		if (str == "GX_TEXCOORD3")
			return 0x3;
		if (str == "GX_TEXCOORD4")
			return 0x4;
		if (str == "GX_TEXCOORD5")
			return 0x5;
		if (str == "GX_TEXCOORD6")
			return 0x6;
		if (str == "GX_TEXCOORD7")
			return 0x7;
		if (str == "GX_MAX_TEXCOORD")
			return 8;
		if (str == "GX_TEXCOORD_NULL")
			return 0xff;
		return 0xff;
	}

	u8 parseGXTexGenType(const std::string& str)
	{
		if (str == "GX_TG_MTX3x4")
			return 0;
		if (str == "GX_TG_MTX2x4")
			return 1;
		if (str == "GX_TG_BUMP0")
			return 2;
		if (str == "GX_TG_BUMP1")
			return 3;
		if (str == "GX_TG_BUMP2")
			return 4;
		if (str == "GX_TG_BUMP3")
			return 5;
		if (str == "GX_TG_BUMP4")
			return 6;
		if (str == "GX_TG_BUMP5")
			return 7;
		if (str == "GX_TG_BUMP6")
			return 8;
		if (str == "GX_TG_BUMP7")
			return 9;
		if (str == "GX_TG_SRTG")
			return 10;
		return 0;
	}

	u8 parseGXTexGenSrc(const std::string& str)
	{
		if (str == "GX_TG_POS")
			return 0;
		if (str == "GX_TG_NRM")
			return 1;
		if (str == "GX_TG_BINRM")
			return 2;
		if (str == "GX_TG_TANGENT")
			return 3;
		if (str == "GX_TG_TEX0")
			return 4;
		if (str == "GX_TG_TEX1")
			return 5;
		if (str == "GX_TG_TEX2")
			return 6;
		if (str == "GX_TG_TEX3")
			return 7;
		if (str == "GX_TG_TEX4")
			return 8;
		if (str == "GX_TG_TEX5")
			return 9;
		if (str == "GX_TG_TEX6")
			return 10;
		if (str == "GX_TG_TEX7")
			return 11;
		if (str == "GX_TG_TEXCOORD0")
			return 12;
		if (str == "GX_TG_TEXCOORD1")
			return 13;
		if (str == "GX_TG_TEXCOORD2")
			return 14;
		if (str == "GX_TG_TEXCOORD3")
			return 15;
		if (str == "GX_TG_TEXCOORD4")
			return 16;
		if (str == "GX_TG_TEXCOORD5")
			return 17;
		if (str == "GX_TG_TEXCOORD6")
			return 18;
		if (str == "GX_TG_COLOR0")
			return 19;
		if (str == "GX_TG_COLOR1")
			return 20;
		return 0;
	}

	u8 parseGXTexMtx(const std::string& str)
	{
		if (str == "GX_TEXMTX0")
			return 0;
		if (str == "GX_TEXMTX1")
			return 1;
		if (str == "GX_TEXMTX2")
			return 2;
		if (str == "GX_TEXMTX3")
			return 3;
		if (str == "GX_TEXMTX4")
			return 4;
		if (str == "GX_TEXMTX5")
			return 5;
		if (str == "GX_TEXMTX6")
			return 6;
		if (str == "GX_TEXMTX7")
			return 7;
		if (str == "GX_TEXMTX8")
			return 8;
		if (str == "GX_TEXMTX9")
			return 9;
		if (str == "GX_IDENTITY")
			return 0xFF;

		// If it's a number string, parse it
		try {
			return static_cast<u8>(std::stoi(str));
		} catch (...) {
			return 0xFF;
		}
	}

	s16 parseGXTexWrapMode(const std::string& str)
	{
		if (str == "GX_CLAMP")
			return 0;
		if (str == "GX_REPEAT")
			return 1;
		if (str == "GX_MIRROR")
			return 2;
		if (str == "GX_MAX_TEXWRAPMODE")
			return 3;
		return 0;
	}

	u16 parseGXTexMapID(const std::string& str)
	{
		if (str == "GX_TEXMAP0")
			return 0;
		if (str == "GX_TEXMAP1")
			return 1;
		if (str == "GX_TEXMAP2")
			return 2;
		if (str == "GX_TEXMAP3")
			return 3;
		if (str == "GX_TEXMAP4")
			return 4;
		if (str == "GX_TEXMAP5")
			return 5;
		if (str == "GX_TEXMAP6")
			return 6;
		if (str == "GX_TEXMAP7")
			return 7;
		if (str == "GX_MAX_TEXMAP")
			return 8;
		if (str == "GX_TEXMAP_NULL")
			return 0xff;
		if (str == "GX_TEX_DISABLE")
			return 0x100;
		return 0xff;
	}

	u8 parseGXChannelID(const std::string& str)
	{
		if (str == "GX_COLOR0")
			return 0;
		if (str == "GX_COLOR1")
			return 1;
		if (str == "GX_ALPHA0")
			return 2;
		if (str == "GX_ALPHA1")
			return 3;
		if (str == "GX_COLOR0A0")
			return 4;
		if (str == "GX_COLOR1A1")
			return 5;
		if (str == "GX_COLOR_ZERO")
			return 6;
		if (str == "GX_ALPHA_BUMP")
			return 7;
		if (str == "GX_ALPHA_BUMPN")
			return 8;
		if (str == "GX_COLOR_NULL")
			return 0xff;
		return 0xff;
	}

	u8 parseGXTevKColorSel(const std::string& str)
	{
		if (str == "GX_TEV_KCSEL_1")
			return 0x00;
		if (str == "GX_TEV_KCSEL_7_8")
			return 0x01;
		if (str == "GX_TEV_KCSEL_3_4")
			return 0x02;
		if (str == "GX_TEV_KCSEL_5_8")
			return 0x03;
		if (str == "GX_TEV_KCSEL_1_2")
			return 0x04;
		if (str == "GX_TEV_KCSEL_3_8")
			return 0x05;
		if (str == "GX_TEV_KCSEL_1_4")
			return 0x06;
		if (str == "GX_TEV_KCSEL_1_8")
			return 0x07;
		if (str == "GX_TEV_KCSEL_K0")
			return 0x0C;
		if (str == "GX_TEV_KCSEL_K1")
			return 0x0D;
		if (str == "GX_TEV_KCSEL_K2")
			return 0x0E;
		if (str == "GX_TEV_KCSEL_K3")
			return 0x0F;
		if (str == "GX_TEV_KCSEL_K0_R")
			return 0x10;
		if (str == "GX_TEV_KCSEL_K1_R")
			return 0x11;
		if (str == "GX_TEV_KCSEL_K2_R")
			return 0x12;
		if (str == "GX_TEV_KCSEL_K3_R")
			return 0x13;
		if (str == "GX_TEV_KCSEL_K0_G")
			return 0x14;
		if (str == "GX_TEV_KCSEL_K1_G")
			return 0x15;
		if (str == "GX_TEV_KCSEL_K2_G")
			return 0x16;
		if (str == "GX_TEV_KCSEL_K3_G")
			return 0x17;
		if (str == "GX_TEV_KCSEL_K0_B")
			return 0x18;
		if (str == "GX_TEV_KCSEL_K1_B")
			return 0x19;
		if (str == "GX_TEV_KCSEL_K2_B")
			return 0x1A;
		if (str == "GX_TEV_KCSEL_K3_B")
			return 0x1B;
		if (str == "GX_TEV_KCSEL_K0_A")
			return 0x1C;
		if (str == "GX_TEV_KCSEL_K1_A")
			return 0x1D;
		if (str == "GX_TEV_KCSEL_K2_A")
			return 0x1E;
		if (str == "GX_TEV_KCSEL_K3_A")
			return 0x1F;
		return 0x1F;
	}

	u8 parseGXTevKAlphaSel(const std::string& str)
	{
		if (str == "GX_TEV_KASEL_1")
			return 0x00;
		if (str == "GX_TEV_KASEL_7_8")
			return 0x01;
		if (str == "GX_TEV_KASEL_3_4")
			return 0x02;
		if (str == "GX_TEV_KASEL_5_8")
			return 0x03;
		if (str == "GX_TEV_KASEL_1_2")
			return 0x04;
		if (str == "GX_TEV_KASEL_3_8")
			return 0x05;
		if (str == "GX_TEV_KASEL_1_4")
			return 0x06;
		if (str == "GX_TEV_KASEL_1_8")
			return 0x07;
		if (str == "GX_TEV_KASEL_K0_R")
			return 0x10;
		if (str == "GX_TEV_KASEL_K1_R")
			return 0x11;
		if (str == "GX_TEV_KASEL_K2_R")
			return 0x12;
		if (str == "GX_TEV_KASEL_K3_R")
			return 0x13;
		if (str == "GX_TEV_KASEL_K0_G")
			return 0x14;
		if (str == "GX_TEV_KASEL_K1_G")
			return 0x15;
		if (str == "GX_TEV_KASEL_K2_G")
			return 0x16;
		if (str == "GX_TEV_KASEL_K3_G")
			return 0x17;
		if (str == "GX_TEV_KASEL_K0_B")
			return 0x18;
		if (str == "GX_TEV_KASEL_K1_B")
			return 0x19;
		if (str == "GX_TEV_KASEL_K2_B")
			return 0x1A;
		if (str == "GX_TEV_KASEL_K3_B")
			return 0x1B;
		if (str == "GX_TEV_KASEL_K0_A")
			return 0x1C;
		if (str == "GX_TEV_KASEL_K1_A")
			return 0x1D;
		if (str == "GX_TEV_KASEL_K2_A")
			return 0x1E;
		if (str == "GX_TEV_KASEL_K3_A")
			return 0x1F;
		return 0x1F;
	}

	u8 parseGXTevColorArg(const std::string& str)
	{
		if (str == "GX_CC_CPREV")
			return 0;
		if (str == "GX_CC_APREV")
			return 1;
		if (str == "GX_CC_C0")
			return 2;
		if (str == "GX_CC_A0")
			return 3;
		if (str == "GX_CC_C1")
			return 4;
		if (str == "GX_CC_A1")
			return 5;
		if (str == "GX_CC_C2")
			return 6;
		if (str == "GX_CC_A2")
			return 7;
		if (str == "GX_CC_TEXC")
			return 8;
		if (str == "GX_CC_TEXA")
			return 9;
		if (str == "GX_CC_RASC")
			return 10;
		if (str == "GX_CC_RASA")
			return 11;
		if (str == "GX_CC_ONE")
			return 12;
		if (str == "GX_CC_HALF")
			return 13;
		if (str == "GX_CC_QUARTER")
			return 14;
		if (str == "GX_CC_ZERO")
			return 15;
		if (str == "GX_CC_TEXRRR")
			return 16;
		if (str == "GX_CC_TEXGGG")
			return 17;
		if (str == "GX_CC_TEXBBB")
			return 18;
		return 15;
	}

public:
	MaterialReader(std::ifstream& in)
	    : m_in(in)
	{
	}

	bool readMaterialFile(std::vector<Material>& materials, std::vector<TEVInfo>& tevInfos)
	{
		if (!readLine() || trimLine() != "MATERIAL_FILE") {
			std::cerr << "Invalid material file format\n";
			return false;
		}

		while (true) {
			auto pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (line == "MAT_SECTION") {
				if (!readMaterialSection(materials))
					return false;
			} else if (line == "TEV_SECTION" || line.find("TEV ") == 0) {
				if (line.find("TEV ") == 0) {
					m_in.seekg(pos);
				}

				if (!readTEVSection(tevInfos))
					return false;
			}
		}

		return true;
	}

private:
	bool readMaterialSection(std::vector<Material>& materials)
	{
		while (true) {
			auto pos = m_in.tellg();
			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty())
				continue;

			auto [key, value] = parseKeyValue();
			if (key == "MAT") {
				Material mat;
				if (!readMaterial(mat))
					return false;
				materials.push_back(mat);
			} else if (key == "TEV_SECTION" || line.find("TEV ") == 0) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}
		}

		return true;
	}

	bool readTEVSection(std::vector<TEVInfo>& tevInfos)
	{
		while (readLine()) {
			std::string line = trimLine();
			if (line.empty())
				continue;

			auto [key, value] = parseKeyValue();
			if (key == "TEV") {
				TEVInfo tev;
				if (!readTEVInfo(tev))
					return false;
				tevInfos.push_back(tev);
			}
		}
		return true;
	}

	bool readMaterial(Material& mat)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			// Check if we've hit the next material or section
			if (m_currentIndent == 0
			    && (line == "MAT" || line.find("MAT ") == 0 || line == "TEV_SECTION" || line == "TEV" || line.find("TEV ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "FLAGS") {
				mat.mFlags = parseMaterialFlags(value);
			} else if (key == "TEXTURE_INDEX") {
				mat.mTextureIndex = std::stol(value);
			} else if (key == "COLOUR") {
				parseColour(value, mat.mColourInfo.mDiffuseColour);
			} else if (key == "TEV_GROUP_ID") {
				mat.mTevGroupId = std::stoi(value);
			} else if (key == "POLYGON_COLOUR_INFO") {
				if (!readPolygonColourInfo(mat.mColourInfo))
					return false;
			} else if (key == "LIGHTING_INFO") {
				if (!readLightingInfo(mat.mLightingInfo))
					return false;
			} else if (key == "PE_INFO") {
				if (!readPeInfo(mat.mPeInfo))
					return false;
			} else if (key == "TEXTURE_INFO") {
				if (!readTextureInfo(mat.mTexInfo))
					return false;
			}
		}
		return true;
	}

	bool readPolygonColourInfo(PolygonColourInfo& info)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 1
			    && (line == "LIGHTING_INFO" || line == "PE_INFO" || line == "TEXTURE_INFO" || line == "MAT" || line.find("MAT ") == 0
			        || line == "TEV" || line.find("TEV ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "DIFFUSE_COLOUR") {
				parseColour(value, info.mDiffuseColour);
			} else if (key == "ANIM_LENGTH") {
				info.mAnimLength = std::stoi(value);
			} else if (key == "ANIM_SPEED") {
				info.mAnimSpeed = std::stof(value);
			} else if (key == "COLOUR_ANIM_COUNT") {
				unsigned int count = std::stoul(value);
				info.mColourAnimInfo.clear();
				info.mColourAnimInfo.reserve(count);
			} else if (key == "COLOUR_ANIM_INDEX") {
				ColourAnimInfo colAnim;
				if (!readColourAnimInfo(colAnim))
					return false;
				info.mColourAnimInfo.push_back(colAnim);
			} else if (key == "ALPHA_ANIM_COUNT") {
				unsigned int count = std::stoul(value);
				info.mAlphaAnimInfo.clear();
				info.mAlphaAnimInfo.reserve(count);
			} else if (key == "ALPHA_ANIM_INDEX") {
				AlphaAnimInfo alphaAnim;
				if (!readAlphaAnimInfo(alphaAnim))
					return false;
				info.mAlphaAnimInfo.push_back(alphaAnim);
			}
		}
		return true;
	}

	bool readColourAnimInfo(ColourAnimInfo& info)
	{
		readLine(); // COLOUR_ANIM

		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			// End of this animation entry
			if (m_currentIndent <= 2) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "INDEX") {
				info.mIndex = std::stoi(value);
			} else if (key == "RED_KEYFRAME") {
				parseKeyInfoU8(value, info.mKeyDataR);
			} else if (key == "GREEN_KEYFRAME") {
				parseKeyInfoU8(value, info.mKeyDataG);
			} else if (key == "BLUE_KEYFRAME") {
				parseKeyInfoU8(value, info.mKeyDataB);
			}
		}
		return true;
	}

	bool readAlphaAnimInfo(AlphaAnimInfo& info)
	{
		readLine(); // ALPHA_ANIM

		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			// End of this animation entry
			if (m_currentIndent <= 2) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "INDEX") {
				info.mIndex = std::stoi(value);
			} else if (key == "ALPHA_KEYFRAME") {
				parseKeyInfoU8(value, info.mKeyData);
			}
		}
		return true;
	}

	bool readLightingInfo(LightingInfo& info)
	{
		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			if (m_currentIndent <= 1 && (line == "PE_INFO" || line == "TEXTURE_INFO" || line == "MAT" || line.find("MAT ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "FLAGS") {
				info.mFlags = parseLightingFlags(value);
			} else if (key == "UNK") {
				info.mUnknown = std::stof(value);
			}
		}
		return true;
	}

	bool readPeInfo(PeInfo& info)
	{
		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			if (m_currentIndent <= 1 && (line == "TEXTURE_INFO" || line == "MAT" || line.find("MAT ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "FLAGS") {
				info.mFlags = std::stoi(value);
			} else if (key == "ALPHA_COMPARE_FUNCTION") {
				if (!readAlphaCompareFunction(info.mAlphaCompareFunction))
					return false;
			} else if (key == "Z_MODE_FUNCTION") {
				info.mZModeFunction = std::stoi(value);
			} else if (key == "BLEND_MODE") {
				if (!readBlendMode(info.mBlendMode))
					return false;
			}
		}
		return true;
	}

	bool readAlphaCompareFunction(PeInfo::AlphaCompareFunction& func)
	{
		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			if (m_currentIndent <= 2) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "COMP0")
				func.bits.comp0 = std::stoi(value);
			else if (key == "REF0")
				func.bits.ref0 = std::stoi(value);
			else if (key == "OP")
				func.bits.op = std::stoi(value);
			else if (key == "COMP1")
				func.bits.comp1 = std::stoi(value);
			else if (key == "REF1")
				func.bits.ref1 = std::stoi(value);
		}
		return true;
	}

	bool readBlendMode(PeInfo::BlendMode& mode)
	{
		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			if (m_currentIndent <= 2) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "TYPE")
				mode.bits.mType = std::stoi(value);
			else if (key == "SRC_FACTOR")
				mode.bits.mSrcFactor = std::stoi(value);
			else if (key == "DST_FACTOR")
				mode.bits.mDstFactor = std::stoi(value);
			else if (key == "LOGIC_OP")
				mode.bits.mLogicOp = std::stoi(value);
		}
		return true;
	}

	bool readTextureInfo(TextureInfo& info)
	{
		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			if (m_currentIndent == 0 && (line == "MAT" || line.find("MAT ") == 0 || line == "TEV" || line.find("TEV ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "USE_SCALE") {
				info.mUseScale = std::stoi(value);
			} else if (key == "SCALE") {
				parseVec(value, info.mScale);
			} else if (key == "TEXTURE_GEN_DATA_COUNT") {
				u32 count = std::stoul(value);
				info.mTextureGenData.clear();
				info.mTextureGenData.reserve(count);
			} else if (key == "ENTRY") {
				// Check what type of entry this is by peeking ahead
				std::streampos savedPos = m_in.tellg();
				std::string savedLine   = m_currentLine;
				int savedIndent         = m_currentIndent;

				if (readLine()) {
					auto [nextKey, nextValue] = parseKeyValue();
					m_in.clear();
					m_in.seekg(savedPos);
					m_currentLine   = savedLine;
					m_currentIndent = savedIndent;

					if (nextKey == "DESTINATION_COORDS") {
						TexGenData genData;
						if (!readTexGenData(genData))
							return false;
						info.mTextureGenData.push_back(genData);
					}
				}
			} else if (key == "TEXDATA_COUNT") {
				u32 count = std::stoul(value);
				info.mTextureData.clear();
				info.mTextureData.reserve(count);
			} else if (key == "TEXDATA_ENTRY") {
				TextureData texData;
				if (!readTextureData(texData))
					return false;
				info.mTextureData.push_back(texData);
			}
		}
		return true;
	}

	bool readTexGenData(TexGenData& data)
	{
		while (true) {
			std::streampos pos = m_in.tellg();

			if (!readLine()) {
				break;
			}

			std::string line = trimLine();
			if (line.empty()) {
				continue;
			}

			if (m_currentIndent <= 3
			    && (line == "ENTRY" || line.find("ENTRY ") == 0 || line.find("TEXDATA_COUNT ") == 0 || line.find("TEXDATA_ENTRY ") == 0)) {
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "DESTINATION_COORDS") {
				data.mDestinationCoords = parseGXTexCoordID(value);
			} else if (key == "FUNC") {
				data.mFunc = parseGXTexGenType(value);
			} else if (key == "SOURCE_PARAM") {
				data.mSourceParam = parseGXTexGenSrc(value);
			} else if (key == "TEXTURE_MTX") {
				data.mTexMtx = parseGXTexMtx(value);
			}
		}
		return true;
	}

	bool readTextureData(TextureData& data)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 2
			    && (line == "TEXDATA_ENTRY" || line.find("TEXDATA_ENTRY ") == 0 || line == "MAT" || line.find("MAT ") == 0
			        || line.find("TEV_SECTION") == 0 || line.find("TEV ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "TEX_ATTR_IDX") {
				data.mTextureAttributeIndex = std::stoi(value);
			} else if (key == "WRAP_MODE_S") {
				data.mWrapModeS = parseGXTexWrapMode(value);
			} else if (key == "WRAP_MODE_T") {
				data.mWrapModeT = parseGXTexWrapMode(value);
			} else if (key == "UNK4") {
				data.mUnknown3 = static_cast<u8>(std::stoi(value));
			} else if (key == "UNK5") {
				data.mUnknown4 = static_cast<u8>(std::stoi(value));
			} else if (key == "UNK6") {
				data.mUnknown5 = static_cast<u8>(std::stoi(value));
			} else if (key == "UNK7") {
				data.mUnknown6 = static_cast<u8>(std::stoi(value));
			} else if (key == "ANIMATION_FACTOR") {
				data.mAnimationFactor = std::stoi(value);
			} else if (key == "ANIM_LENGTH") {
				data.mAnimLength = std::stoi(value);
			} else if (key == "ANIM_SPEED") {
				data.mAnimSpeed = std::stof(value);
			} else if (key == "SCALE") {
				parseVec(value, data.mScale);
			} else if (key == "ROTATION") {
				data.mRotation = std::stof(value);
			} else if (key == "PIVOT") {
				parseVec(value, data.mPivot);
			} else if (key == "POSITION") {
				parseVec(value, data.mPosition);
			} else if (key == "SCALE_FRAME_COUNT") {
				u32 count = std::stoul(value);
				data.mScaleInfo.clear();
				data.mScaleInfo.reserve(count);
			} else if (key == "ROTATION_FRAME_COUNT") {
				u32 count = std::stoul(value);
				data.mRotationInfo.clear();
				data.mRotationInfo.reserve(count);
			} else if (key == "TRANSLATION_FRAME_COUNT") {
				u32 count = std::stoul(value);
				data.mTranslationInfo.clear();
				data.mTranslationInfo.reserve(count);
			} else if (key == "ENTRY") {
				TextureAnimData animData;
				if (data.mScaleInfo.size() < data.mScaleInfo.capacity()) {
					if (!readTextureAnimData(animData))
						return false;
					data.mScaleInfo.push_back(animData);
				} else if (data.mRotationInfo.size() < data.mRotationInfo.capacity()) {
					if (!readTextureAnimData(animData))
						return false;
					data.mRotationInfo.push_back(animData);
				} else if (data.mTranslationInfo.size() < data.mTranslationInfo.capacity()) {
					if (!readTextureAnimData(animData))
						return false;
					data.mTranslationInfo.push_back(animData);
				}
			}
		}
		return true;
	}

	bool readTextureAnimData(TextureAnimData& data)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 4
			        && (line == "ENTRY" || line.find("ENTRY ") == 0 || line.find("TEXDATA_ENTRY") == 0
			            || line.find("FRAME_COUNT") != std::string::npos || line.find("MAT ") == 0)
			    || line.find("TEV_SECTION") == 0) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "ANIMATION_FRAME") {
				data.mAnimationFrame = std::stoi(value);
			} else if (key == "VALUE_X") {
				parseKeyInfoF32(value, data.mValueX);
			} else if (key == "VALUE_Y") {
				parseKeyInfoF32(value, data.mValueY);
			} else if (key == "VALUE_Z") {
				parseKeyInfoF32(value, data.mValueZ);
			}
		}
		return true;
	}

	bool readTEVInfo(TEVInfo& info)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent == 0 && (line == "TEV" || line.find("TEV ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "TEVCOLREG_A") {
				if (!readTEVColReg(info.mTevColourRegA))
					return false;
			} else if (key == "TEVCOLREG_B") {
				if (!readTEVColReg(info.mTevColourRegB))
					return false;
			} else if (key == "TEVCOLREG_C") {
				if (!readTEVColReg(info.mTevColourRegC))
					return false;
			} else if (key == "KONST_COL_A") {
				parseColour(value, info.mKonstColourA);
			} else if (key == "KONST_COL_B") {
				parseColour(value, info.mKonstColourB);
			} else if (key == "KONST_COL_C") {
				parseColour(value, info.mKonstColourC);
			} else if (key == "KONST_COL_D") {
				parseColour(value, info.mKonstColourD);
			} else if (key == "TEVSTAGE_COUNT") {
				u32 count = std::stoul(value);
				info.mTevStages.clear();
				info.mTevStages.reserve(count);
			} else if (key == "TEVSTAGE_ENTRY") {
				TEVStage stage;
				if (!readTEVStage(stage))
					return false;
				info.mTevStages.push_back(stage);
			}
		}
		return true;
	}

	bool readTEVColReg(TEVColReg& reg)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 1 && (line.find("TEVCOLREG_") == 0 || line.find("KONST_COL_") == 0 || line == "TEVSTAGE_COUNT")) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "COLOUR") {
				parseColour(value, reg.mColour);
			} else if (key == "ANIM_LENGTH") {
				reg.mAnimLength = std::stoi(value);
			} else if (key == "ANIM_SPEED") {
				reg.mAnimSpeed = std::stof(value);
			} else if (key == "COLOR_ANIM_COUNT") {
				u32 count = std::stoul(value);
				reg.mColorAnimInfo.clear();
				reg.mColorAnimInfo.reserve(count);
			} else if (key == "COLOR_ANIM_ENTRY") {
				PVWAnimInfo_3_S10 animInfo;
				if (!readPVWAnimInfo_3_S10(animInfo))
					return false;
				reg.mColorAnimInfo.push_back(animInfo);
			} else if (key == "ALPHA_ANIM_COUNT") {
				u32 count = std::stoul(value);
				reg.mAlphaAnimInfo.clear();
				reg.mAlphaAnimInfo.reserve(count);
			} else if (key == "ALPHA_ANIM_ENTRY") {
				PVWAnimInfo_1_S10 animInfo;
				if (!readPVWAnimInfo_1_S10(animInfo))
					return false;
				reg.mAlphaAnimInfo.push_back(animInfo);
			}
		}
		return true;
	}

	bool readPVWAnimInfo_1_S10(PVWAnimInfo_1_S10& info)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 3
			    && (line.find("ANIM_ENTRY") != std::string::npos || line.find("ANIM_COUNT") != std::string::npos
			        || line == "TEVCOLREG_B") || line.find("KONST_COL") == 0) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "KEYFRAME_COUNT") {
				info.mKeyframeCount = std::stoi(value);
			} else if (key == "KEYFRAME_INFO") {
				parseKeyInfoS10(value, info.mKeyframeInfo);
			}
		}
		return true;
	}

	bool readPVWAnimInfo_3_S10(PVWAnimInfo_3_S10& info)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 3
			    && (line.find("ANIM_ENTRY") != std::string::npos || line.find("ANIM_COUNT") != std::string::npos
			        || line == "TEVCOLREG_B")) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "KEYFRAME_COUNT") {
				info.mKeyframeCount = std::stoi(value);
			} else if (key == "KEYFRAME_A") {
				parseKeyInfoS10(value, info.mKeyframeA);
			} else if (key == "KEYFRAME_B") {
				parseKeyInfoS10(value, info.mKeyframeB);
			} else if (key == "KEYFRAME_C") {
				parseKeyInfoS10(value, info.mKeyframeC);
			}
		}
		return true;
	}

	bool readTEVStage(TEVStage& stage)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 2
			    && (line == "TEVSTAGE_ENTRY" || line.find("TEVSTAGE_ENTRY ") == 0 || line == "TEV" || line.find("TEV ") == 0)) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "UNKNOWN") {
				stage.mUnknown = static_cast<u8>(std::stoi(value));
			} else if (key == "TEX_COORD_ID") {
				stage.mTexCoordID = parseGXTexCoordID(value);
			} else if (key == "TEX_MAP_ID") {
				stage.mTexMapID = static_cast<u8>(parseGXTexMapID(value));
			} else if (key == "GX_CHANNEL_ID") {
				stage.mGXChannelID = parseGXChannelID(value);
			} else if (key == "K_COLOR_SEL") {
				stage.mKColorSel = parseGXTevKColorSel(value);
			} else if (key == "K_ALPHA_SEL") {
				stage.mKAlphaSel = parseGXTevKAlphaSel(value);
			} else if (key == "TEV_COLOR_COMBINER") {
				if (!readPVWCombiner(stage.mTevColorCombiner))
					return false;
			} else if (key == "TEV_ALPHA_COMBINER") {
				if (!readPVWCombiner(stage.mTevAlphaCombiner))
					return false;
			}
		}
		return true;
	}

	bool readPVWCombiner(PVWCombiner& combiner)
	{
		while (true) {
			std::streampos pos = m_in.tellg();
			if (!readLine())
				break;

			std::string line = trimLine();
			if (line.empty())
				continue;

			if (m_currentIndent <= 3 && (line == "TEV_ALPHA_COMBINER" || line == "TEVSTAGE_ENTRY" || line.find("TEVSTAGE_ENTRY ") == 0)
			    || line.find("TEV ") == 0) {
				m_in.clear();
				m_in.seekg(pos);
				break;
			}

			auto [key, value] = parseKeyValue();

			if (key == "INPUT_A") {
				combiner.mInputABCD[0] = parseGXTevColorArg(value);
			} else if (key == "INPUT_B") {
				combiner.mInputABCD[1] = parseGXTevColorArg(value);
			} else if (key == "INPUT_C") {
				combiner.mInputABCD[2] = parseGXTevColorArg(value);
			} else if (key == "INPUT_D") {
				combiner.mInputABCD[3] = parseGXTevColorArg(value);
			} else if (key == "OP") {
				combiner.mOp = static_cast<u8>(std::stoi(value));
			} else if (key == "BIAS") {
				combiner.mBias = static_cast<u8>(std::stoi(value));
			} else if (key == "SCALE") {
				combiner.mScale = static_cast<u8>(std::stoi(value));
			} else if (key == "CLAMP") {
				combiner.mClamp = static_cast<u8>(std::stoi(value));
			} else if (key == "OUT_REG") {
				combiner.mOutReg = static_cast<u8>(std::stoi(value));
			} else if (key == "UNKNOWN_A") {
				combiner._unused[0] = static_cast<u8>(std::stoi(value));
			} else if (key == "UNKNOWN_B") {
				combiner._unused[1] = static_cast<u8>(std::stoi(value));
			} else if (key == "UNKNOWN_C") {
				combiner._unused[2] = static_cast<u8>(std::stoi(value));
			}
		}
		return true;
	}
};

#endif // MATERIAL_WRITER_HPP