#include "MOD.hpp"
#include <iostream>
#include <map>

namespace {
static inline void writeGenericChunk(util::fstream_writer& writer, auto& vector, u32 chunkIdentifier)
{
	std::cout << "Writing 0x" << std::hex << chunkIdentifier << std::dec << " at offset 0x" << std::hex << writer.tellp() << std::dec
	          << ", " << MOD::getChunkName(chunkIdentifier).value() << std::endl;

	u32 subchunkPos = startChunk(writer, chunkIdentifier);
	writer.writeU32(static_cast<u32>(vector.size()));

	writer.align(0x20);
	for (auto& contents : vector) {
		contents.write(writer);
	}
	finishChunk(writer, subchunkPos);
}

static inline void readGenericChunk(util::fstream_reader& reader, auto& vector)
{
	vector.resize(reader.readU32());

	reader.align(0x20);
	for (auto& elem : vector) {
		elem.read(reader);
	}
	reader.align(0x20);
}
} // namespace

void MOD::read(util::fstream_reader& reader)
{
	bool stopRead = false;
	while (!stopRead) {
		std::streampos position = reader.tellg();
		const u32 opcode        = reader.readU32();
		const u32 length        = reader.readU32();

		if (position & 0x1F) {
			std::cout << "Error in chunk " << opcode << ", offset " << position
			          << ", chunk start isn't aligned to 0x20, this means an "
			             "improper read occured."
			          << std::endl;
			return;
		}

		const auto ocString = getChunkName(opcode);
		std::cout << "Reading 0x" << std::hex << opcode << std::dec << ", " << (ocString.has_value() ? ocString.value() : "Unknown chunk")
		          << std::endl;

		switch (static_cast<EChunkType>(opcode)) {
		case EChunkType::Header:
			reader.align(0x20);
			mHeader.mDateTime.mYear  = reader.readU16();
			mHeader.mDateTime.mMonth = reader.readU8();
			mHeader.mDateTime.mDay   = reader.readU8();
			mHeader.mFlags           = reader.readU32();
			reader.align(0x20);
			break;
		case EChunkType::Vertex:
			readGenericChunk(reader, mVertices);
			break;
		case EChunkType::VertexNormal:
			readGenericChunk(reader, mVertexNormals);
			break;
		case EChunkType::VertexNBT:
			readGenericChunk(reader, mVertexNbt);
			break;
		case EChunkType::VertexColour:
			readGenericChunk(reader, mVertexColours);
			break;
		case EChunkType::TexCoord0:
		case EChunkType::TexCoord1:
		case EChunkType::TexCoord2:
		case EChunkType::TexCoord3:
		case EChunkType::TexCoord4:
		case EChunkType::TexCoord5:
		case EChunkType::TexCoord6:
		case EChunkType::TexCoord7:
			readGenericChunk(reader, mTextureCoords[opcode - 0x18]);
			break;
		case EChunkType::Texture:
			readGenericChunk(reader, mTextures);
			break;
		case EChunkType::TextureAttribute:
			readGenericChunk(reader, mTextureAttributes);
			break;
		case EChunkType::Material:
			mMaterials.mMaterials.resize(reader.readU32());
			mMaterials.mTevEnvironmentInfo.resize(reader.readU32());

			reader.align(0x20);
			if (mMaterials.mTevEnvironmentInfo.size()) {
				for (mat::TEVInfo& info : mMaterials.mTevEnvironmentInfo) {
					info.read(reader);
				}
			}

			if (mMaterials.mMaterials.size()) {
				for (mat::Material& mat : mMaterials.mMaterials) {
					mat.read(reader);
				}
			}
			reader.align(0x20);
			break;
		case EChunkType::VertexMatrix:
			readGenericChunk(reader, mVertexMatrices);
			break;
		case EChunkType::MatrixEnvelope:
			readGenericChunk(reader, mVertexEnvelopes);
			break;
		case EChunkType::Mesh:
			readGenericChunk(reader, mMeshes);
			break;
		case EChunkType::Joint:
			readGenericChunk(reader, mJoints);
			break;
		case EChunkType::JointName:
			mJointNames.resize(reader.readU32());
			reader.align(0x20);
			for (std::string& str : mJointNames) {
				str.resize(reader.readU32());
				for (u32 i = 0; i < str.size(); i++) {
					str[i] = reader.readU8();
				}
			}
			reader.align(0x20);
			break;
		case EChunkType::CollisionPrism:
			mCollisionTriangles.read(reader);
			break;
		case EChunkType::CollisionGrid:
			mCollisionGridInfo.read(reader);
			break;
		case EChunkType::EndOfFile:
			// Seek ahead to the end of the chunk subheader
			reader.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(length), std::ios_base::cur);

			// Read the end of file bytes (ini file, most often)
			while (!reader.eof()) {
				mEndOfFileData.push_back(static_cast<u8>(reader.get()));
				reader.peek();
			}

			stopRead = true;
			break;
		default:
			// If we don't recognise the chunk then skip it
			reader.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(length), std::ios_base::cur);
			break;
		}
	}
}

