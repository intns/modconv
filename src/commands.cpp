#include <commands.hpp>
#include <common.hpp>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <util/misc.hpp>
#include <util/vector_reader.hpp>
#include <common/material_io.hpp>
#include <map>
#include <functional>

namespace cmd {
MOD gModFile;
std::string gModFileName;
util::tokeniser gTokeniser;

namespace mod {
static inline const bool isModFileOpen() { return static_cast<bool>(gModFileName.size() != 0); }

void loadFile()
{
	const std::string& filename = gTokeniser.next();
	if (!filename.size()) {
		std::cout << "Filename not provided!" << std::endl;
	}

	util::fstream_reader reader;
	reader.open(filename, std::ios_base::binary);
	if (!reader.is_open()) {
		std::cout << "Unable to open " << filename << std::endl;
		return;
	}
	gModFileName = filename;

	reader.seekg(0, std::ios_base::beg);
	gModFile.read(reader);
	reader.close();

	std::cout << "Done!" << std::endl;
}

void writeFile()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	const std::string& filename = gTokeniser.next();
	util::fstream_writer writer(util::fstream_writer::Endianness::Big);
	writer.open(filename, std::ios_base::binary);
	if (!writer.is_open()) {
		std::cout << "Unable to open " << filename << std::endl;
		return;
	}

	gModFile.write(writer);
	writer.close();

	std::cout << "SANITY CHECK" << std::endl;

	std::cout << "Done!" << std::endl;
}

void closeFile()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	gModFile.reset();
	gModFileName = "";

	std::cout << "Done!" << std::endl;
}

void importTexture()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	if (!gModFile.mTextures.size()) {
		std::cout << "Loaded MOD file has no textures" << std::endl;
		return;
	}

	for (u32 i = 0; i < gModFile.mTextures.size(); i++) {
		std::cout << "Texture [" << i << "]" << std::endl;
	}
	std::cout << "Which one do you want to swap? (number): ";

	std::string input = "";
	std::getline(std::cin, input);

	try {
		const u32 toSwap = std::stoi(input);
		if (toSwap >= gModFile.mTextures.size()) {
			std::cout << "Error given index is incorrect!" << std::endl;
			return;
		}

		std::cout << "Path of the TXE file you want to replace Texture " << toSwap << " with: ";
		std::getline(std::cin, input);
		if (!std::filesystem::exists(input)) {
			std::cout << "Error invalid path given!" << std::endl;
			return;
		}

		if (std::filesystem::path(input).extension() != ".txe") {
			std::cout << "Error path is not a TXE file!" << std::endl;
			return;
		}

		util::fstream_reader txeReader;
		txeReader.open(input, std::ios_base::binary);
		if (!txeReader.is_open()) {
			std::cout << "Error couldn't open file!" << std::endl;
			return;
		}

		Texture texture;
		texture.mWidth  = txeReader.readU16();
		texture.mHeight = txeReader.readU16();
		texture.mFormat = static_cast<TextureFormat>(txeReader.readU16());
		txeReader.readU16();
		txeReader.readU32();
		for (u32 i = 0; i < 10; i++) {
			txeReader.readU16();
		}
		texture.mImageData.resize(util::CalculateTxeSize((u32)texture.mFormat, texture.mWidth, texture.mHeight));
		txeReader.read(reinterpret_cast<char*>(texture.mImageData.data()), texture.mImageData.size());
		txeReader.close();
		gModFile.mTextures[toSwap] = texture;
	} catch (...) {
		std::cout << "Error while trying to swap textures!" << std::endl;
	}

	std::cout << "Done!" << std::endl;
}

void importIni()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	const std::string& filename = gTokeniser.next();
	std::ifstream inStream(filename);
	if (!inStream.is_open()) {
		std::cout << "Error can't open " << filename << std::endl;
		return;
	}

	gModFile.mEndOfFileData.clear();
	std::string str((std::istreambuf_iterator<char>(inStream)), std::istreambuf_iterator<char>());
	for (const auto& c : str) {
		gModFile.mEndOfFileData.push_back(c);
	}

	std::cout << "Done!" << std::endl;
}

