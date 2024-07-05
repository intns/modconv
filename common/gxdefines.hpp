#ifndef GXDEFINES_HPP
#define GXDEFINES_HPP

#include <string>

typedef enum _GXTexCoordID {
	GX_TEXCOORD0 = 0x0, // generated texture coordinate 0
	GX_TEXCOORD1,       // generated texture coordinate 1
	GX_TEXCOORD2,       // generated texture coordinate 2
	GX_TEXCOORD3,       // generated texture coordinate 3
	GX_TEXCOORD4,       // generated texture coordinate 4
	GX_TEXCOORD5,       // generated texture coordinate 5
	GX_TEXCOORD6,       // generated texture coordinate 6
	GX_TEXCOORD7,       // generated texture coordinate 7
	GX_MAX_TEXCOORD  = 8,
	GX_TEXCOORD_NULL = 0xff

} GXTexCoordID;

inline std::string GXTexCoordIDToStringConverter(_GXTexCoordID texCoordID)
{
	switch (texCoordID) {
	case GX_TEXCOORD0:
		return "GX_TEXCOORD0";
	case GX_TEXCOORD1:
		return "GX_TEXCOORD1";
	case GX_TEXCOORD2:
		return "GX_TEXCOORD2";
	case GX_TEXCOORD3:
		return "GX_TEXCOORD3";
	case GX_TEXCOORD4:
		return "GX_TEXCOORD4";
	case GX_TEXCOORD5:
		return "GX_TEXCOORD5";
	case GX_TEXCOORD6:
		return "GX_TEXCOORD6";
	case GX_TEXCOORD7:
		return "GX_TEXCOORD7";
	case GX_MAX_TEXCOORD:
		return "GX_MAX_TEXCOORD";
	case GX_TEXCOORD_NULL:
		return "GX_TEXCOORD_NULL";
	default:
		return "Unknown";
	}
}

typedef enum _GXTexMapID {
	GX_TEXMAP0,
	GX_TEXMAP1,
	GX_TEXMAP2,
	GX_TEXMAP3,
	GX_TEXMAP4,
	GX_TEXMAP5,
	GX_TEXMAP6,
	GX_TEXMAP7,
	GX_MAX_TEXMAP,

	GX_TEXMAP_NULL = 0xff,
	GX_TEX_DISABLE = 0x100 // mask: disables texture look up

} GXTexMapID;

inline std::string GXTexMapIDToStringConverter(_GXTexMapID texMapID)
{
	switch (texMapID) {
	case GX_TEXMAP0:
		return "GX_TEXMAP0";
	case GX_TEXMAP1:
		return "GX_TEXMAP1";
	case GX_TEXMAP2:
		return "GX_TEXMAP2";
	case GX_TEXMAP3:
		return "GX_TEXMAP3";
	case GX_TEXMAP4:
		return "GX_TEXMAP4";
	case GX_TEXMAP5:
		return "GX_TEXMAP5";
	case GX_TEXMAP6:
		return "GX_TEXMAP6";
	case GX_TEXMAP7:
		return "GX_TEXMAP7";
	case GX_MAX_TEXMAP:
		return "GX_MAX_TEXMAP";
	case GX_TEXMAP_NULL:
		return "GX_TEXMAP_NULL";
	case GX_TEX_DISABLE:
		return "GX_TEX_DISABLE";
	default:
		return "Unknown";
	}
}

typedef enum _GXChannelID {
	GX_COLOR0,
	GX_COLOR1,
	GX_ALPHA0,
	GX_ALPHA1,
	GX_COLOR0A0,    // Color 0 + Alpha 0
	GX_COLOR1A1,    // Color 1 + Alpha 1
	GX_COLOR_ZERO,  // RGBA = 0
	GX_ALPHA_BUMP,  // bump alpha 0-248, RGB=0
	GX_ALPHA_BUMPN, // normalized bump alpha, 0-255, RGB=0
	GX_COLOR_NULL = 0xff

} GXChannelID;

inline std::string GXChannelIDToStringConverter(_GXChannelID channelID)
{
	switch (channelID) {
	case GX_COLOR0:
		return "GX_COLOR0";
	case GX_COLOR1:
		return "GX_COLOR1";
	case GX_ALPHA0:
		return "GX_ALPHA0";
	case GX_ALPHA1:
		return "GX_ALPHA1";
	case GX_COLOR0A0:
		return "GX_COLOR0A0";
	case GX_COLOR1A1:
		return "GX_COLOR1A1";
	case GX_COLOR_ZERO:
		return "GX_COLOR_ZERO";
	case GX_ALPHA_BUMP:
		return "GX_ALPHA_BUMP";
	case GX_ALPHA_BUMPN:
		return "GX_ALPHA_BUMPN";
	case GX_COLOR_NULL:
		return "GX_COLOR_NULL";
	default:
		return "Unknown";
	}
}

