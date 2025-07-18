#ifndef MATERIAL_SERIALIZER_HPP
#define MATERIAL_SERIALIZER_HPP

#pragma once

#include "serialization_base.hpp"
#include "serialization_utils.hpp"
#include "common.hpp" // Your existing material structures
#include <unordered_map>

using namespace serialization;

namespace mat {

// Enum conversion helpers
namespace EnumConverters {

// Material flags converter
inline std::vector<std::pair<u32, std::string>> getMaterialFlagNames()
{
	return { { static_cast<u32>(MaterialFlags::IsEnabled), "IsEnabled" },
		     { static_cast<u32>(MaterialFlags::Opaque), "Opaque" },
		     { static_cast<u32>(MaterialFlags::AlphaClip), "AlphaClip" },
		     { static_cast<u32>(MaterialFlags::TransparentBlend), "TransparentBlend" },
		     { static_cast<u32>(MaterialFlags::InvertSpecialBlend), "InvertSpecialBlend" },
		     { static_cast<u32>(MaterialFlags::Hidden), "Hidden" } };
}

inline std::unordered_map<std::string, u32> getMaterialFlagMap()
{
	std::unordered_map<std::string, u32> map;
	for (const auto& [flag, name] : getMaterialFlagNames()) {
		map[name] = flag;
	}
	return map;
}

// Lighting flags converter
inline std::vector<std::pair<u32, std::string>> getLightingFlagNames()
{
	return { { static_cast<u32>(LightingInfoFlags::EnableColor0), "EnableColor0" },
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
		     { static_cast<u32>(LightingInfoFlags::MatSrcAlpha0Vtx), "MatSrcAlpha0Vtx" } };
}

inline std::unordered_map<std::string, u32> getLightingFlagMap()
{
	std::unordered_map<std::string, u32> map;
	for (const auto& [flag, name] : getLightingFlagNames()) {
		map[name] = flag;
	}
	return map;
}

// GX enum converters
inline std::string GXTexWrapModeToString(int mode)
{
	switch (mode) {
	case 0:
		return "GX_CLAMP";
	case 1:
		return "GX_REPEAT";
	case 2:
		return "GX_MIRROR";
	case 3:
		return "GX_MAX_TEXWRAPMODE";
	default:
		return std::to_string(mode);
	}
}

inline int StringToGXTexWrapMode(const std::string& str)
{
	if (str == "GX_CLAMP")
		return 0;
	if (str == "GX_REPEAT")
		return 1;
	if (str == "GX_MIRROR")
		return 2;
	if (str == "GX_MAX_TEXWRAPMODE")
		return 3;
	try {
		return std::stoi(str);
	} catch (...) {
		return 0;
	}
}

inline std::string GXTexCoordIDToString(int id)
{
	switch (id) {
	case 0x0:
		return "GX_TEXCOORD0";
	case 0x1:
		return "GX_TEXCOORD1";
	case 0x2:
		return "GX_TEXCOORD2";
	case 0x3:
		return "GX_TEXCOORD3";
	case 0x4:
		return "GX_TEXCOORD4";
	case 0x5:
		return "GX_TEXCOORD5";
	case 0x6:
		return "GX_TEXCOORD6";
	case 0x7:
		return "GX_TEXCOORD7";
	case 8:
		return "GX_MAX_TEXCOORD";
	case 0xff:
		return "GX_TEXCOORD_NULL";
	default:
		return std::to_string(id);
	}
}

inline int StringToGXTexCoordID(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0xff;
	}
}

inline std::string GXTexGenTypeToString(int type)
{
	switch (type) {
	case 0:
		return "GX_TG_MTX3x4";
	case 1:
		return "GX_TG_MTX2x4";
	case 2:
		return "GX_TG_BUMP0";
	case 3:
		return "GX_TG_BUMP1";
	case 4:
		return "GX_TG_BUMP2";
	case 5:
		return "GX_TG_BUMP3";
	case 6:
		return "GX_TG_BUMP4";
	case 7:
		return "GX_TG_BUMP5";
	case 8:
		return "GX_TG_BUMP6";
	case 9:
		return "GX_TG_BUMP7";
	case 10:
		return "GX_TG_SRTG";
	default:
		return std::to_string(type);
	}
}

inline int StringToGXTexGenType(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0;
	}
}

inline std::string GXTexGenSrcToString(int src)
{
	switch (src) {
	case 0:
		return "GX_TG_POS";
	case 1:
		return "GX_TG_NRM";
	case 2:
		return "GX_TG_BINRM";
	case 3:
		return "GX_TG_TANGENT";
	case 4:
		return "GX_TG_TEX0";
	case 5:
		return "GX_TG_TEX1";
	case 6:
		return "GX_TG_TEX2";
	case 7:
		return "GX_TG_TEX3";
	case 8:
		return "GX_TG_TEX4";
	case 9:
		return "GX_TG_TEX5";
	case 10:
		return "GX_TG_TEX6";
	case 11:
		return "GX_TG_TEX7";
	case 12:
		return "GX_TG_TEXCOORD0";
	case 13:
		return "GX_TG_TEXCOORD1";
	case 14:
		return "GX_TG_TEXCOORD2";
	case 15:
		return "GX_TG_TEXCOORD3";
	case 16:
		return "GX_TG_TEXCOORD4";
	case 17:
		return "GX_TG_TEXCOORD5";
	case 18:
		return "GX_TG_TEXCOORD6";
	case 19:
		return "GX_TG_COLOR0";
	case 20:
		return "GX_TG_COLOR1";
	default:
		return std::to_string(src);
	}
}

inline int StringToGXTexGenSrc(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0;
	}
}