void exportObj()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	if (gModFile.mVertices.empty()) {
		std::cout << "Loaded file has no vertex data to export!" << std::endl;
		return;
	}

	const std::string& filename = gTokeniser.isEnd() ? gModFileName + ".obj" : gTokeniser.next();
	std::ofstream os(filename);
	if (!os.is_open()) {
		std::cout << "Error can't open " << filename << std::endl;
		return;
	}

	os << "# Exported with MODConv\n";
	os << "# Date: " << (u32)gModFile.mHeader.mDateTime.mYear << "/" << (u32)gModFile.mHeader.mDateTime.mMonth << "/"
	   << (u32)gModFile.mHeader.mDateTime.mDay << "\n\n";

	// Export vertices
	os << "# Vertices (" << gModFile.mVertices.size() << ")\n";
	for (const auto& v : gModFile.mVertices) {
		os << "v " << v.x << " " << v.y << " " << v.z << "\n";
	}
	os << "\n";

	// Export vertex normals
	if (!gModFile.mVertexNormals.empty()) {
		os << "# Vertex normals (" << gModFile.mVertexNormals.size() << ")\n";
		for (const auto& vn : gModFile.mVertexNormals) {
			os << "vn " << vn.x << " " << vn.y << " " << vn.z << "\n";
		}
		os << "\n";
	}

	// Export texture coordinates
	for (u32 i = 0; i < gModFile.mTextureCoords.size(); ++i) {
		if (gModFile.mTextureCoords[i].empty())
			continue;
		os << "# Texture coordinates " << i << " (" << gModFile.mTextureCoords[i].size() << ")\n";
		for (const auto& vt : gModFile.mTextureCoords[i]) {
			os << "vt " << vt.x << " " << (1.0f - vt.y) << "\n"; // Flip Y for OBJ format
		}
		os << "\n";
	}

	// Parse and export mesh data
	os << "# Mesh data\n";
	u32 meshIndex  = 0;
	u32 totalFaces = 0;

	for (const auto& mesh : gModFile.mMeshes) {
		os << "o mesh_" << meshIndex++ << "\n";
		os << "# Vertex descriptor: 0x" << std::hex << mesh.mVtxDescriptor << std::dec << "\n";

		// Parse vertex descriptor to understand attribute layout
		const bool hasPNMTXIDX  = (mesh.mVtxDescriptor & 0x1) != 0;
		const bool hasTEXMTXIDX = (mesh.mVtxDescriptor & 0x2) != 0;
		const bool hasColor     = (mesh.mVtxDescriptor & 0x4) != 0;
		std::vector<bool> hasTexCoord(8);
		for (int i = 0; i < 8; i++) {
			hasTexCoord[i] = (mesh.mVtxDescriptor & (1 << (i + 3))) != 0;
		}

		// Process each packet in the mesh
		for (const auto& packet : mesh.mPackets) {
			// Process display lists in the packet
			for (const auto& dlist : packet.mDisplayLists) {
				util::vector_reader reader(dlist.mData, 0, util::vector_reader::Endianness::Big);

				while (reader.getRemaining() > 0) {
					const u8 opcode = reader.readU8();

					// GX draw commands
					if (opcode == 0x98 || opcode == 0xA0 || opcode == 0xA8 || opcode == 0xB0 || opcode == 0xB8) {
						// Read vertex count
						u16 vertexCount = reader.readU16();

						// For triangle strips, we need at least 3 vertices
						if (vertexCount < 3)
							continue;

						std::vector<u16> posIndices;
						std::vector<u16> nrmIndices;
						std::vector<u16> texIndices[8];

						// Read vertex data
						for (u16 v = 0; v < vertexCount; v++) {
							// Read matrix indices if present (DIRECT format)
							if (hasPNMTXIDX) {
								reader.readU8(); // Position/Normal matrix index
							}
							if (hasTEXMTXIDX) {
								reader.readU8(); // Texture matrix index
							}

							// Position index (always present)
							posIndices.push_back(reader.readU16());

							// Normal index (or NBT index)
							if (!gModFile.mVertexNormals.empty() || !gModFile.mVertexNbt.empty()) {
								nrmIndices.push_back(reader.readU16());
							}

							// Color index
							if (hasColor) {
								reader.readU16(); // Skip color index for now
							}

							// Texture coordinate indices
							for (int i = 0; i < 8; i++) {
								if (hasTexCoord[i]) {
									texIndices[i].push_back(reader.readU16());
								}
							}
						}

						// Convert triangle strip to individual triangles
						if (opcode == 0x98) { // Triangle strip
							for (u16 i = 0; i < vertexCount - 2; i++) {
								// Alternate winding order for triangle strips
								if (i % 2 == 0) {
									os << "f";
									// Vertex 1
									os << " " << (posIndices[i] + 1);
									if (!nrmIndices.empty())
										os << "//" << (nrmIndices[i] + 1);
									else if (hasTexCoord[0])
										os << "/" << (texIndices[0][i] + 1);

									// Vertex 2
									os << " " << (posIndices[i + 1] + 1);
									if (!nrmIndices.empty())
										os << "//" << (nrmIndices[i + 1] + 1);
									else if (hasTexCoord[0])
										os << "/" << (texIndices[0][i + 1] + 1);

									// Vertex 3
									os << " " << (posIndices[i + 2] + 1);
									if (!nrmIndices.empty())
										os << "//" << (nrmIndices[i + 2] + 1);
									else if (hasTexCoord[0])
										os << "/" << (texIndices[0][i + 2] + 1);

									os << "\n";
								} else {
									os << "f";
									// Vertex 1 (reversed order for alternating triangles)
									os << " " << (posIndices[i + 2] + 1);
									if (!nrmIndices.empty())
										os << "//" << (nrmIndices[i + 2] + 1);
									else if (hasTexCoord[0])
										os << "/" << (texIndices[0][i + 2] + 1);

									// Vertex 2
									os << " " << (posIndices[i + 1] + 1);
									if (!nrmIndices.empty())
										os << "//" << (nrmIndices[i + 1] + 1);
									else if (hasTexCoord[0])
										os << "/" << (texIndices[0][i + 1] + 1);

									// Vertex 3
									os << " " << (posIndices[i] + 1);
									if (!nrmIndices.empty())
										os << "//" << (nrmIndices[i] + 1);
									else if (hasTexCoord[0])
										os << "/" << (texIndices[0][i] + 1);

									os << "\n";
								}
								totalFaces++;
							}
						} else if (opcode == 0xA0) { // Triangle fan
							for (u16 i = 1; i < vertexCount - 1; i++) {
								os << "f";
								// Center vertex
								os << " " << (posIndices[0] + 1);
								if (!nrmIndices.empty())
									os << "//" << (nrmIndices[0] + 1);
								else if (hasTexCoord[0])
									os << "/" << (texIndices[0][0] + 1);

								// Current vertex
								os << " " << (posIndices[i] + 1);
								if (!nrmIndices.empty())
									os << "//" << (nrmIndices[i] + 1);
								else if (hasTexCoord[0])
									os << "/" << (texIndices[0][i] + 1);

								// Next vertex
								os << " " << (posIndices[i + 1] + 1);
								if (!nrmIndices.empty())
									os << "//" << (nrmIndices[i + 1] + 1);
								else if (hasTexCoord[0])
									os << "/" << (texIndices[0][i + 1] + 1);

								os << "\n";
								totalFaces++;
							}
						}
						// Add more primitive types as needed (0xA8 = triangles, 0xB0 = quads, etc.)
					}
				}
			}
		}
		os << "\n";
	}

	// Export collision mesh if present
	if (!gModFile.mCollisionTriangles.mCollInfo.empty()) {
		os << "o collision_mesh\n";
		os << "# Collision triangles (" << gModFile.mCollisionTriangles.mCollInfo.size() << ")\n";
		for (const auto& tri : gModFile.mCollisionTriangles.mCollInfo) {
			os << "f " << (tri.mVertexIndexA + 1) << " " << (tri.mVertexIndexB + 1) << " " << (tri.mVertexIndexC + 1) << "\n";
			totalFaces++;
		}
	}

	os.close();
	std::cout << "Done! Exported " << totalFaces << " faces to " << filename << std::endl;
}

