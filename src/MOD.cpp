#include "MOD.hpp"
#include <iostream>
#include <map>

namespace {
inline void writeGenericChunk(util::fstream_writer& writer, auto& vector, u32 chunkIdentifier, bool verbose = true)
{
	if (verbose) {
		std::cout << "Writing 0x" << std::hex << chunkIdentifier << std::dec << " at offset 0x" << std::hex << std::uppercase
		          << writer.tellp() << std::nouppercase << std::dec << ", " << MOD::getChunkName(chunkIdentifier).value() << '\n';
	}

	const u32 subchunkPos = startChunk(writer, chunkIdentifier);
	writer.writeU32(static_cast<u32>(vector.size()));

	writer.align();

	for (auto& contents : vector) {
		contents.write(writer);
	}

	finishChunk(writer, subchunkPos);
}

inline void writeGenericChunk(util::fstream_writer& writer, auto& vector, MOD::EChunkType chunkIdentifier, bool verbose = true)
{
	writeGenericChunk(writer, vector, static_cast<u32>(chunkIdentifier), verbose);
}

inline void readGenericChunk(util::fstream_reader& reader, auto& vector)
{
	vector.resize(reader.readU32());

	reader.align();
	for (auto& elem : vector) {
		elem.read(reader);
	}
	reader.align();
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
			          << '\n';
			return;
		}

		if (mVerbosePrint) {
			const auto ocString = getChunkName(opcode);
			std::cout << "Reading 0x" << std::hex << opcode << std::dec << ", "
			          << (ocString.has_value() ? ocString.value() : "Unknown chunk") << '\n';
		}