inline std::string GXTexMtxToString(int mtx)
{
	switch (mtx) {
	case 0:
		return "GX_TEXMTX0";
	case 1:
		return "GX_TEXMTX1";
	case 2:
		return "GX_TEXMTX2";
	case 3:
		return "GX_TEXMTX3";
	case 4:
		return "GX_TEXMTX4";
	case 5:
		return "GX_TEXMTX5";
	case 6:
		return "GX_TEXMTX6";
	case 7:
		return "GX_TEXMTX7";
	case 8:
		return "GX_TEXMTX8";
	case 9:
		return "GX_TEXMTX9";
	case 0xFF:
		return "GX_IDENTITY";
	default:
		return std::to_string(mtx);
	}
}

inline int StringToGXTexMtx(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0xFF;
	}
}

inline std::string GXTexMapIDToString(int id)
{
	switch (id) {
	case 0:
		return "GX_TEXMAP0";
	case 1:
		return "GX_TEXMAP1";
	case 2:
		return "GX_TEXMAP2";
	case 3:
		return "GX_TEXMAP3";
	case 4:
		return "GX_TEXMAP4";
	case 5:
		return "GX_TEXMAP5";
	case 6:
		return "GX_TEXMAP6";
	case 7:
		return "GX_TEXMAP7";
	case 8:
		return "GX_MAX_TEXMAP";
	case 0xff:
		return "GX_TEXMAP_NULL";
	case 0x100:
		return "GX_TEX_DISABLE";
	default:
		return std::to_string(id);
	}
}

inline int StringToGXTexMapID(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0xff;
	}
}

inline std::string GXChannelIDToString(int id)
{
	switch (id) {
	case 0:
		return "GX_COLOR0";
	case 1:
		return "GX_COLOR1";
	case 2:
		return "GX_ALPHA0";
	case 3:
		return "GX_ALPHA1";
	case 4:
		return "GX_COLOR0A0";
	case 5:
		return "GX_COLOR1A1";
	case 6:
		return "GX_COLOR_ZERO";
	case 7:
		return "GX_ALPHA_BUMP";
	case 8:
		return "GX_ALPHA_BUMPN";
	case 0xff:
		return "GX_COLOR_NULL";
	default:
		return std::to_string(id);
	}
}

inline int StringToGXChannelID(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0xff;
	}
}

inline std::string GXTevKColorSelToString(int sel)
{
	switch (sel) {
	case 0x00:
		return "GX_TEV_KCSEL_1";
	case 0x01:
		return "GX_TEV_KCSEL_7_8";
	case 0x02:
		return "GX_TEV_KCSEL_3_4";
	case 0x03:
		return "GX_TEV_KCSEL_5_8";
	case 0x04:
		return "GX_TEV_KCSEL_1_2";
	case 0x05:
		return "GX_TEV_KCSEL_3_8";
	case 0x06:
		return "GX_TEV_KCSEL_1_4";
	case 0x07:
		return "GX_TEV_KCSEL_1_8";
	case 0x0C:
		return "GX_TEV_KCSEL_K0";
	case 0x0D:
		return "GX_TEV_KCSEL_K1";
	case 0x0E:
		return "GX_TEV_KCSEL_K2";
	case 0x0F:
		return "GX_TEV_KCSEL_K3";
	case 0x10:
		return "GX_TEV_KCSEL_K0_R";
	case 0x11:
		return "GX_TEV_KCSEL_K1_R";
	case 0x12:
		return "GX_TEV_KCSEL_K2_R";
	case 0x13:
		return "GX_TEV_KCSEL_K3_R";
	case 0x14:
		return "GX_TEV_KCSEL_K0_G";
	case 0x15:
		return "GX_TEV_KCSEL_K1_G";
	case 0x16:
		return "GX_TEV_KCSEL_K2_G";
	case 0x17:
		return "GX_TEV_KCSEL_K3_G";
	case 0x18:
		return "GX_TEV_KCSEL_K0_B";
	case 0x19:
		return "GX_TEV_KCSEL_K1_B";
	case 0x1A:
		return "GX_TEV_KCSEL_K2_B";
	case 0x1B:
		return "GX_TEV_KCSEL_K3_B";
	case 0x1C:
		return "GX_TEV_KCSEL_K0_A";
	case 0x1D:
		return "GX_TEV_KCSEL_K1_A";
	case 0x1E:
		return "GX_TEV_KCSEL_K2_A";
	case 0x1F:
		return "GX_TEV_KCSEL_K3_A";
	default:
		return std::to_string(sel);
	}
}

inline int StringToGXTevKColorSel(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0x1F;
	}
}

inline std::string GXTevKAlphaSelToString(int sel)
{
	switch (sel) {
	case 0x00:
		return "GX_TEV_KASEL_1";
	case 0x01:
		return "GX_TEV_KASEL_7_8";
	case 0x02:
		return "GX_TEV_KASEL_3_4";
	case 0x03:
		return "GX_TEV_KASEL_5_8";
	case 0x04:
		return "GX_TEV_KASEL_1_2";
	case 0x05:
		return "GX_TEV_KASEL_3_8";
	case 0x06:
		return "GX_TEV_KASEL_1_4";
	case 0x07:
		return "GX_TEV_KASEL_1_8";
	case 0x10:
		return "GX_TEV_KASEL_K0_R";
	case 0x11:
		return "GX_TEV_KASEL_K1_R";
	case 0x12:
		return "GX_TEV_KASEL_K2_R";
	case 0x13:
		return "GX_TEV_KASEL_K3_R";
	case 0x14:
		return "GX_TEV_KASEL_K0_G";
	case 0x15:
		return "GX_TEV_KASEL_K1_G";
	case 0x16:
		return "GX_TEV_KASEL_K2_G";
	case 0x17:
		return "GX_TEV_KASEL_K3_G";
	case 0x18:
		return "GX_TEV_KASEL_K0_B";
	case 0x19:
		return "GX_TEV_KASEL_K1_B";
	case 0x1A:
		return "GX_TEV_KASEL_K2_B";
	case 0x1B:
		return "GX_TEV_KASEL_K3_B";
	case 0x1C:
		return "GX_TEV_KASEL_K0_A";
	case 0x1D:
		return "GX_TEV_KASEL_K1_A";
	case 0x1E:
		return "GX_TEV_KASEL_K2_A";
	case 0x1F:
		return "GX_TEV_KASEL_K3_A";
	default:
		return std::to_string(sel);
	}
}