void exportDmd()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	const std::string& filename = gTokeniser.isEnd() ? gModFileName + ".dmd" : gTokeniser.next();
	std::ofstream os(filename);
	if (!os.is_open()) {
		std::cout << "Error can't open " << filename << std::endl;
		return;
	}

	os << "<INFORMATION>\n{" << std::endl;
	os << "\tnumjoints\t" << gModFile.mJoints.size() << std::endl;
	os << "\tprimitive\tTriangleStrip" << std::endl;
	os << "\tembossbump\t" << (gModFile.mVertexNbt.size() ? "on" : "off") << std::endl;
	os << "\tscalingrule\tsoftimage" << std::endl;
	os << "}" << std::endl << std::endl;

	if (gModFile.mVertices.size()) {
		Vector3f minbounds = gModFile.mVertices[0];
		Vector3f maxbounds = gModFile.mVertices[0];
		for (const Vector3f& vertex : gModFile.mVertices) {
			maxbounds.x = std::max(maxbounds.x, vertex.x);
			maxbounds.y = std::max(maxbounds.y, vertex.y);
			maxbounds.z = std::max(maxbounds.z, vertex.z);

			minbounds.x = std::min(minbounds.x, vertex.x);
			minbounds.y = std::min(minbounds.y, vertex.y);
			minbounds.z = std::min(minbounds.z, vertex.z);
		}

		os << "<ENVELOPE_XYZ>\n{" << std::endl;
		os << "\tsize\t" << gModFile.mVertices.size() << std::endl;
		os << "\tmin\t" << minbounds << std::endl;
		os << "\tmax\t" << maxbounds << std::endl << std::endl;
		for (const Vector3f& c : gModFile.mVertices) {
			os << "\tfloat\t" << c << std::endl;
		}
		os << "}" << std::endl << std::endl;
	}

	if (gModFile.mVertexNormals.size()) {
		os << "<ENVELOPE_NRM>\n{" << std::endl;
		os << "\tsize\t" << gModFile.mVertexNormals.size() << std::endl << std::endl;
		for (const Vector3f& c : gModFile.mVertexNormals) {
			os << "\tfloat\t" << c << std::endl;
		}
		os << "}" << std::endl << std::endl;
	}

	for (u32 i = 0; i < gModFile.mTextureCoords.size(); i++) {
		std::vector<Vector2f>& texcoords = gModFile.mTextureCoords[i];
		if (!texcoords.size()) {
			continue;
		}

		os << "<TEXCOORD" << i << ">\n{" << std::endl;
		os << "\tsize\t" << texcoords.size() << std::endl;

		Vector2f minbounds = texcoords[0];
		Vector2f maxbounds = texcoords[0];
		for (const Vector2f& coord : texcoords) {
			maxbounds.x = std::max(maxbounds.x, coord.x);
			maxbounds.y = std::max(maxbounds.y, coord.y);

			minbounds.x = std::min(minbounds.x, coord.x);
			minbounds.y = std::min(minbounds.y, coord.y);
		}

		os << "\tmin\t" << minbounds.x << " " << minbounds.y << std::endl;
		os << "\tmax\t" << maxbounds.x << " " << maxbounds.y << std::endl << std::endl;

		os << std::fixed << std::setprecision(6);
		for (const Vector2f& coord : texcoords) {
			os << "\tfloat\t" << coord.x << " " << coord.y << std::endl;
		}
		os << "}" << std::endl << std::endl;
	}

	if (gModFile.mVertexColours.size()) {
		os << "<COLOR0>\n{" << std::endl;
		os << "\tsize\t" << gModFile.mVertexColours.size() << std::endl << std::endl;
		for (const ColourU8& c : gModFile.mVertexColours) {
			os << "\tbyte\t" << c << std::endl;
		}
		os << "}" << std::endl << std::endl;
	}

	os.flush();
	os.close();

	std::cout << "Done!" << std::endl;
}