typedef enum _GXTevKColorSel {
	GX_TEV_KCSEL_1   = 0x00,
	GX_TEV_KCSEL_7_8 = 0x01,
	GX_TEV_KCSEL_3_4 = 0x02,
	GX_TEV_KCSEL_5_8 = 0x03,
	GX_TEV_KCSEL_1_2 = 0x04,
	GX_TEV_KCSEL_3_8 = 0x05,
	GX_TEV_KCSEL_1_4 = 0x06,
	GX_TEV_KCSEL_1_8 = 0x07,

	GX_TEV_KCSEL_K0   = 0x0C,
	GX_TEV_KCSEL_K1   = 0x0D,
	GX_TEV_KCSEL_K2   = 0x0E,
	GX_TEV_KCSEL_K3   = 0x0F,
	GX_TEV_KCSEL_K0_R = 0x10,
	GX_TEV_KCSEL_K1_R = 0x11,
	GX_TEV_KCSEL_K2_R = 0x12,
	GX_TEV_KCSEL_K3_R = 0x13,
	GX_TEV_KCSEL_K0_G = 0x14,
	GX_TEV_KCSEL_K1_G = 0x15,
	GX_TEV_KCSEL_K2_G = 0x16,
	GX_TEV_KCSEL_K3_G = 0x17,
	GX_TEV_KCSEL_K0_B = 0x18,
	GX_TEV_KCSEL_K1_B = 0x19,
	GX_TEV_KCSEL_K2_B = 0x1A,
	GX_TEV_KCSEL_K3_B = 0x1B,
	GX_TEV_KCSEL_K0_A = 0x1C,
	GX_TEV_KCSEL_K1_A = 0x1D,
	GX_TEV_KCSEL_K2_A = 0x1E,
	GX_TEV_KCSEL_K3_A = 0x1F

} GXTevKColorSel;

inline std::string GXTevKColorSelToStringConverter(_GXTevKColorSel kColorSel)
{
	switch (kColorSel) {
	case GX_TEV_KCSEL_1:
		return "GX_TEV_KCSEL_1";
	case GX_TEV_KCSEL_7_8:
		return "GX_TEV_KCSEL_7_8";
	case GX_TEV_KCSEL_3_4:
		return "GX_TEV_KCSEL_3_4";
	case GX_TEV_KCSEL_5_8:
		return "GX_TEV_KCSEL_5_8";
	case GX_TEV_KCSEL_1_2:
		return "GX_TEV_KCSEL_1_2";
	case GX_TEV_KCSEL_3_8:
		return "GX_TEV_KCSEL_3_8";
	case GX_TEV_KCSEL_1_4:
		return "GX_TEV_KCSEL_1_4";
	case GX_TEV_KCSEL_1_8:
		return "GX_TEV_KCSEL_1_8";
	case GX_TEV_KCSEL_K0:
		return "GX_TEV_KCSEL_K0";
	case GX_TEV_KCSEL_K1:
		return "GX_TEV_KCSEL_K1";
	case GX_TEV_KCSEL_K2:
		return "GX_TEV_KCSEL_K2";
	case GX_TEV_KCSEL_K3:
		return "GX_TEV_KCSEL_K3";
	case GX_TEV_KCSEL_K0_R:
		return "GX_TEV_KCSEL_K0_R";
	case GX_TEV_KCSEL_K1_R:
		return "GX_TEV_KCSEL_K1_R";
	case GX_TEV_KCSEL_K2_R:
		return "GX_TEV_KCSEL_K2_R";
	case GX_TEV_KCSEL_K3_R:
		return "GX_TEV_KCSEL_K3_R";
	case GX_TEV_KCSEL_K0_G:
		return "GX_TEV_KCSEL_K0_G";
	case GX_TEV_KCSEL_K1_G:
		return "GX_TEV_KCSEL_K1_G";
	case GX_TEV_KCSEL_K2_G:
		return "GX_TEV_KCSEL_K2_G";
	case GX_TEV_KCSEL_K3_G:
		return "GX_TEV_KCSEL_K3_G";
	case GX_TEV_KCSEL_K0_B:
		return "GX_TEV_KCSEL_K0_B";
	case GX_TEV_KCSEL_K1_B:
		return "GX_TEV_KCSEL_K1_B";
	case GX_TEV_KCSEL_K2_B:
		return "GX_TEV_KCSEL_K2_B";
	case GX_TEV_KCSEL_K3_B:
		return "GX_TEV_KCSEL_K3_B";
	case GX_TEV_KCSEL_K0_A:
		return "GX_TEV_KCSEL_K0_A";
	case GX_TEV_KCSEL_K1_A:
		return "GX_TEV_KCSEL_K1_A";
	case GX_TEV_KCSEL_K2_A:
		return "GX_TEV_KCSEL_K2_A";
	case GX_TEV_KCSEL_K3_A:
		return "GX_TEV_KCSEL_K3_A";
	default:
		return "Unknown";
	}
}