inline int StringToGXTevKAlphaSel(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 0x1F;
	}
}

inline std::string GXTevColorArgToString(int arg)
{
	switch (arg) {
	case 0:
		return "GX_CC_CPREV";
	case 1:
		return "GX_CC_APREV";
	case 2:
		return "GX_CC_C0";
	case 3:
		return "GX_CC_A0";
	case 4:
		return "GX_CC_C1";
	case 5:
		return "GX_CC_A1";
	case 6:
		return "GX_CC_C2";
	case 7:
		return "GX_CC_A2";
	case 8:
		return "GX_CC_TEXC";
	case 9:
		return "GX_CC_TEXA";
	case 10:
		return "GX_CC_RASC";
	case 11:
		return "GX_CC_RASA";
	case 12:
		return "GX_CC_ONE";
	case 13:
		return "GX_CC_HALF";
	case 14:
		return "GX_CC_QUARTER";
	case 15:
		return "GX_CC_ZERO";
	case 16:
		return "GX_CC_TEXRRR";
	case 17:
		return "GX_CC_TEXGGG";
	case 18:
		return "GX_CC_TEXBBB";
	default:
		return std::to_string(arg);
	}
}

inline int StringToGXTevColorArg(const std::string& str)
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
	try {
		return std::stoi(str);
	} catch (...) {
		return 15;
	}
}
} // namespace EnumConverters

// Serializable wrappers for material structures

class SerializableKeyInfoU8 : public ISerializable {
public:
	KeyInfoU8 data;

	void serialize(ISerializer& s) const override
	{
		s.write("time", static_cast<int>(data.mTime));
		s.write("value", data.mValue);
		s.write("tangent", data.mTangent);
	}

	void deserialize(IDeserializer& d) override
	{
		int time = 0;
		d.read("time", time);
		data.mTime = static_cast<u8>(time);
		d.read("value", data.mValue);
		d.read("tangent", data.mTangent);
	}
};

class SerializableKeyInfoF32 : public ISerializable {
public:
	KeyInfoF32 data;

	void serialize(ISerializer& s) const override
	{
		s.write("time", data.mTime);
		s.write("value", data.mValue);
		s.write("tangent", data.mTangent);
	}

	void deserialize(IDeserializer& d) override
	{
		d.read("time", data.mTime);
		d.read("value", data.mValue);
		d.read("tangent", data.mTangent);
	}
};

class SerializableKeyInfoS10 : public ISerializable {
public:
	KeyInfoS10 data;

	void serialize(ISerializer& s) const override
	{
		s.write("time", static_cast<int>(data.mTime));
		s.write("value", data.mValue);
		s.write("tangent", data.mTangent);
	}

	void deserialize(IDeserializer& d) override
	{
		int time = 0;
		d.read("time", time);
		data.mTime = static_cast<s16>(time);
		d.read("value", data.mValue);
		d.read("tangent", data.mTangent);
	}
};

class SerializableColourAnimInfo : public ISerializable {
public:
	ColourAnimInfo data;

	void serialize(ISerializer& s) const override
	{
		s.write("index", data.mIndex);

		s.beginObject("red_keyframe");
		SerializableKeyInfoU8 red;
		red.data = data.mKeyDataR;
		red.serialize(s);
		s.endObject();

		s.beginObject("green_keyframe");
		SerializableKeyInfoU8 green;
		green.data = data.mKeyDataG;
		green.serialize(s);
		s.endObject();

		s.beginObject("blue_keyframe");
		SerializableKeyInfoU8 blue;
		blue.data = data.mKeyDataB;
		blue.serialize(s);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		d.read("index", data.mIndex);

		if (d.enterObject("red_keyframe")) {
			SerializableKeyInfoU8 red;
			red.deserialize(d);
			data.mKeyDataR = red.data;
			d.exitObject();
		}

		if (d.enterObject("green_keyframe")) {
			SerializableKeyInfoU8 green;
			green.deserialize(d);
			data.mKeyDataG = green.data;
			d.exitObject();
		}

		if (d.enterObject("blue_keyframe")) {
			SerializableKeyInfoU8 blue;
			blue.deserialize(d);
			data.mKeyDataB = blue.data;
			d.exitObject();
		}
	}
};

class SerializableAlphaAnimInfo : public ISerializable {
public:
	AlphaAnimInfo data;

	void serialize(ISerializer& s) const override
	{
		s.write("index", data.mIndex);

		s.beginObject("alpha_keyframe");
		SerializableKeyInfoU8 alpha;
		alpha.data = data.mKeyData;
		alpha.serialize(s);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		d.read("index", data.mIndex);

		if (d.enterObject("alpha_keyframe")) {
			SerializableKeyInfoU8 alpha;
			alpha.deserialize(d);
			data.mKeyData = alpha.data;
			d.exitObject();
		}
	}
};

class SerializableTextureAnimData : public ISerializable {
public:
	TextureAnimData data;