void exportTextures()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	if (!gModFile.mTextures.size()) {
		std::cout << "Loaded MOD file has no textures" << std::endl;
		return;
	}

	std::string pathStr = std::filesystem::path(gTokeniser.isEnd() ? "./" : gTokeniser.next()).string();
	if (!pathStr.ends_with('/')) {
		pathStr += "/";
	}

	if (!std::filesystem::exists(pathStr)) {
		std::filesystem::create_directory(pathStr);
	}

	u32 i = 0;
	for (Texture& tex : gModFile.mTextures) {
		util::fstream_writer writer(util::fstream_writer::Endianness::Big);
		const std::string& filename = pathStr + "tex" + std::to_string(i++) + ".txe";
		std::cout << "Writing " << filename << std::endl;
		writer.open(filename);
		if (!writer.is_open()) {
			std::cout << "Error unable to open " << filename << std::endl;
			return;
		}

		writer.writeU16(tex.mWidth);
		writer.writeU16(tex.mHeight);
		writer.writeU16((u16)tex.mFormat);
		writer.writeU16(0);
		writer.writeU32(0);
		for (u32 j = 0; j < 10; j++) {
			writer.writeU16(0);
		}

		writer.write(reinterpret_cast<char*>(tex.mImageData.data()), tex.mImageData.size());
		writer.close();
	}

	std::cout << "Done!" << std::endl;
}