typedef enum _GXTevKAlphaSel {
	GX_TEV_KASEL_1   = 0x00,
	GX_TEV_KASEL_7_8 = 0x01,
	GX_TEV_KASEL_3_4 = 0x02,
	GX_TEV_KASEL_5_8 = 0x03,
	GX_TEV_KASEL_1_2 = 0x04,
	GX_TEV_KASEL_3_8 = 0x05,
	GX_TEV_KASEL_1_4 = 0x06,
	GX_TEV_KASEL_1_8 = 0x07,

	GX_TEV_KASEL_K0_R = 0x10,
	GX_TEV_KASEL_K1_R = 0x11,
	GX_TEV_KASEL_K2_R = 0x12,
	GX_TEV_KASEL_K3_R = 0x13,
	GX_TEV_KASEL_K0_G = 0x14,
	GX_TEV_KASEL_K1_G = 0x15,
	GX_TEV_KASEL_K2_G = 0x16,
	GX_TEV_KASEL_K3_G = 0x17,
	GX_TEV_KASEL_K0_B = 0x18,
	GX_TEV_KASEL_K1_B = 0x19,
	GX_TEV_KASEL_K2_B = 0x1A,
	GX_TEV_KASEL_K3_B = 0x1B,
	GX_TEV_KASEL_K0_A = 0x1C,
	GX_TEV_KASEL_K1_A = 0x1D,
	GX_TEV_KASEL_K2_A = 0x1E,
	GX_TEV_KASEL_K3_A = 0x1F

} GXTevKAlphaSel;

inline std::string GXTevKAlphaSelToStringConverter(_GXTevKAlphaSel kAlphaSel)
{
	switch (kAlphaSel) {
	case GX_TEV_KASEL_1:
		return "GX_TEV_KASEL_1";
	case GX_TEV_KASEL_7_8:
		return "GX_TEV_KASEL_7_8";
	case GX_TEV_KASEL_3_4:
		return "GX_TEV_KASEL_3_4";
	case GX_TEV_KASEL_5_8:
		return "GX_TEV_KASEL_5_8";
	case GX_TEV_KASEL_1_2:
		return "GX_TEV_KASEL_1_2";
	case GX_TEV_KASEL_3_8:
		return "GX_TEV_KASEL_3_8";
	case GX_TEV_KASEL_1_4:
		return "GX_TEV_KASEL_1_4";
	case GX_TEV_KASEL_1_8:
		return "GX_TEV_KASEL_1_8";
	case GX_TEV_KASEL_K0_R:
		return "GX_TEV_KASEL_K0_R";
	case GX_TEV_KASEL_K1_R:
		return "GX_TEV_KASEL_K1_R";
	case GX_TEV_KASEL_K2_R:
		return "GX_TEV_KASEL_K2_R";
	case GX_TEV_KASEL_K3_R:
		return "GX_TEV_KASEL_K3_R";
	case GX_TEV_KASEL_K0_G:
		return "GX_TEV_KASEL_K0_G";
	case GX_TEV_KASEL_K1_G:
		return "GX_TEV_KASEL_K1_G";
	case GX_TEV_KASEL_K2_G:
		return "GX_TEV_KASEL_K2_G";
	case GX_TEV_KASEL_K3_G:
		return "GX_TEV_KASEL_K3_G";
	case GX_TEV_KASEL_K0_B:
		return "GX_TEV_KASEL_K0_B";
	case GX_TEV_KASEL_K1_B:
		return "GX_TEV_KASEL_K1_B";
	case GX_TEV_KASEL_K2_B:
		return "GX_TEV_KASEL_K2_B";
	case GX_TEV_KASEL_K3_B:
		return "GX_TEV_KASEL_K3_B";
	case GX_TEV_KASEL_K0_A:
		return "GX_TEV_KASEL_K0_A";
	case GX_TEV_KASEL_K1_A:
		return "GX_TEV_KASEL_K1_A";
	case GX_TEV_KASEL_K2_A:
		return "GX_TEV_KASEL_K2_A";
	case GX_TEV_KASEL_K3_A:
		return "GX_TEV_KASEL_K3_A";
	default:
		return "Unknown";
	}
}