		switch (static_cast<EChunkType>(opcode)) {
		case EChunkType::Header:
			reader.align();
			mHeader.mDateTime.mYear  = reader.readU16();
			mHeader.mDateTime.mMonth = reader.readU8();
			mHeader.mDateTime.mDay   = reader.readU8();
			mHeader.mFlags           = reader.readU32();
			reader.align();
			break;
		case EChunkType::Vertex:
			readGenericChunk(reader, mVertices);
			if (mVertices.empty()) {
				mEmptyChunks.insert(EChunkType::Vertex);
			}
			break;
		case EChunkType::VertexNormal:
			readGenericChunk(reader, mVertexNormals);
			if (mVertexNormals.empty()) {
				mEmptyChunks.insert(EChunkType::VertexNormal);
			}
			break;
		case EChunkType::VertexNBT:
			readGenericChunk(reader, mVertexNbt);
			if (mVertexNbt.empty()) {
				mEmptyChunks.insert(EChunkType::VertexNBT);
			}
			break;
		case EChunkType::VertexColour:
			readGenericChunk(reader, mVertexColours);
			if (mVertexColours.empty()) {
				mEmptyChunks.insert(EChunkType::VertexColour);
			}
			break;
		case EChunkType::TexCoord0:
		case EChunkType::TexCoord1:
		case EChunkType::TexCoord2:
		case EChunkType::TexCoord3:
		case EChunkType::TexCoord4:
		case EChunkType::TexCoord5:
		case EChunkType::TexCoord6:
		case EChunkType::TexCoord7: {
			auto& texCoords = mTextureCoords[static_cast<std::size_t>(opcode - 0x18)];
			readGenericChunk(reader, texCoords);
			if (texCoords.empty()) {
				mEmptyChunks.insert(static_cast<EChunkType>(opcode));
			}
			break;
		}
		case EChunkType::Texture:
			readGenericChunk(reader, mTextures);
			if (mTextures.empty()) {
				mEmptyChunks.insert(EChunkType::Texture);
			}
			break;
		case EChunkType::TextureAttribute:
			readGenericChunk(reader, mTextureAttributes);
			if (mTextureAttributes.empty()) {
				mEmptyChunks.insert(EChunkType::TextureAttribute);
			}
			break;
		case EChunkType::Material:
			mMaterials.mMaterials.resize(reader.readU32());
			mMaterials.mTevEnvironmentInfo.resize(reader.readU32());

			reader.align();
			if (!mMaterials.mTevEnvironmentInfo.empty()) {
				for (mat::TEVInfo& info : mMaterials.mTevEnvironmentInfo) {
					info.read(reader);
				}
			}

			if (!mMaterials.mMaterials.empty()) {
				for (mat::Material& mat : mMaterials.mMaterials) {
					mat.read(reader);
				}
			}

			reader.align();

			if (mMaterials.mMaterials.empty() && mMaterials.mTevEnvironmentInfo.empty()) {
				mEmptyChunks.insert(EChunkType::Material);
			}
			break;
		case EChunkType::VertexMatrix:
			readGenericChunk(reader, mVertexMatrices);
			if (mVertexMatrices.empty()) {
				mEmptyChunks.insert(EChunkType::VertexMatrix);
			}
			break;
		case EChunkType::MatrixEnvelope:
			readGenericChunk(reader, mVertexEnvelopes);
			if (mVertexEnvelopes.empty()) {
				mEmptyChunks.insert(EChunkType::MatrixEnvelope);
			}
			break;
		case EChunkType::Mesh:
			readGenericChunk(reader, mMeshes);
			if (mMeshes.empty()) {
				mEmptyChunks.insert(EChunkType::Mesh);
			}
			break;
		case EChunkType::Joint:
			readGenericChunk(reader, mJoints);
			if (mJoints.empty()) {
				mEmptyChunks.insert(EChunkType::Joint);
			}
			break;
		case EChunkType::JointName:
			mJointNames.resize(reader.readU32());
			reader.align();
			for (std::string& str : mJointNames) {
				str.resize(reader.readU32());
				for (char& i : str) {
					i = reader.readU8();
				}
			}
			reader.align();

			if (mJointNames.empty()) {
				mEmptyChunks.insert(EChunkType::JointName);
			}
			break;
		case EChunkType::CollisionPrism:
			mCollisionTriangles.read(reader);
			break;
		case EChunkType::CollisionGrid:
			mCollisionGridInfo.read(reader);
			break;
		case EChunkType::EndOfFile: {
			// Skip 'length' bytes from current position
			reader.seekg(static_cast<std::istream::off_type>(length), std::ios_base::cur);

			// Read the rest of the stream into mEndOfFileData
			std::vector<u8> buffer(std::istreambuf_iterator<char>(reader), {});
			mEndOfFileData.insert(mEndOfFileData.end(), buffer.begin(), buffer.end());

			stopRead = true;
			break;
		}
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
	u32 headerPos = startChunk(writer, static_cast<u32>(EChunkType::Header));
	writer.align();
	writer.writeU16(mHeader.mDateTime.mYear);
	writer.writeU8(mHeader.mDateTime.mMonth);
	writer.writeU8(mHeader.mDateTime.mDay);
	writer.writeU32(mHeader.mFlags);
	finishChunk(writer, headerPos);

	if (!mVertices.empty() || mEmptyChunks.contains(EChunkType::Vertex)) {
		writeGenericChunk(writer, mVertices, EChunkType::Vertex, mVerbosePrint);
	}

	if (!mVertexColours.empty() || mEmptyChunks.contains(EChunkType::VertexColour)) {
		writeGenericChunk(writer, mVertexColours, EChunkType::VertexColour, mVerbosePrint);
	}

	if (!mVertexNormals.empty() || mEmptyChunks.contains(EChunkType::VertexNormal)) {
		writeGenericChunk(writer, mVertexNormals, EChunkType::VertexNormal, mVerbosePrint);
	}

	if ((mHeader.mFlags & static_cast<u32>(MODFlags::UseNBT) && !mVertexNbt.empty()) || mEmptyChunks.contains(EChunkType::VertexNBT)) {
		writeGenericChunk(writer, mVertexNbt, EChunkType::VertexNBT, mVerbosePrint);
	}

	for (std::size_t i = 0; i < mTextureCoords.size(); i++) {
		auto texIdx = static_cast<EChunkType>(static_cast<u32>(EChunkType::TexCoord0) + i);
		if (!mTextureCoords[i].empty() || mEmptyChunks.contains(texIdx)) {
			writeGenericChunk(writer, mTextureCoords[i], texIdx, mVerbosePrint);
		}
	}

	if (!mTextures.empty() || mEmptyChunks.contains(EChunkType::Texture)) {
		writeGenericChunk(writer, mTextures, EChunkType::Texture, mVerbosePrint);
	}

	if (!mTextureAttributes.empty() || mEmptyChunks.contains(EChunkType::TextureAttribute)) {
		writeGenericChunk(writer, mTextureAttributes, EChunkType::TextureAttribute, mVerbosePrint);
	}

	if (!mMaterials.mMaterials.empty() || mEmptyChunks.contains(EChunkType::Material)) {
		if (mVerbosePrint) {
			std::cout << "Writing 0x30, " << MOD::getChunkName(EChunkType::Material).value() << '\n';
		}

		const u32 start = startChunk(writer, static_cast<u32>(EChunkType::Material));
		writer.writeU32(static_cast<u32>(mMaterials.mMaterials.size()));
		writer.writeU32(static_cast<u32>(mMaterials.mTevEnvironmentInfo.size()));
		writer.align();
		for (mat::TEVInfo& tevInfo : mMaterials.mTevEnvironmentInfo) {
			tevInfo.write(writer);
		}
		for (mat::Material& material : mMaterials.mMaterials) {
			material.write(writer);
		}
		finishChunk(writer, start);
	}

	if (!mVertexEnvelopes.empty() || mEmptyChunks.contains(EChunkType::MatrixEnvelope)) {
		writeGenericChunk(writer, mVertexEnvelopes, EChunkType::MatrixEnvelope, mVerbosePrint);
	}

	if (!mVertexMatrices.empty() || mEmptyChunks.contains(EChunkType::VertexMatrix)) {
		writeGenericChunk(writer, mVertexMatrices, EChunkType::VertexMatrix, mVerbosePrint);
	}

	if (!mMeshes.empty() || mEmptyChunks.contains(EChunkType::Mesh)) {
		writeGenericChunk(writer, mMeshes, EChunkType::Mesh, mVerbosePrint);
	}

	if (!mJoints.empty() || mEmptyChunks.contains(EChunkType::Joint)) {
		writeGenericChunk(writer, mJoints, EChunkType::Joint, mVerbosePrint);

		if (!mJointNames.empty() || mEmptyChunks.contains(EChunkType::JointName)) {
			if (mVerbosePrint) {
				std::cout << "Writing 0x61, " << MOD::getChunkName(EChunkType::JointName).value() << '\n';
			}

			const u32 start = startChunk(writer, static_cast<u32>(EChunkType::JointName));
			writer.writeU32(static_cast<u32>(mJointNames.size()));
			writer.align();
			for (std::string& name : mJointNames) {
				writer.writeU32(static_cast<u32>(name.size()));
				for (char i : name) {
					writer.writeU8(i);
				}
			}
			finishChunk(writer, start);
		}
	}

	if (!mCollisionTriangles.mCollInfo.empty()) {
		if (mVerbosePrint) {
			std::cout << "Writing 0x100, " << MOD::getChunkName(EChunkType::CollisionPrism).value() << '\n';
		}

		mCollisionTriangles.write(writer);

		// These come as a duo
		const u32 start = startChunk(writer, static_cast<u32>(EChunkType::CollisionGrid));
		writer.align();
		mCollisionGridInfo.mAABBMin.write(writer);
		mCollisionGridInfo.mAABBMax.write(writer);
		writer.writeF32(mCollisionGridInfo.mCellSize);
		writer.writeU32(mCollisionGridInfo.mCellCountX);
		writer.writeU32(mCollisionGridInfo.mCellCountY);
		writer.writeU32(static_cast<u32>(mCollisionGridInfo.mGroups.size()));

		for (CollGroup& group : mCollisionGridInfo.mGroups) {
			group.write(writer);
		}

		for (s32& i : mCollisionGridInfo.mGroupIndices) {
			writer.writeS32(i);
		}
		writer.align();
		finishChunk(writer, start);
	}

	// Finalise writing with 0xFFFF chunk and append any INI file
	finishChunk(writer, startChunk(writer, static_cast<u32>(EChunkType::EndOfFile)));

	if (!mEndOfFileData.empty()) {
		if (mVerbosePrint) {
			std::cout << "Writing 0xffff, " << MOD::getChunkName(EChunkType::EndOfFile).value() << '\n';
		}

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

	mEmptyChunks.clear();
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

std::optional<std::string_view> MOD::getChunkName(u32 opcode)
{
	if (gChunkNames.contains(opcode)) {
		return gChunkNames.at(opcode);
	}

	return std::nullopt;
}

std::optional<std::string_view> MOD::getChunkName(EChunkType chunkType)
{
	if (gChunkNames.contains(static_cast<u32>(chunkType))) {
		return gChunkNames.at(static_cast<u32>(chunkType));
	}

	return std::nullopt;
}