void exportMaterials()
{
	if (gModFileName.empty()) {
		std::cout << "You haven't opened a MOD file!\n";
		return;
	}

	if (gModFile.mMaterials.mMaterials.empty() && gModFile.mMaterials.mTevEnvironmentInfo.empty()) {
		std::cout << "Loaded file has no materials!\n";
		return;
	}

	std::string filename = gTokeniser.isEnd() ? "./materials.txt" : gTokeniser.next();

	std::ofstream output(filename, std::ios::out | std::ios::trunc);
	if (!output.is_open()) {
		std::cout << "Error: can't open " << filename << '\n';
		return;
	}

	MaterialWriter writer(output);
	writer.writeLine("MATERIAL_FILE");

	// Write materials section
	if (!gModFile.mMaterials.mMaterials.empty()) {
		writer.writeLine("MAT_SECTION");
		for (size_t i = 0; i < gModFile.mMaterials.mMaterials.size(); ++i) {
			writer.writeLine("MAT " + std::to_string(i));
			writer.m_indentLevel = 1;
			writer.writeMaterial(gModFile.mMaterials.mMaterials[i]);
			writer.m_indentLevel = 0;
			writer.writeLine("");
		}
	}

	// Write TEV section
	if (!gModFile.mMaterials.mTevEnvironmentInfo.empty()) {
		writer.writeLine("TEV_SECTION");
		for (size_t i = 0; i < gModFile.mMaterials.mTevEnvironmentInfo.size(); ++i) {
			writer.writeLine("TEV " + std::to_string(i));
			writer.m_indentLevel = 1;
			writer.writeTEVInfo(gModFile.mMaterials.mTevEnvironmentInfo[i]);
			writer.m_indentLevel = 0;
			writer.writeLine("");
		}
	}

	output.close();
	std::cout << "Materials exported to " << filename << "\n";
}