	void serialize(ISerializer& s) const override
	{
		s.write("frame", data.mAnimationFrame);

		s.beginObject("value_x");
		s.write("time", data.mValueX.mTime);
		s.write("value", data.mValueX.mValue);
		s.write("tangent", data.mValueX.mTangent);
		s.endObject();

		s.beginObject("value_y");
		s.write("time", data.mValueY.mTime);
		s.write("value", data.mValueY.mValue);
		s.write("tangent", data.mValueY.mTangent);
		s.endObject();

		s.beginObject("value_z");
		s.write("time", data.mValueZ.mTime);
		s.write("value", data.mValueZ.mValue);
		s.write("tangent", data.mValueZ.mTangent);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		d.read("frame", data.mAnimationFrame);

		if (d.enterObject("value_x")) {
			d.read("time", data.mValueX.mTime);
			d.read("value", data.mValueX.mValue);
			d.read("tangent", data.mValueX.mTangent);
			d.exitObject();
		}

		if (d.enterObject("value_y")) {
			d.read("time", data.mValueY.mTime);
			d.read("value", data.mValueY.mValue);
			d.read("tangent", data.mValueY.mTangent);
			d.exitObject();
		}

		if (d.enterObject("value_z")) {
			d.read("time", data.mValueZ.mTime);
			d.read("value", data.mValueZ.mValue);
			d.read("tangent", data.mValueZ.mTangent);
			d.exitObject();
		}
	}
};

class SerializablePVWAnimInfo_1_S10 : public ISerializable {
public:
	PVWAnimInfo_1_S10 data;

	void serialize(ISerializer& s) const override
	{
		s.write("keyframe_count", data.mKeyframeCount);

		s.beginObject("keyframe_info");
		SerializableKeyInfoS10 key;
		key.data = data.mKeyframeInfo;
		key.serialize(s);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		d.read("keyframe_count", data.mKeyframeCount);

		if (d.enterObject("keyframe_info")) {
			SerializableKeyInfoS10 key;
			key.deserialize(d);
			data.mKeyframeInfo = key.data;
			d.exitObject();
		}
	}
};

class SerializablePVWAnimInfo_3_S10 : public ISerializable {
public:
	PVWAnimInfo_3_S10 data;

	void serialize(ISerializer& s) const override
	{
		s.write("keyframe_count", data.mKeyframeCount);

		s.beginObject("keyframe_a");
		SerializableKeyInfoS10 keyA;
		keyA.data = data.mKeyframeA;
		keyA.serialize(s);
		s.endObject();

		s.beginObject("keyframe_b");
		SerializableKeyInfoS10 keyB;
		keyB.data = data.mKeyframeB;
		keyB.serialize(s);
		s.endObject();

		s.beginObject("keyframe_c");
		SerializableKeyInfoS10 keyC;
		keyC.data = data.mKeyframeC;
		keyC.serialize(s);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		d.read("keyframe_count", data.mKeyframeCount);

		if (d.enterObject("keyframe_a")) {
			SerializableKeyInfoS10 key;
			key.deserialize(d);
			data.mKeyframeA = key.data;
			d.exitObject();
		}

		if (d.enterObject("keyframe_b")) {
			SerializableKeyInfoS10 key;
			key.deserialize(d);
			data.mKeyframeB = key.data;
			d.exitObject();
		}

		if (d.enterObject("keyframe_c")) {
			SerializableKeyInfoS10 key;
			key.deserialize(d);
			data.mKeyframeC = key.data;
			d.exitObject();
		}
	}
};

class SerializablePolygonColourInfo : public ISerializable {
public:
	PolygonColourInfo data;