typedef enum _GXTexMtx {
	GX_TEXMTX0  = 0,
	GX_TEXMTX1  = 1,
	GX_TEXMTX2  = 2,
	GX_TEXMTX3  = 3,
	GX_TEXMTX4  = 4,
	GX_TEXMTX5  = 5,
	GX_TEXMTX6  = 6,
	GX_TEXMTX7  = 7,
	GX_TEXMTX8  = 8,
	GX_TEXMTX9  = 9,
	GX_IDENTITY = 0xFF,

} GXTexMtx;

inline std::string GXTexMtxToStringConverter(_GXTexMtx texMtx)
{
	switch (texMtx) {
	case GX_TEXMTX0:
		return "GX_TEXMTX0";
	case GX_TEXMTX1:
		return "GX_TEXMTX1";
	case GX_TEXMTX2:
		return "GX_TEXMTX2";
	case GX_TEXMTX3:
		return "GX_TEXMTX3";
	case GX_TEXMTX4:
		return "GX_TEXMTX4";
	case GX_TEXMTX5:
		return "GX_TEXMTX5";
	case GX_TEXMTX6:
		return "GX_TEXMTX6";
	case GX_TEXMTX7:
		return "GX_TEXMTX7";
	case GX_TEXMTX8:
		return "GX_TEXMTX8";
	case GX_TEXMTX9:
		return "GX_TEXMTX9";
	case GX_IDENTITY:
		return "GX_IDENTITY";
	default:
		return std::to_string((u32)texMtx);
	}
}

typedef enum _GXTexGenSrc {
	GX_TG_POS = 0,
	GX_TG_NRM,
	GX_TG_BINRM,
	GX_TG_TANGENT,
	GX_TG_TEX0,
	GX_TG_TEX1,
	GX_TG_TEX2,
	GX_TG_TEX3,
	GX_TG_TEX4,
	GX_TG_TEX5,
	GX_TG_TEX6,
	GX_TG_TEX7,
	GX_TG_TEXCOORD0,
	GX_TG_TEXCOORD1,
	GX_TG_TEXCOORD2,
	GX_TG_TEXCOORD3,
	GX_TG_TEXCOORD4,
	GX_TG_TEXCOORD5,
	GX_TG_TEXCOORD6,
	GX_TG_COLOR0,
	GX_TG_COLOR1

} GXTexGenSrc;

inline std::string GXTexGenSrcToStringConverter(_GXTexGenSrc texGenSrc)
{
	switch (texGenSrc) {
	case GX_TG_POS:
		return "GX_TG_POS";
	case GX_TG_NRM:
		return "GX_TG_NRM";
	case GX_TG_BINRM:
		return "GX_TG_BINRM";
	case GX_TG_TANGENT:
		return "GX_TG_TANGENT";
	case GX_TG_TEX0:
		return "GX_TG_TEX0";
	case GX_TG_TEX1:
		return "GX_TG_TEX1";
	case GX_TG_TEX2:
		return "GX_TG_TEX2";
	case GX_TG_TEX3:
		return "GX_TG_TEX3";
	case GX_TG_TEX4:
		return "GX_TG_TEX4";
	case GX_TG_TEX5:
		return "GX_TG_TEX5";
	case GX_TG_TEX6:
		return "GX_TG_TEX6";
	case GX_TG_TEX7:
		return "GX_TG_TEX7";
	case GX_TG_TEXCOORD0:
		return "GX_TG_TEXCOORD0";
	case GX_TG_TEXCOORD1:
		return "GX_TG_TEXCOORD1";
	case GX_TG_TEXCOORD2:
		return "GX_TG_TEXCOORD2";
	case GX_TG_TEXCOORD3:
		return "GX_TG_TEXCOORD3";
	case GX_TG_TEXCOORD4:
		return "GX_TG_TEXCOORD4";
	case GX_TG_TEXCOORD5:
		return "GX_TG_TEXCOORD5";
	case GX_TG_TEXCOORD6:
		return "GX_TG_TEXCOORD6";
	case GX_TG_COLOR0:
		return "GX_TG_COLOR0";
	case GX_TG_COLOR1:
		return "GX_TG_COLOR1";
	default:
		return "Unknown";
	}
}