void importMaterials()
{
	std::string filename = gTokeniser.isEnd() ? "./materials.txt" : gTokeniser.next();

	std::ifstream input(filename);
	if (!input.is_open()) {
		std::cout << "Error: can't open " << filename << '\n';
		return;
	}

	MaterialReader reader(input);
	std::vector<Material> materials;
	std::vector<TEVInfo> tevInfos;

	if (reader.readMaterialFile(materials, tevInfos)) {
		// Update the global mod file with imported materials
		gModFile.mMaterials.mMaterials          = std::move(materials);
		gModFile.mMaterials.mTevEnvironmentInfo = std::move(tevInfos);
		std::cout << "Successfully imported materials from " << filename << "\n";
		std::cout << "Loaded " << gModFile.mMaterials.mMaterials.size() << " materials and "
		          << gModFile.mMaterials.mTevEnvironmentInfo.size() << " TEV configurations\n";
	} else {
		std::cout << "Failed to import materials from " << filename << "\n";
	}

	input.close();
}

void exportIni()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!\n";
		return;
	}

	std::string filename = "ini_dump.txt";
	if (!gTokeniser.isEnd()) {
		filename = gTokeniser.next();
	} else {
		std::cout << "Filename not provided, defaulting to ini_dump.txt!\n";
	}

	std::filesystem::path filepath { filename };
	std::ofstream outStream(filepath, std::ios::binary);

	if (!outStream) {
		std::cout << "Error can't open " << filepath << '\n';
		return;
	}

	outStream.write(reinterpret_cast<const char*>(gModFile.mEndOfFileData.data()), gModFile.mEndOfFileData.size());

	std::cout << "Done!\n";
}

void deleteChunk()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << std::endl;
		return;
	}

	if (gTokeniser.isEnd()) {
		std::cout << "Chunk not provided!" << std::endl;
		return;
	}

	// Convert input (hex value) to value
	std::string input = gTokeniser.next();

	u32 chunkId = 0;
	try {
		if (input.starts_with("0x") || input.starts_with("0X")) {
			chunkId = std::stoul(input, nullptr, 16);
		} else {
			chunkId = std::stoul(input);
		}
	} catch (...) {
		std::cout << "Invalid chunk ID format!" << std::endl;
		return;
	}

	// Is the chunk value a valid MOD chunk type?
	auto chunkName = MOD::getChunkName(chunkId);
	if (!chunkName.has_value()) {
		std::cout << "Chunk doesn't exist!" << std::endl;
		return;
	}

	// Remove the chunk from the loaded MOD file
	bool failure         = false;
	const auto chunkType = static_cast<MOD::EChunkType>(chunkId);
	switch (chunkType) {
	case MOD::EChunkType::Header:
		std::cout << "Cannot delete Header chunk!" << std::endl;
		failure = true;
		break;
	case MOD::EChunkType::Vertex:
		gModFile.mVertices.clear();
		break;
	case MOD::EChunkType::VertexNormal:
		gModFile.mVertexNormals.clear();
		break;
	case MOD::EChunkType::VertexNBT:
		gModFile.mVertexNbt.clear();
		break;
	case MOD::EChunkType::VertexColour:
		gModFile.mVertexColours.clear();
		break;
	case MOD::EChunkType::TexCoord0:
	case MOD::EChunkType::TexCoord1:
	case MOD::EChunkType::TexCoord2:
	case MOD::EChunkType::TexCoord3:
	case MOD::EChunkType::TexCoord4:
	case MOD::EChunkType::TexCoord5:
	case MOD::EChunkType::TexCoord6:
	case MOD::EChunkType::TexCoord7: {
		u32 idx = chunkId - static_cast<u32>(MOD::EChunkType::TexCoord0);
		if (idx < gModFile.mTextureCoords.size()) {
			gModFile.mTextureCoords[idx].clear();
			std::cout << "Deleted TexCoord" << idx << " chunk." << std::endl;
		} else {
			std::cout << "TexCoord index out of range!" << std::endl;
			failure = true;
		}
		break;
	}
	case MOD::EChunkType::Texture:
		gModFile.mTextures.clear();
		break;
	case MOD::EChunkType::TextureAttribute:
		gModFile.mTextureAttributes.clear();
		break;
	case MOD::EChunkType::Material:
		gModFile.mMaterials.mMaterials.clear();
		gModFile.mMaterials.mTevEnvironmentInfo.clear();
		break;
	case MOD::EChunkType::VertexMatrix:
		gModFile.mVertexMatrices.clear();
		break;
	case MOD::EChunkType::MatrixEnvelope:
		gModFile.mVertexEnvelopes.clear();
		break;
	case MOD::EChunkType::Mesh:
		gModFile.mMeshes.clear();
		break;
	case MOD::EChunkType::Joint:
		gModFile.mJoints.clear();
		break;
	case MOD::EChunkType::JointName:
		gModFile.mJointNames.clear();
		break;
	case MOD::EChunkType::CollisionPrism:
		gModFile.mCollisionTriangles.mCollInfo.clear();
		gModFile.mCollisionTriangles.mRoomInfo.clear();
		break;
	case MOD::EChunkType::CollisionGrid:
		gModFile.mCollisionGridInfo.clear();
		break;
	case MOD::EChunkType::EndOfFile:
		gModFile.mEndOfFileData.clear();
		break;
	}

	if (!failure) {
		std::cout << "Successfully deleted (" << chunkName.value() << ")" << std::endl;
	}
}
} // namespace mod