	void serialize(ISerializer& s) const override
	{
		serializeColor(s, "diffuse_color", data.mDiffuseColour);
		s.write("anim_length", data.mAnimLength);
		s.write("anim_speed", data.mAnimSpeed);

		// Serialize color animations
		s.beginArray("color_animations");
		for (const auto& colAnim : data.mColourAnimInfo) {
			SerializableColourAnimInfo wrapper;
			wrapper.data = colAnim;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();

		// Serialize alpha animations
		s.beginArray("alpha_animations");
		for (const auto& alphaAnim : data.mAlphaAnimInfo) {
			SerializableAlphaAnimInfo wrapper;
			wrapper.data = alphaAnim;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		deserializeColor(d, "diffuse_color", data.mDiffuseColour);
		d.read("anim_length", data.mAnimLength);
		d.read("anim_speed", data.mAnimSpeed);

		// Fixed color animations deserialization
		data.mColourAnimInfo.clear();
		if (d.enterArray("color_animations")) {
			size_t count = d.getArraySize();
			data.mColourAnimInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableColourAnimInfo wrapper;
					wrapper.deserialize(d);
					data.mColourAnimInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// Fixed alpha animations deserialization
		data.mAlphaAnimInfo.clear();
		if (d.enterArray("alpha_animations")) {
			size_t count = d.getArraySize();
			data.mAlphaAnimInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableAlphaAnimInfo wrapper;
					wrapper.deserialize(d);
					data.mAlphaAnimInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

class SerializableLightingInfo : public ISerializable {
public:
	LightingInfo data;

	void serialize(ISerializer& s) const override
	{
		auto flags = EnumConverters::getLightingFlagNames();

		std::ostringstream oss;
		oss << "Options: ";
		for (size_t i = 0; i < flags.size(); ++i) {
			oss << flags[i].second;
			if (i != flags.size() - 1) {
				oss << ", ";
			}
		}
		s.writeComment(oss.str());

		serializeFlags(s, "flags", data.mFlags, flags);
		s.write("unknown", data.mUnknown);
	}

	void deserialize(IDeserializer& d) override
	{
		deserializeFlags(d, "flags", data.mFlags, EnumConverters::getLightingFlagMap());
		d.read("unknown", data.mUnknown);
	}
};

class SerializablePeInfo : public ISerializable {
public:
	PeInfo data;

	void serialize(ISerializer& s) const override
	{
		s.writeU32("flags", data.mFlags);

		s.beginObject("alpha_compare_function");
		s.write("comp0", static_cast<int>(data.mAlphaCompareFunction.bits.comp0));
		s.write("ref0", static_cast<int>(data.mAlphaCompareFunction.bits.ref0));
		s.write("op", static_cast<int>(data.mAlphaCompareFunction.bits.op));
		s.write("comp1", static_cast<int>(data.mAlphaCompareFunction.bits.comp1));
		s.write("ref1", static_cast<int>(data.mAlphaCompareFunction.bits.ref1));
		s.endObject();

		s.writeU32("z_mode_function", data.mZModeFunction);

		s.beginObject("blend_mode");
		s.write("type", static_cast<int>(data.mBlendMode.bits.mType));
		s.write("src_factor", static_cast<int>(data.mBlendMode.bits.mSrcFactor));
		s.write("dst_factor", static_cast<int>(data.mBlendMode.bits.mDstFactor));
		s.write("logic_op", static_cast<int>(data.mBlendMode.bits.mLogicOp));
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		d.readU32("flags", data.mFlags);

		if (d.enterObject("alpha_compare_function")) {
			int comp0, ref0, op, comp1, ref1;
			d.read("comp0", comp0);
			d.read("ref0", ref0);
			d.read("op", op);
			d.read("comp1", comp1);
			d.read("ref1", ref1);
			data.mAlphaCompareFunction.bits.comp0 = comp0;
			data.mAlphaCompareFunction.bits.ref0  = ref0;
			data.mAlphaCompareFunction.bits.op    = op;
			data.mAlphaCompareFunction.bits.comp1 = comp1;
			data.mAlphaCompareFunction.bits.ref1  = ref1;
			d.exitObject();
		}

		d.readU32("z_mode_function", data.mZModeFunction);

		if (d.enterObject("blend_mode")) {
			int type, srcFactor, dstFactor, logicOp;
			d.read("type", type);
			d.read("src_factor", srcFactor);
			d.read("dst_factor", dstFactor);
			d.read("logic_op", logicOp);
			data.mBlendMode.bits.mType      = type;
			data.mBlendMode.bits.mSrcFactor = srcFactor;
			data.mBlendMode.bits.mDstFactor = dstFactor;
			data.mBlendMode.bits.mLogicOp   = logicOp;
			d.exitObject();
		}
	}
};

class SerializableTexGenData : public ISerializable {
public:
	TexGenData data;

	void serialize(ISerializer& s) const override
	{
		s.write("destination_coords", EnumConverters::GXTexCoordIDToString(data.mDestinationCoords));
		s.write("func", EnumConverters::GXTexGenTypeToString(data.mFunc));
		s.write("source_param", EnumConverters::GXTexGenSrcToString(data.mSourceParam));
		s.write("texture_mtx", EnumConverters::GXTexMtxToString(data.mTexMtx));
	}

	void deserialize(IDeserializer& d) override
	{
		std::string str;
		if (d.read("destination_coords", str)) {
			data.mDestinationCoords = EnumConverters::StringToGXTexCoordID(str);
		}
		if (d.read("func", str)) {
			data.mFunc = EnumConverters::StringToGXTexGenType(str);
		}
		if (d.read("source_param", str)) {
			data.mSourceParam = EnumConverters::StringToGXTexGenSrc(str);
		}
		if (d.read("texture_mtx", str)) {
			data.mTexMtx = EnumConverters::StringToGXTexMtx(str);
		}
	}
};

class SerializableTextureData : public ISerializable {
public:
	TextureData data;

	void serialize(ISerializer& s) const override
	{
		s.write("texture_attr_idx", data.mTextureAttributeIndex);
		s.write("wrap_mode_s", EnumConverters::GXTexWrapModeToString(data.mWrapModeS));
		s.write("wrap_mode_t", EnumConverters::GXTexWrapModeToString(data.mWrapModeT));
		s.write("unknown3", static_cast<int>(data.mUnknown3));
		s.write("unknown4", static_cast<int>(data.mUnknown4));
		s.write("unknown5", static_cast<int>(data.mUnknown5));
		s.write("unknown6", static_cast<int>(data.mUnknown6));
		s.write("animation_factor", data.mAnimationFactor);
		s.write("anim_length", data.mAnimLength);
		s.write("anim_speed", data.mAnimSpeed);

		serializeVec2(s, "scale", data.mScale);
		s.write("rotation", data.mRotation);
		serializeVec2(s, "pivot", data.mPivot);
		serializeVec2(s, "position", data.mPosition);

		// Serialize animation data
		s.writeComment("Scale animation frames");
		s.beginArray("scale_animations");
		for (const auto& anim : data.mScaleInfo) {
			SerializableTextureAnimData wrapper;
			wrapper.data = anim;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();

		s.writeComment("Rotation animation frames");
		s.beginArray("rotation_animations");
		for (const auto& anim : data.mRotationInfo) {
			SerializableTextureAnimData wrapper;
			wrapper.data = anim;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();

		s.writeComment("Translation animation frames");
		s.beginArray("translation_animations");
		for (const auto& anim : data.mTranslationInfo) {
			SerializableTextureAnimData wrapper;
			wrapper.data = anim;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		d.read("texture_attr_idx", data.mTextureAttributeIndex);

		std::string wrapS, wrapT;
		if (d.read("wrap_mode_s", wrapS)) {
			data.mWrapModeS = EnumConverters::StringToGXTexWrapMode(wrapS);
		}
		if (d.read("wrap_mode_t", wrapT)) {
			data.mWrapModeT = EnumConverters::StringToGXTexWrapMode(wrapT);
		}

		int unknown;
		if (d.read("unknown3", unknown))
			data.mUnknown3 = static_cast<u8>(unknown);
		if (d.read("unknown4", unknown))
			data.mUnknown4 = static_cast<u8>(unknown);
		if (d.read("unknown5", unknown))
			data.mUnknown5 = static_cast<u8>(unknown);
		if (d.read("unknown6", unknown))
			data.mUnknown6 = static_cast<u8>(unknown);

		d.read("animation_factor", data.mAnimationFactor);
		d.read("anim_length", data.mAnimLength);
		d.read("anim_speed", data.mAnimSpeed);

		deserializeVec2(d, "scale", data.mScale);
		d.read("rotation", data.mRotation);
		deserializeVec2(d, "pivot", data.mPivot);
		deserializeVec2(d, "position", data.mPosition);

		// FIXED: Deserialize scale animations
		data.mScaleInfo.clear();
		if (d.enterArray("scale_animations")) {
			size_t count = d.getArraySize();
			data.mScaleInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableTextureAnimData wrapper;
					wrapper.deserialize(d);
					data.mScaleInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// FIXED: Deserialize rotation animations
		data.mRotationInfo.clear();
		if (d.enterArray("rotation_animations")) {
			size_t count = d.getArraySize();
			data.mRotationInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableTextureAnimData wrapper;
					wrapper.deserialize(d);
					data.mRotationInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// FIXED: Deserialize translation animations
		data.mTranslationInfo.clear();
		if (d.enterArray("translation_animations")) {
			size_t count = d.getArraySize();
			data.mTranslationInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableTextureAnimData wrapper;
					wrapper.deserialize(d);
					data.mTranslationInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

class SerializableTextureInfo : public ISerializable {
public:
	TextureInfo data;

	void serialize(ISerializer& s) const override
	{
		s.writeU32("use_scale", data.mUseScale);
		serializeVec3(s, "scale", data.mScale);

		s.beginArray("texture_gen_data");
		for (const auto& genData : data.mTextureGenData) {
			SerializableTexGenData wrapper;
			wrapper.data = genData;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();

		s.beginArray("texture_data");
		for (const auto& texData : data.mTextureData) {
			SerializableTextureData wrapper;
			wrapper.data = texData;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		d.readU32("use_scale", data.mUseScale);
		deserializeVec3(d, "scale", data.mScale);

		// FIXED: Deserialize texture gen data
		data.mTextureGenData.clear();
		if (d.enterArray("texture_gen_data")) {
			size_t count = d.getArraySize();
			data.mTextureGenData.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableTexGenData wrapper;
					wrapper.deserialize(d);
					data.mTextureGenData.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// FIXED: Deserialize texture data
		data.mTextureData.clear();
		if (d.enterArray("texture_data")) {
			size_t count = d.getArraySize();
			data.mTextureData.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableTextureData wrapper;
					wrapper.deserialize(d);
					data.mTextureData.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

class SerializablePVWCombiner : public ISerializable {
public:
	PVWCombiner data;

	void serialize(ISerializer& s) const override
	{
		s.write("input_a", EnumConverters::GXTevColorArgToString(data.mInputABCD[0]));
		s.write("input_b", EnumConverters::GXTevColorArgToString(data.mInputABCD[1]));
		s.write("input_c", EnumConverters::GXTevColorArgToString(data.mInputABCD[2]));
		s.write("input_d", EnumConverters::GXTevColorArgToString(data.mInputABCD[3]));
		s.write("op", static_cast<int>(data.mOp));
		s.write("bias", static_cast<int>(data.mBias));
		s.write("scale", static_cast<int>(data.mScale));
		s.write("clamp", static_cast<int>(data.mClamp));
		s.write("out_reg", static_cast<int>(data.mOutReg));
		s.write("unused0", static_cast<int>(data._unused[0]));
		s.write("unused1", static_cast<int>(data._unused[1]));
		s.write("unused2", static_cast<int>(data._unused[2]));
	}

	void deserialize(IDeserializer& d) override
	{
		std::string str;
		if (d.read("input_a", str))
			data.mInputABCD[0] = EnumConverters::StringToGXTevColorArg(str);
		if (d.read("input_b", str))
			data.mInputABCD[1] = EnumConverters::StringToGXTevColorArg(str);
		if (d.read("input_c", str))
			data.mInputABCD[2] = EnumConverters::StringToGXTevColorArg(str);
		if (d.read("input_d", str))
			data.mInputABCD[3] = EnumConverters::StringToGXTevColorArg(str);

		int val;
		if (d.read("op", val))
			data.mOp = static_cast<u8>(val);
		if (d.read("bias", val))
			data.mBias = static_cast<u8>(val);
		if (d.read("scale", val))
			data.mScale = static_cast<u8>(val);
		if (d.read("clamp", val))
			data.mClamp = static_cast<u8>(val);
		if (d.read("out_reg", val))
			data.mOutReg = static_cast<u8>(val);
		if (d.read("unused0", val))
			data._unused[0] = static_cast<u8>(val);
		if (d.read("unused1", val))
			data._unused[1] = static_cast<u8>(val);
		if (d.read("unused2", val))
			data._unused[2] = static_cast<u8>(val);
	}
};

class SerializableTEVStage : public ISerializable {
public:
	TEVStage data;

	void serialize(ISerializer& s) const override
	{
		s.write("unknown", static_cast<int>(data.mUnknown));
		s.write("tex_coord_id", EnumConverters::GXTexCoordIDToString(data.mTexCoordID));
		s.write("tex_map_id", EnumConverters::GXTexMapIDToString(data.mTexMapID));
		s.write("gx_channel_id", EnumConverters::GXChannelIDToString(data.mGXChannelID));
		s.write("k_color_sel", EnumConverters::GXTevKColorSelToString(data.mKColorSel));
		s.write("k_alpha_sel", EnumConverters::GXTevKAlphaSelToString(data.mKAlphaSel));

		s.beginObject("tev_color_combiner");
		SerializablePVWCombiner colorCombiner;
		colorCombiner.data = data.mTevColorCombiner;
		colorCombiner.serialize(s);
		s.endObject();

		s.beginObject("tev_alpha_combiner");
		SerializablePVWCombiner alphaCombiner;
		alphaCombiner.data = data.mTevAlphaCombiner;
		alphaCombiner.serialize(s);
		s.endObject();
	}

	void deserialize(IDeserializer& d) override
	{
		int val;
		if (d.read("unknown", val))
			data.mUnknown = static_cast<u8>(val);

		std::string str;
		if (d.read("tex_coord_id", str))
			data.mTexCoordID = EnumConverters::StringToGXTexCoordID(str);
		if (d.read("tex_map_id", str))
			data.mTexMapID = static_cast<u8>(EnumConverters::StringToGXTexMapID(str));
		if (d.read("gx_channel_id", str))
			data.mGXChannelID = EnumConverters::StringToGXChannelID(str);
		if (d.read("k_color_sel", str))
			data.mKColorSel = EnumConverters::StringToGXTevKColorSel(str);
		if (d.read("k_alpha_sel", str))
			data.mKAlphaSel = EnumConverters::StringToGXTevKAlphaSel(str);

		if (d.enterObject("tev_color_combiner")) {
			SerializablePVWCombiner combiner;
			combiner.deserialize(d);
			data.mTevColorCombiner = combiner.data;
			d.exitObject();
		}

		if (d.enterObject("tev_alpha_combiner")) {
			SerializablePVWCombiner combiner;
			combiner.deserialize(d);
			data.mTevAlphaCombiner = combiner.data;
			d.exitObject();
		}
	}
};

class SerializableTEVColReg : public ISerializable {
public:
	TEVColReg data;

	void serialize(ISerializer& s) const override
	{
		serializeColor(s, "colour", data.mColour);
		s.write("anim_length", data.mAnimLength);
		s.write("anim_speed", data.mAnimSpeed);

		s.beginArray("color_anim_info");
		for (const auto& anim : data.mColorAnimInfo) {
			SerializablePVWAnimInfo_3_S10 wrapper;
			wrapper.data = anim;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();

		s.beginArray("alpha_anim_info");
		for (const auto& anim : data.mAlphaAnimInfo) {
			SerializablePVWAnimInfo_1_S10 wrapper;
			wrapper.data = anim;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		deserializeColor(d, "colour", data.mColour);
		d.read("anim_length", data.mAnimLength);
		d.read("anim_speed", data.mAnimSpeed);

		// FIXED: Deserialize color anim info
		data.mColorAnimInfo.clear();
		if (d.enterArray("color_anim_info")) {
			size_t count = d.getArraySize();
			data.mColorAnimInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializablePVWAnimInfo_3_S10 wrapper;
					wrapper.deserialize(d);
					data.mColorAnimInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}

		// FIXED: Deserialize alpha anim info
		data.mAlphaAnimInfo.clear();
		if (d.enterArray("alpha_anim_info")) {
			size_t count = d.getArraySize();
			data.mAlphaAnimInfo.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializablePVWAnimInfo_1_S10 wrapper;
					wrapper.deserialize(d);
					data.mAlphaAnimInfo.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

class SerializableTEVInfo : public ISerializable {
public:
	TEVInfo data;

	void serialize(ISerializer& s) const override
	{
		s.beginObject("tev_colour_reg_a");
		SerializableTEVColReg regA;
		regA.data = data.mTevColourRegA;
		regA.serialize(s);
		s.endObject();

		s.beginObject("tev_colour_reg_b");
		SerializableTEVColReg regB;
		regB.data = data.mTevColourRegB;
		regB.serialize(s);
		s.endObject();

		s.beginObject("tev_colour_reg_c");
		SerializableTEVColReg regC;
		regC.data = data.mTevColourRegC;
		regC.serialize(s);
		s.endObject();

		serializeColor(s, "konst_colour_a", data.mKonstColourA);
		serializeColor(s, "konst_colour_b", data.mKonstColourB);
		serializeColor(s, "konst_colour_c", data.mKonstColourC);
		serializeColor(s, "konst_colour_d", data.mKonstColourD);

		s.beginArray("tev_stages");
		for (const auto& stage : data.mTevStages) {
			SerializableTEVStage wrapper;
			wrapper.data = stage;
			s.beginObject("");
			wrapper.serialize(s);
			s.endObject();
		}
		s.endArray();
	}

	void deserialize(IDeserializer& d) override
	{
		if (d.enterObject("tev_colour_reg_a")) {
			SerializableTEVColReg reg;
			reg.deserialize(d);
			data.mTevColourRegA = reg.data;
			d.exitObject();
		}

		if (d.enterObject("tev_colour_reg_b")) {
			SerializableTEVColReg reg;
			reg.deserialize(d);
			data.mTevColourRegB = reg.data;
			d.exitObject();
		}

		if (d.enterObject("tev_colour_reg_c")) {
			SerializableTEVColReg reg;
			reg.deserialize(d);
			data.mTevColourRegC = reg.data;
			d.exitObject();
		}

		deserializeColor(d, "konst_colour_a", data.mKonstColourA);
		deserializeColor(d, "konst_colour_b", data.mKonstColourB);
		deserializeColor(d, "konst_colour_c", data.mKonstColourC);
		deserializeColor(d, "konst_colour_d", data.mKonstColourD);

		// FIXED: Deserialize TEV stages
		data.mTevStages.clear();
		if (d.enterArray("tev_stages")) {
			size_t count = d.getArraySize();
			data.mTevStages.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				if (d.enterObject("")) {
					SerializableTEVStage wrapper;
					wrapper.deserialize(d);
					data.mTevStages.push_back(wrapper.data);
					d.exitObject();
				}
				if (i < count - 1)
					d.nextArrayElement();
			}
			d.exitArray();
		}
	}
};

class SerializableMaterial : public ISerializable {
public:
	Material data;

	void serialize(ISerializer& s) const override
	{
		s.writeComment("Material properties");

		std::vector<std::pair<u32, std::string>> matFlags = EnumConverters::getMaterialFlagNames();

		std::ostringstream oss;
		oss << "Options: ";
		for (size_t i = 0; i < matFlags.size(); ++i) {
			oss << matFlags[i].second;
			if (i != matFlags.size() - 1) {
				oss << ", ";
			}
		}
		s.writeComment(oss.str());

		serializeFlags(s, "flags", data.mFlags, matFlags);
		s.write("texture_index", data.mTextureIndex);
		serializeColor(s, "diffuse_color", data.mColourInfo.mDiffuseColour);

		if (data.mFlags & static_cast<u32>(MaterialFlags::IsEnabled)) {
			s.writeU32("tev_group_id", data.mTevGroupId);

			s.beginObject("color_info");
			SerializablePolygonColourInfo colorInfo;
			colorInfo.data = data.mColourInfo;
			colorInfo.serialize(s);
			s.endObject();

			s.beginObject("lighting_info");
			SerializableLightingInfo lightingInfo;
			lightingInfo.data = data.mLightingInfo;
			lightingInfo.serialize(s);
			s.endObject();

			s.beginObject("pe_info");
			SerializablePeInfo peInfo;
			peInfo.data = data.mPeInfo;
			peInfo.serialize(s);
			s.endObject();

			s.beginObject("texture_info");
			SerializableTextureInfo textureInfo;
			textureInfo.data = data.mTexInfo;
			textureInfo.serialize(s);
			s.endObject();
		}
	}

	void deserialize(IDeserializer& d) override
	{
		// Deserialize flags
		deserializeFlags(d, "flags", data.mFlags, EnumConverters::getMaterialFlagMap());
		d.read("texture_index", data.mTextureIndex);
		deserializeColor(d, "diffuse_color", data.mColourInfo.mDiffuseColour);

		if (data.mFlags & static_cast<u32>(MaterialFlags::IsEnabled)) {
			d.readU32("tev_group_id", data.mTevGroupId);

			if (d.enterObject("color_info")) {
				SerializablePolygonColourInfo colorInfo;
				colorInfo.deserialize(d);
				data.mColourInfo = colorInfo.data;
				d.exitObject();
			}

			if (d.enterObject("lighting_info")) {
				SerializableLightingInfo lightingInfo;
				lightingInfo.deserialize(d);
				data.mLightingInfo = lightingInfo.data;
				d.exitObject();
			}

			if (d.enterObject("pe_info")) {
				SerializablePeInfo peInfo;
				peInfo.deserialize(d);
				data.mPeInfo = peInfo.data;
				d.exitObject();
			}

			if (d.enterObject("texture_info")) {
				SerializableTextureInfo textureInfo;
				textureInfo.deserialize(d);
				data.mTexInfo = textureInfo.data;
				d.exitObject();
			}
		}
	}
};

// Convenience functions
inline bool saveMaterialsToFile(const std::string& filename, const std::vector<Material>& materials, const std::vector<TEVInfo>& tevInfos)
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	// Use the new JSON serializer
	JsonTextSerializer serializer(file);
	serializer.beginDocument(); // Starts the root "{"

	// The file itself is one big object, so we manually add the main keys.
	serializer.beginArray("materials");
	for (const auto& mat : materials) {
		SerializableMaterial wrapper;
		wrapper.data = mat;
		// beginObject with no name for an anonymous object in an array
		serializer.beginObject("");
		wrapper.serialize(serializer);
		serializer.endObject();
	}
	serializer.endArray();

	serializer.beginArray("tev_infos");
	for (const auto& tev : tevInfos) {
		SerializableTEVInfo wrapper;
		wrapper.data = tev;
		serializer.beginObject("");
		wrapper.serialize(serializer);
		serializer.endObject();
	}
	serializer.endArray();

	serializer.endDocument(); // Closes the root "}"
	return true;
}

inline bool loadMaterialsFromFile(const std::string& filename, std::vector<Material>& materials, std::vector<TEVInfo>& tevInfos)
{
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	JsonTextDeserializer deserializer(file);
	if (!deserializer.parseDocument())
		return false;

	// FIXED: Load materials
	materials.clear();
	if (deserializer.enterArray("materials")) {
		size_t count = deserializer.getArraySize();
		materials.reserve(count);
		for (size_t i = 0; i < count; ++i) {
			if (deserializer.enterObject("")) {
				SerializableMaterial wrapper;
				wrapper.deserialize(deserializer);
				materials.push_back(wrapper.data);
				deserializer.exitObject();
			}
			if (i < count - 1)
				deserializer.nextArrayElement();
		}
		deserializer.exitArray();
	}

	// FIXED: Load TEV infos
	tevInfos.clear();
	if (deserializer.enterArray("tev_infos")) {
		size_t count = deserializer.getArraySize();
		tevInfos.reserve(count);
		for (size_t i = 0; i < count; ++i) {
			if (deserializer.enterObject("")) {
				SerializableTEVInfo wrapper;
				wrapper.deserialize(deserializer);
				tevInfos.push_back(wrapper.data);
				deserializer.exitObject();
			}
			if (i < count - 1)
				deserializer.nextArrayElement();
		}
		deserializer.exitArray();
	}

	return true;
}
} // namespace mat

#endif // MATERIAL_SERIALIZER_HPP