// NOTE: the control flow and layout of this function is a replica of a
// decompiled version of the DMD->MOD process, found in plugTexConv
void MOD::write(util::fstream_writer& writer)
{
	// Write header
	u32 headerPos = startChunk(writer, 0);
	writer.align(0x20);
	writer.writeU16(mHeader.mDateTime.mYear);
	writer.writeU8(mHeader.mDateTime.mMonth);
	writer.writeU8(mHeader.mDateTime.mDay);
	writer.writeU32(mHeader.mFlags);
	finishChunk(writer, headerPos);

	if (mVertices.size()) {
		writeGenericChunk(writer, mVertices, 0x10);
	}

	if (mVertexColours.size()) {
		writeGenericChunk(writer, mVertexColours, 0x13);
	}

	if (mVertexNormals.size()) {
		writeGenericChunk(writer, mVertexNormals, 0x11);
	}

	if (mHeader.mFlags & static_cast<u32>(MODFlags::UseNBT) && mVertexNbt.size()) {
		writeGenericChunk(writer, mVertexNbt, 0x12);
	}

	for (std::size_t i = 0; i < mTextureCoords.size(); i++) {
		if (mTextureCoords[i].size()) {
			writeGenericChunk(writer, mTextureCoords[i], static_cast<u32>(i + 0x18));
		}
	}

	if (mTextures.size()) {
		writeGenericChunk(writer, mTextures, 0x20);
	}

	if (mTextureAttributes.size()) {
		writeGenericChunk(writer, mTextureAttributes, 0x22);
	}

	if (mMaterials.mMaterials.size()) {
		std::cout << "Writing 0x30, " << MOD::getChunkName(0x30).value() << std::endl;

		const u32 start = startChunk(writer, 0x30);
		writer.writeU32(static_cast<u32>(mMaterials.mMaterials.size()));
		writer.writeU32(static_cast<u32>(mMaterials.mTevEnvironmentInfo.size()));
		writer.align(0x20);

		for (mat::TEVInfo& tevInfo : mMaterials.mTevEnvironmentInfo) {
			std::cout << "tev @ " << std::hex << writer.tellp() << std::dec << std::endl;
			tevInfo.write(writer);
		}

		for (mat::Material& material : mMaterials.mMaterials) {
			std::cout << "material @ " << std::hex << writer.tellp() << std::dec << std::endl;
			material.write(writer);
		}

		finishChunk(writer, start);
	}

	if (mVertexEnvelopes.size()) {
		writeGenericChunk(writer, mVertexEnvelopes, 0x41);
	}

	if (mVertexMatrices.size()) {
		writeGenericChunk(writer, mVertexMatrices, 0x40);
	}

	if (mMeshes.size()) {
		writeGenericChunk(writer, mMeshes, 0x50);
	}

	if (mJoints.size()) {
		writeGenericChunk(writer, mJoints, 0x60);

		if (mJointNames.size()) {
			std::cout << "Writing 0x61, " << MOD::getChunkName(0x61).value() << std::endl;

			const u32 start = startChunk(writer, 0x61);
			writer.writeU32(static_cast<u32>(mJointNames.size()));
			writer.align(0x20);
			for (std::string& name : mJointNames) {
				writer.writeU32(static_cast<u32>(name.size()));
				for (std::size_t i = 0; i < name.size(); i++) {
					writer.writeU8(name[i]);
				}
			}
			finishChunk(writer, start);
		}
	}

	if (mCollisionTriangles.mCollInfo.size()) {
		std::cout << "Writing 0x100, " << MOD::getChunkName(0x100).value() << std::endl;
		mCollisionTriangles.write(writer);

		const u32 start = startChunk(writer, 0x110);
		writer.align(0x20);
		mCollisionGridInfo.mBoundsMin.write(writer);
		mCollisionGridInfo.mBoundsMax.write(writer);
		writer.writeF32(mCollisionGridInfo.mGridSize);
		writer.writeU32(mCollisionGridInfo.mGridSizeX);
		writer.writeU32(mCollisionGridInfo.mGridSizeY);
		writer.writeU32(static_cast<u32>(mCollisionGridInfo.m_groups.size()));
		for (CollGroup& group : mCollisionGridInfo.m_groups) {
			group.write(writer);
		}
		for (s32& i : mCollisionGridInfo.m_unknown2) {
			writer.writeS32(i);
		}
		writer.align(0x20);
		finishChunk(writer, start);
	}

	// Finalise writing with 0xFFFF chunk and append any INI file
	finishChunk(writer, startChunk(writer, 0xFFFF));
	if (mEndOfFileData.size()) {
		std::cout << "Writing 0xffff, " << MOD::getChunkName(0xffff).value() << std::endl;

		writer.write(reinterpret_cast<char*>(mEndOfFileData.data()), mEndOfFileData.size());
	}
}