typedef enum _GXTexGenType {
	GX_TG_MTX3x4 = 0,
	GX_TG_MTX2x4,
	GX_TG_BUMP0,
	GX_TG_BUMP1,
	GX_TG_BUMP2,
	GX_TG_BUMP3,
	GX_TG_BUMP4,
	GX_TG_BUMP5,
	GX_TG_BUMP6,
	GX_TG_BUMP7,
	GX_TG_SRTG

} GXTexGenType;

inline std::string GXTexGenTypeToStringConverter(_GXTexGenType texGenType)
{
	switch (texGenType) {
	case GX_TG_MTX3x4:
		return "GX_TG_MTX3x4";
	case GX_TG_MTX2x4:
		return "GX_TG_MTX2x4";
	case GX_TG_BUMP0:
		return "GX_TG_BUMP0";
	case GX_TG_BUMP1:
		return "GX_TG_BUMP1";
	case GX_TG_BUMP2:
		return "GX_TG_BUMP2";
	case GX_TG_BUMP3:
		return "GX_TG_BUMP3";
	case GX_TG_BUMP4:
		return "GX_TG_BUMP4";
	case GX_TG_BUMP5:
		return "GX_TG_BUMP5";
	case GX_TG_BUMP6:
		return "GX_TG_BUMP6";
	case GX_TG_BUMP7:
		return "GX_TG_BUMP7";
	case GX_TG_SRTG:
		return "GX_TG_SRTG";
	default:
		return "Unknown";
	}
}

typedef enum _GXTevColorArg {
	GX_CC_CPREV,
	GX_CC_APREV,
	GX_CC_C0,
	GX_CC_A0,
	GX_CC_C1,
	GX_CC_A1,
	GX_CC_C2,
	GX_CC_A2,
	GX_CC_TEXC,
	GX_CC_TEXA,
	GX_CC_RASC,
	GX_CC_RASA,
	GX_CC_ONE,
	GX_CC_HALF,
	GX_CC_QUARTER,
	GX_CC_ZERO,

	GX_CC_TEXRRR,
	GX_CC_TEXGGG,
	GX_CC_TEXBBB
} GXTevColorArg;

inline std::string GXTevColorArgToStringConverter(_GXTevColorArg tevColorArg)
{
	switch (tevColorArg) {
	case GX_CC_CPREV:
		return "GX_CC_CPREV";
	case GX_CC_APREV:
		return "GX_CC_APREV";
	case GX_CC_C0:
		return "GX_CC_C0";
	case GX_CC_A0:
		return "GX_CC_A0";
	case GX_CC_C1:
		return "GX_CC_C1";
	case GX_CC_A1:
		return "GX_CC_A1";
	case GX_CC_C2:
		return "GX_CC_C2";
	case GX_CC_A2:
		return "GX_CC_A2";
	case GX_CC_TEXC:
		return "GX_CC_TEXC";
	case GX_CC_TEXA:
		return "GX_CC_TEXA";
	case GX_CC_RASC:
		return "GX_CC_RASC";
	case GX_CC_RASA:
		return "GX_CC_RASA";
	case GX_CC_ONE:
		return "GX_CC_ONE";
	case GX_CC_HALF:
		return "GX_CC_HALF";
	case GX_CC_QUARTER:
		return "GX_CC_QUARTER";
	case GX_CC_ZERO:
		return "GX_CC_ZERO";
	case GX_CC_TEXRRR:
		return "GX_CC_TEXRRR";
	case GX_CC_TEXGGG:
		return "GX_CC_TEXGGG";
	case GX_CC_TEXBBB:
		return "GX_CC_TEXBBB";
	default:
		return "Unknown";
	}
}

typedef enum _GXTexWrapMode {
	GX_CLAMP,
	GX_REPEAT,
	GX_MIRROR,
	GX_MAX_TEXWRAPMODE

} GXTexWrapMode;

inline std::string GXTexWrapModeToStringConverter(_GXTexWrapMode texWrapMode)
{
	switch (texWrapMode) {
	case GX_CLAMP:
		return "GX_CLAMP";
	case GX_REPEAT:
		return "GX_REPEAT";
	case GX_MIRROR:
		return "GX_MIRROR";
	case GX_MAX_TEXWRAPMODE:
		return "GX_MAX_TEXWRAPMODE";
	default:
		return "Unknown";
	}
}

#endif // GXDEFINES_HPP