void objToDmd()
{
	if (gTokeniser.isEnd()) {
		std::cout << "Input filename not provided!" << std::endl;
		return;
	}

	std::string input = gTokeniser.next();

	if (gTokeniser.isEnd()) {
		std::cout << "Output filename not provided, defaulting to out.dmd!" << std::endl;
	}

	std::string output = gTokeniser.isEnd() ? "out.dmd" : gTokeniser.next();

	std::ifstream inputFile(input);
	if (!inputFile.is_open()) {
		std::cout << "Error can't open " << input << std::endl;
		return;
	}

	std::vector<Vector2f> texcoords;
	std::vector<Vector3f> vnormals;
	std::vector<Vector3f> vertices;
	std::vector<Vector3i> faces;

	for (std::string line; std::getline(inputFile, line);) {
		util::tokeniser tokeniser(line);

		if (tokeniser.isEnd()) {
			continue;
		}

		std::string first = tokeniser.next();
		if (first.starts_with('#')) { // Skip comments
			continue;
		}

		if (first == "vt") {
			texcoords.push_back({ std::stof(tokeniser.next()), std::stof(tokeniser.next()) });
		} else if (first == "vn") {
			vnormals.push_back({ std::stof(tokeniser.next()), std::stof(tokeniser.next()), std::stof(tokeniser.next()) });
		} else if (first == "v") {
			vertices.push_back({ std::stof(tokeniser.next()), std::stof(tokeniser.next()), std::stof(tokeniser.next()) });
		} else if (first == "f") {
			faces.push_back({ static_cast<u32>(std::stoul(tokeniser.next())), static_cast<u32>(std::stoul(tokeniser.next())),
			                  static_cast<u32>(std::stoul(tokeniser.next())) });
		}
	}

	std::ofstream os(output);
	if (!os.is_open()) {
		std::cout << "Error can't open " << input << std::endl;
		return;
	}

	os << "<INFORMATION>\n{";
	os << "\tmagnify\t1\n";
	os << "\tnumjoints\t0\n";
	os << "\tscalingrule\tsoftimage\n";
	os << "\tprimitive\tTriangleStrip\n";
	os << "\tembossbump\toff\n}\n";

	if (vertices.size()) { }
}
} // namespace cmd