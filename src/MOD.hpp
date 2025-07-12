#ifndef MOD_HPP
#define MOD_HPP

#include "common.hpp"
#include <array>
#include <optional>
#include <type_traits>
#include <string_view>
#include <vector>
#include <bitset>
#include <unordered_set>

/**
 * @brief Enumeration representing the flags for the MOD.
 */
enum class MODFlags : u8 {
	None         = 0x00,
	UseNBT       = 0x01,
	AllowCaching = 0x02, // Allows caching of shape geometry into a display list.
	AlwaysRedraw = 0x04, // Forces the shape to be redrawn every frame, bypassing any cached display list.
	IsPlatform   = 0x10, // Indicates the shape is a platform or has platform collision.
};

/**
 * @brief Struct representing the header of the MOD.
 */
struct MODHeader {
	struct {
		u16 mYear = 2001;
		u8 mMonth = 04;
		u8 mDay   = 05;
	} mDateTime {};

	u32 mFlags = static_cast<u32>(MODFlags::None);
};

/**
 * @brief Struct representing the MOD.
 */
struct MOD {
	/**
	 * @brief Enumeration representing the chunk types in the MOD.
	 */
	enum class EChunkType {
		Header = 0x00,

		Vertex       = 0x10,
		VertexNormal = 0x11,
		VertexNBT    = 0x12,
		VertexColour = 0x13,

		TexCoord0 = 0x18,
		TexCoord1 = 0x19,
		TexCoord2 = 0x1A,
		TexCoord3 = 0x1B,
		TexCoord4 = 0x1C,
		TexCoord5 = 0x1D,
		TexCoord6 = 0x1E,
		TexCoord7 = 0x1F,

		Texture          = 0x20,
		TextureAttribute = 0x22,

		Material = 0x30,

		VertexMatrix   = 0x40,
		MatrixEnvelope = 0x41,

		Mesh      = 0x50,
		Joint     = 0x60,
		JointName = 0x61,

		CollisionPrism = 0x100,
		CollisionGrid  = 0x110,

		EndOfFile = 0xFFFF,
	};

	/**
	 * @brief Default constructor for MOD.
	 */
	MOD() = default;

	/**
	 * @brief Constructor for MOD that reads data from the given reader.
	 * @param reader The fstream_reader to read data from.
	 */
	MOD(util::fstream_reader& reader) { read(reader); }

	/**
	 * @brief Default destructor for MOD.
	 */
	~MOD() = default;

	/**
	 * @brief Reads the MOD data from the given reader.
	 * @param reader The fstream_reader to read data from.
	 */
	void read(util::fstream_reader& reader);

	/**
	 * @brief Writes the MOD data to the given writer.
	 * @param writer The fstream_writer to write data to.
	 */
	void write(util::fstream_writer& writer);

	/**
	 * @brief Resets the MOD data to its default state.
	 */
	void reset();

	/**
	 * @brief Gets the name of the chunk with the given opcode.
	 * @param opcode The opcode of the chunk.
	 * @return The name of the chunk as an optional string_view.
	 */
	static std::optional<std::string_view> getChunkName(u32 opcode);

	/**
	 * @brief Gets the name of the chunk with the given chunk type.
	 * @param chunkType The chunk type.
	 * @return The name of the chunk as an optional string_view.
	 */
	static std::optional<std::string_view> getChunkName(EChunkType chunkType);

	MODHeader mHeader;
	std::vector<Vector3f> mVertices;
	std::vector<Vector3f> mVertexNormals;
	std::vector<NBT> mVertexNbt;
	std::vector<ColourU8> mVertexColours;
	std::array<std::vector<Vector2f>, 8> mTextureCoords;
	std::vector<Texture> mTextures;
	std::vector<TextureAttributes> mTextureAttributes;
	MaterialContainer mMaterials;
	std::vector<VtxMatrix> mVertexMatrices;
	std::vector<Envelope> mVertexEnvelopes;
	std::vector<Mesh> mMeshes;
	std::vector<Joint> mJoints;
	std::vector<std::string> mJointNames;
	CollTriInfo mCollisionTriangles;
	CollGrid mCollisionGridInfo;
	std::vector<u8> mEndOfFileData;

	// When doing unit tests, empty chunks arise, we need to keep track of them
	std::unordered_set<EChunkType> mEmptyChunks;

	bool mVerbosePrint = false;
};

#endif