void MOD::reset()
{
	mVertices.clear();
	mVertexNormals.clear();
	mVertexNbt.clear();
	mVertexColours.clear();
	for (u32 i = 0; i < 8; i++) {
		mTextureCoords[i].clear();
	}
	mTextures.clear();
	mTextureAttributes.clear();
	mMaterials.mMaterials.clear();
	mMaterials.mTevEnvironmentInfo.clear();
	mVertexMatrices.clear();
	mVertexEnvelopes.clear();
	mMeshes.clear();
	mJoints.clear();
	mJointNames.clear();

	mCollisionTriangles.mCollInfo.clear();
	mCollisionTriangles.mRoomInfo.clear();

	mCollisionGridInfo.clear();

	mEndOfFileData.clear();
}

// clang-format off
const std::map<u32, std::string_view> gChunkNames = 
{
    { 0x00, "Header" },
    { 0x10, "Vertices" },
    { 0x11, "Vertex Normals" },
    { 0x12, "Vertex Normal/Binormal/Tangent Descriptors" },
    { 0x13, "Vertex Colours" },

    { 0x18, "Texture Coordinate 0" },
    { 0x19, "Texture Coordinate 1" },
    { 0x1A, "Texture Coordinate 2" },
    { 0x1B, "Texture Coordinate 3" },
    { 0x1C, "Texture Coordinate 4" },
    { 0x1D, "Texture Coordinate 5" },
    { 0x1E, "Texture Coordinate 6" },
    { 0x1F, "Texture Coordinate 7" },

    { 0x20, "Textures" },
    { 0x22, "Texture Attributes" },
    { 0x30, "Materials" },

    { 0x40, "Vertex Matrix" },
    { 0x41, "Matrix Envelope" },

    { 0x50, "Mesh" },
    { 0x60, "Joints" },
    { 0x61, "Joint Names" },

    { 0x100, "Collision Prism" },
    { 0x110, "Collision Grid" },
    { 0xFFFF, "End Of File" }
};
// clang-format on

const std::optional<std::string_view> MOD::getChunkName(u32 opcode)
{
	if (gChunkNames.contains(opcode)) {
		return gChunkNames.at(opcode);
	}

	return std::nullopt;
}

const std::optional<std::string_view> MOD::getChunkName(EChunkType chunkType)
{
	if (gChunkNames.contains(static_cast<u32>(chunkType))) {
		return gChunkNames.at(static_cast<u32>(chunkType));
	}

	return std::nullopt;
}
