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
#include <set>

namespace cmd {
MOD gModFile;
std::string gModFileName;
util::tokeniser gTokeniser;

namespace mod {
inline bool isModFileOpen() { return static_cast<bool>(!gModFileName.empty()); }

void loadFile()
{
	const std::string& filename = gTokeniser.next();
	if (filename.empty()) {
		std::cout << "Filename not provided!" << '\n';
	}

	util::fstream_reader reader;
	reader.open(filename, std::ios_base::binary);
	if (!reader.is_open()) {
		std::cout << "Unable to open " << filename << '\n';
		return;
	}
	gModFileName = filename;

	reader.seekg(0, std::ios_base::beg);
	gModFile.read(reader);
	reader.close();

	if (gModFile.mVerbosePrint) {
		std::cout << "Done!" << '\n';
	}
}

void writeFile()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	const std::string& filename = gTokeniser.next();
	util::fstream_writer writer;
	writer.open(filename, std::ios_base::binary);
	if (!writer.is_open()) {
		std::cout << "Unable to open " << filename << '\n';
		return;
	}

	gModFile.write(writer);
	writer.close();

	if (gModFile.mVerbosePrint) {
		std::cout << "Done!" << '\n';
	}
}

void resetActiveModel()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	gModFile.reset();
	gModFileName = "";

	if (gModFile.mVerbosePrint) {
		std::cout << "Done!" << '\n';
	}
}

void importTexture()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	if (gModFile.mTextures.empty()) {
		std::cout << "Loaded MOD file has no textures" << '\n';
		return;
	}

	for (u32 i = 0; i < gModFile.mTextures.size(); i++) {
		std::cout << "Texture [" << i << "]" << '\n';
	}
	std::cout << "Which one do you want to swap? (number): ";

	std::string input;
	std::getline(std::cin, input);

	try {
		const u32 toSwap = std::stoi(input);
		if (toSwap >= gModFile.mTextures.size()) {
			std::cout << "Error given index is incorrect!" << '\n';
			return;
		}

		std::cout << "Path of the TXE file you want to replace Texture " << toSwap << " with: ";
		std::getline(std::cin, input);
		if (!std::filesystem::exists(input)) {
			std::cout << "Error invalid path given!" << '\n';
			return;
		}

		if (std::filesystem::path(input).extension() != ".txe") {
			std::cout << "Error path is not a TXE file!" << '\n';
			return;
		}

		util::fstream_reader txeReader;
		txeReader.open(input, std::ios_base::binary);
		if (!txeReader.is_open()) {
			std::cout << "Error couldn't open file!" << '\n';
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
		std::cout << "Error while trying to swap textures!" << '\n';
	}

	std::cout << "Done!" << '\n';
}

void importIni()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	const std::string& filename = gTokeniser.next();
	std::ifstream inStream(filename);
	if (!inStream.is_open()) {
		std::cout << "Error can't open " << filename << '\n';
		return;
	}

	gModFile.mEndOfFileData.clear();
	std::string str((std::istreambuf_iterator<char>(inStream)), std::istreambuf_iterator<char>());
	for (const auto& c : str) {
		gModFile.mEndOfFileData.push_back(c);
	}

	std::cout << "Done!" << '\n';
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

	// Export texcoords
	std::vector<u32> texCoordOffsets(8);
	{
		u32 totalTexCoords        = 0;
		bool anyTexCoordsExported = false;

		os << "# Texture coordinates (all sets merged)\n";
		for (int i = 0; i < 8; ++i) {
			texCoordOffsets[i]    = totalTexCoords; // Store the starting offset for this set
			const auto& texCoords = gModFile.mTextureCoords[i];

			if (!texCoords.empty()) {
				anyTexCoordsExported = true;
				for (const auto& vt : texCoords) {
					os << "vt " << vt.x << " " << (1.0f - vt.y) << "\n"; // Flip Y for OBJ format
				}

				totalTexCoords += static_cast<u32>(texCoords.size());
			}
		}

		if (anyTexCoordsExported) {
			os << "\n";
		}
	}

	// Create material library reference if we have materials
	if (!gModFile.mMaterials.mMaterials.empty()) {
		std::string mtlFilename = std::filesystem::path(filename).stem().string() + ".mtl";
		os << "mtllib " << mtlFilename << "\n\n";

		// Export material file
		std::ofstream mtlFile(std::filesystem::path(filename).parent_path() / mtlFilename);
		if (mtlFile.is_open()) {
			for (size_t i = 0; i < gModFile.mMaterials.mMaterials.size(); ++i) {
				const auto& mat = gModFile.mMaterials.mMaterials[i];
				mtlFile << "newmtl material_" << i << "\n";

				// Convert color from 0-255 to 0-1 range
				float r = mat.mColourInfo.mDiffuseColour.r / 255.0f;
				float g = mat.mColourInfo.mDiffuseColour.g / 255.0f;
				float b = mat.mColourInfo.mDiffuseColour.b / 255.0f;
				float a = mat.mColourInfo.mDiffuseColour.a / 255.0f;

				mtlFile << "Kd " << r << " " << g << " " << b << "\n";
				mtlFile << "d " << a << "\n"; // Transparency

				// Add ambient and specular based on lighting info
				if (mat.mFlags & static_cast<u32>(mat::MaterialFlags::IsEnabled)) {
					// Check lighting flags
					if (mat.mLightingInfo.mFlags & static_cast<u32>(mat::LightingInfoFlags::EnableColor0)) {
						mtlFile << "Ka 0.2 0.2 0.2\n"; // Default ambient
					}
					if (mat.mLightingInfo.mFlags & static_cast<u32>(mat::LightingInfoFlags::EnableSpecular)) {
						mtlFile << "Ks 0.5 0.5 0.5\n"; // Default specular
						mtlFile << "Ns 32.0\n";        // Specular exponent
					}
				}

				// Check material flags for transparency/blending
				if (mat.mFlags & static_cast<u32>(mat::MaterialFlags::TransparentBlend)) {
					mtlFile << "illum 4\n"; // Transparent material
				} else if (mat.mFlags & static_cast<u32>(mat::MaterialFlags::AlphaClip)) {
					mtlFile << "illum 3\n"; // Alpha test material
				} else {
					mtlFile << "illum 2\n"; // Basic lighting with specular
				}

				// Reference texture if available
				if (mat.mTextureIndex >= 0 && mat.mTextureIndex < gModFile.mTextures.size()) {
					mtlFile << "# Texture index: " << mat.mTextureIndex << "\n";

					// TODO: export texture and reference it here
					// mtlFile << "map_Kd texture_" << mat.mTextureIndex << ".png\n";
				}

				mtlFile << "\n";
			}
			mtlFile.close();
		}
	}

	// Parse and export mesh data
	os << "# Mesh data\n";
	u32 meshIndex  = 0;
	u32 totalFaces = 0;

	// Track which materials are actually used by meshes
	std::set<s16> usedMaterials;

	for (const auto& mesh : gModFile.mMeshes) {
		os << "g mesh_" << meshIndex << "\n";
		os << "# Bone index: " << mesh.mBoneIndex << "\n";
		os << "# Vertex descriptor: 0x" << std::hex << mesh.mVtxDescriptor << std::dec << "\n";

		// Parse vertex descriptor
		const bool hasPNMTXIDX  = (mesh.mVtxDescriptor & 0x1) != 0;
		const bool hasTEXMTXIDX = (mesh.mVtxDescriptor & 0x2) != 0;
		const bool hasColor     = (mesh.mVtxDescriptor & 0x4) != 0;
		const bool hasNormal    = (!gModFile.mVertexNormals.empty() || !gModFile.mVertexNbt.empty());

		std::vector<bool> hasTexCoord(8);
		for (int i = 0; i < 8; i++) {
			hasTexCoord[i] = (mesh.mVtxDescriptor & (1 << (i + 3))) != 0;
		}

		// Process each packet
		for (size_t packetIdx = 0; packetIdx < mesh.mPackets.size(); ++packetIdx) {
			const auto& packet = mesh.mPackets[packetIdx];

			// The packet indices often contain material indices
			// Check if first index is a valid material index
			s16 currentMaterialIndex = -1;
			if (!packet.mIndices.empty()) {
				s16 potentialMatIdx = packet.mIndices[0];
				if (potentialMatIdx >= 0 && potentialMatIdx < gModFile.mMaterials.mMaterials.size()) {
					currentMaterialIndex = potentialMatIdx;
					usedMaterials.insert(currentMaterialIndex);
					os << "usemtl material_" << currentMaterialIndex << "\n";
				}
			}

			// Look for material index in joint-material-polygon associations
			if (currentMaterialIndex == -1 && mesh.mBoneIndex < gModFile.mJoints.size()) {
				const Joint& joint = gModFile.mJoints[mesh.mBoneIndex];
				for (const auto& matPoly : joint.mLinkedPolygons) {
					if (static_cast<u32>(matPoly.mMeshIndex) != meshIndex) {
						continue;
					}

					currentMaterialIndex = matPoly.mMaterialIndex;
					if (currentMaterialIndex >= 0 && currentMaterialIndex < gModFile.mMaterials.mMaterials.size()) {
						usedMaterials.insert(currentMaterialIndex);
						os << "usemtl material_" << currentMaterialIndex << "\n";
					}

					break;
				}
			}

			// Process display lists
			for (const auto& dlist : packet.mDisplayLists) {
				util::vector_reader reader(dlist.mData, 0, util::vector_reader::Endianness::Big);

				while (reader.getRemaining() > 0) {
					const u8 opcode = reader.readU8();

					// GX primitive types
					if (opcode == 0x90 || opcode == 0x98 || opcode == 0xA0 || opcode == 0xA8 || opcode == 0xB0 || opcode == 0xB8) {

						u16 vertexCount = reader.readU16();
						if (vertexCount < 3)
							continue;

						std::vector<std::string> faceVertices;

						// Read all vertices for this primitive
						for (u16 v = 0; v < vertexCount; v++) {
							std::string vertexStr;

							// Skip matrix indices if present
							if (hasPNMTXIDX)
								reader.readU8();
							if (hasTEXMTXIDX)
								reader.readU8();

							// Position index (always present)
							u16 posIdx = reader.readU16();
							vertexStr  = std::to_string(posIdx + 1);

							// Normal index
							u16 nrmIdx = 0;
							if (hasNormal) {
								nrmIdx = reader.readU16();
							}

							// Color index
							if (hasColor) {
								reader.readU16(); // Skip for now
							}

							// Texture coordinate indices
							u32 texIdx          = 0;
							bool hasAnyTexCoord = false;
							for (int i = 0; i < 8; i++) {
								if (hasTexCoord[i]) {
									u16 localTexIdx = reader.readU16();

									// Use the first valid tex coord found for this vertex.
									if (!hasAnyTexCoord) {
										texIdx         = static_cast<u32>(localTexIdx) + texCoordOffsets[i];
										hasAnyTexCoord = true;
									}
								}
							}

							// Build vertex string (pos/tex/normal format)
							if (hasAnyTexCoord && hasNormal) {
								vertexStr += "/" + std::to_string(texIdx + 1) + "/" + std::to_string(nrmIdx + 1);
							} else if (hasAnyTexCoord) {
								vertexStr += "/" + std::to_string(texIdx + 1);
							} else if (hasNormal) {
								vertexStr += "//" + std::to_string(nrmIdx + 1);
							}

							faceVertices.push_back(vertexStr);
						}

						// Convert primitives to triangles
						switch (opcode) {
						case 0x90: // Triangles
						case 0xA8: {
							for (u16 i = 0; i < vertexCount; i += 3) {
								if (i + 2 < vertexCount) {
									os << "f " << faceVertices[i] << " " << faceVertices[i + 1] << " " << faceVertices[i + 2] << "\n";
									totalFaces++;
								}
							}
							break;
						}
						case 0x98: { // Triangle strip
							for (u16 i = 0; i < vertexCount - 2; i++) {
								if (i % 2 == 0) {
									os << "f " << faceVertices[i] << " " << faceVertices[i + 1] << " " << faceVertices[i + 2] << "\n";
								} else {
									os << "f " << faceVertices[i + 2] << " " << faceVertices[i + 1] << " " << faceVertices[i] << "\n";
								}
								totalFaces++;
							}
							break;
						}
						case 0xA0: { // Triangle fan
							for (u16 i = 1; i < vertexCount - 1; i++) {
								os << "f " << faceVertices[0] << " " << faceVertices[i] << " " << faceVertices[i + 1] << "\n";
								totalFaces++;
							}
							break;
						}
						case 0xB0: // Quads
						case 0xB8: {
							for (u16 i = 0; i < vertexCount; i += 4) {
								if (i + 3 < vertexCount) {
									// Convert quad to two triangles
									os << "f " << faceVertices[i] << " " << faceVertices[i + 1] << " " << faceVertices[i + 2] << "\n";
									os << "f " << faceVertices[i] << " " << faceVertices[i + 2] << " " << faceVertices[i + 3] << "\n";
									totalFaces += 2;
								}
							}
							break;
						}
						}
					}
				}
			}
		}
		os << "\n";
		meshIndex++;
	}

	// Export collision mesh if present
	if (!gModFile.mCollisionTriangles.mCollInfo.empty()) {
		os << "g collision_mesh\n";
		os << "# Collision triangles (" << gModFile.mCollisionTriangles.mCollInfo.size() << ")\n";

		for (const auto& tri : gModFile.mCollisionTriangles.mCollInfo) {
			os << "f " << (tri.mVertexIndexA + 1) << " " << (tri.mVertexIndexB + 1) << " " << (tri.mVertexIndexC + 1) << "\n";
			totalFaces++;
		}
	}

	os.close();

	std::cout << "Done! Exported " << totalFaces << " faces to " << filename << std::endl;
	if (!gModFile.mMaterials.mMaterials.empty()) {
		std::cout << "Exported " << gModFile.mMaterials.mMaterials.size() << " materials (";
		std::cout << usedMaterials.size() << " used) to " << std::filesystem::path(filename).stem().string() << ".mtl" << std::endl;
	}
}

void exportDmd()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	const std::string& filename = gTokeniser.isEnd() ? gModFileName + ".dmd" : gTokeniser.next();
	std::ofstream os(filename);
	if (!os.is_open()) {
		std::cout << "Error can't open " << filename << '\n';
		return;
	}

	os << "<INFORMATION>\n{" << '\n';
	os << "\tnumjoints\t" << gModFile.mJoints.size() << '\n';
	os << "\tprimitive\tTriangleStrip" << '\n';
	os << "\tembossbump\t" << (!gModFile.mVertexNbt.empty() ? "on" : "off") << '\n';
	os << "\tscalingrule\tsoftimage" << '\n';
	os << "}" << '\n' << '\n';

	if (!gModFile.mVertices.empty()) {
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

		os << "<ENVELOPE_XYZ>\n{" << '\n';
		os << "\tsize\t" << gModFile.mVertices.size() << '\n';
		os << "\tmin\t" << minbounds << '\n';
		os << "\tmax\t" << maxbounds << '\n' << '\n';
		for (const Vector3f& c : gModFile.mVertices) {
			os << "\tfloat\t" << c << '\n';
		}
		os << "}" << '\n' << '\n';
	}

	if (!gModFile.mVertexNormals.empty()) {
		os << "<ENVELOPE_NRM>\n{" << '\n';
		os << "\tsize\t" << gModFile.mVertexNormals.size() << '\n' << '\n';
		for (const Vector3f& c : gModFile.mVertexNormals) {
			os << "\tfloat\t" << c << '\n';
		}
		os << "}" << '\n' << '\n';
	}

	for (u32 i = 0; i < gModFile.mTextureCoords.size(); i++) {
		std::vector<Vector2f>& texcoords = gModFile.mTextureCoords[i];
		if (texcoords.empty()) {
			continue;
		}

		os << "<TEXCOORD" << i << ">\n{" << '\n';
		os << "\tsize\t" << texcoords.size() << '\n';

		Vector2f minbounds = texcoords[0];
		Vector2f maxbounds = texcoords[0];
		for (const Vector2f& coord : texcoords) {
			maxbounds.x = std::max(maxbounds.x, coord.x);
			maxbounds.y = std::max(maxbounds.y, coord.y);

			minbounds.x = std::min(minbounds.x, coord.x);
			minbounds.y = std::min(minbounds.y, coord.y);
		}

		os << "\tmin\t" << minbounds.x << " " << minbounds.y << '\n';
		os << "\tmax\t" << maxbounds.x << " " << maxbounds.y << '\n' << '\n';

		os << std::fixed << std::setprecision(6);
		for (const Vector2f& coord : texcoords) {
			os << "\tfloat\t" << coord.x << " " << coord.y << '\n';
		}
		os << "}" << '\n' << '\n';
	}

	if (!gModFile.mVertexColours.empty()) {
		os << "<COLOR0>\n{" << '\n';
		os << "\tsize\t" << gModFile.mVertexColours.size() << '\n' << '\n';
		for (const ColourU8& c : gModFile.mVertexColours) {
			os << "\tbyte\t" << c << '\n';
		}
		os << "}" << '\n' << '\n';
	}

	os.flush();
	os.close();

	std::cout << "Done!" << '\n';
}

void exportTextures()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	if (gModFile.mTextures.empty()) {
		std::cout << "Loaded MOD file has no textures" << '\n';
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
		util::fstream_writer writer;
		const std::string& filename = pathStr + "tex" + std::to_string(i++) + ".txe";
		std::cout << "Writing " << filename << '\n';
		writer.open(filename);
		if (!writer.is_open()) {
			std::cout << "Error unable to open " << filename << '\n';
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

	std::cout << "Done!" << '\n';
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
	if (gModFile.mVerbosePrint) {
		std::cout << "Materials exported to " << filename << "\n";
	}
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
		if (gModFile.mVerbosePrint) {
			std::cout << "Successfully imported materials from " << filename << "\n";
			std::cout << "Loaded " << gModFile.mMaterials.mMaterials.size() << " materials and "
			          << gModFile.mMaterials.mTevEnvironmentInfo.size() << " TEV configurations\n";
		}
	} else if (gModFile.mVerbosePrint) {
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
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	if (gTokeniser.isEnd()) {
		std::cout << "Chunk not provided!" << '\n';
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
		std::cout << "Invalid chunk ID format!" << '\n';
		return;
	}

	// Is the chunk value a valid MOD chunk type?
	auto chunkName = MOD::getChunkName(chunkId);
	if (!chunkName.has_value()) {
		std::cout << "Chunk doesn't exist!" << '\n';
		return;
	}

	// Remove the chunk from the loaded MOD file
	bool failure         = false;
	const auto chunkType = static_cast<MOD::EChunkType>(chunkId);
	switch (chunkType) {
	case MOD::EChunkType::Header:
		std::cout << "Cannot delete Header chunk!" << '\n';
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
			if (gModFile.mVerbosePrint) {
				std::cout << "Deleted TexCoord" << idx << " chunk." << '\n';
			}
		} else {
			std::cout << "TexCoord index out of range!" << '\n';
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

	if (!failure && gModFile.mVerbosePrint) {
		std::cout << "Successfully deleted (" << chunkName.value() << ")" << '\n';
	}
}

void editHeader()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	std::cout << '\n';

	std::cout << "What would you like to edit?" << '\n';
	std::cout << "\t(1) date of creation" << '\n';
	std::cout << "\t(2) flags" << '\n';

	std::string input;
	std::getline(std::cin, input);

	std::cout << '\n';

	try {
		int choice = std::stoi(input);

		switch (choice) {
		case 1: {
			// Edit date
			std::cout << "Current date: " << gModFile.mHeader.mDateTime.mYear << "/" << (u32)gModFile.mHeader.mDateTime.mMonth << "/"
			          << (u32)gModFile.mHeader.mDateTime.mDay << '\n';

			std::cout << "Enter new year (e.g., 2025): ";
			std::getline(std::cin, input);
			u16 year = static_cast<u16>(std::stoi(input));

			std::cout << "Enter new month (1-12): ";
			std::getline(std::cin, input);
			u8 month = static_cast<u8>(std::stoi(input));

			std::cout << "Enter new day (1-31): ";
			std::getline(std::cin, input);
			u8 day = static_cast<u8>(std::stoi(input));

			// Validate input
			if (month < 1 || month > 12) {
				std::cout << "Invalid month! Must be between 1-12." << '\n';
				return;
			}
			if (day < 1 || day > 31) {
				std::cout << "Invalid day! Must be between 1-31." << '\n';
				return;
			}

			gModFile.mHeader.mDateTime.mYear  = year;
			gModFile.mHeader.mDateTime.mMonth = month;
			gModFile.mHeader.mDateTime.mDay   = day;

			std::cout << "Date updated to: " << gModFile.mHeader.mDateTime.mYear << "/" << (u32)gModFile.mHeader.mDateTime.mMonth << "/"
			          << (u32)gModFile.mHeader.mDateTime.mDay << '\n';
			break;
		}
		case 2: {
			// Edit flags
			std::cout << "Current flags: 0x" << std::hex << gModFile.mHeader.mFlags << std::dec << '\n';
			std::cout << "\t0x00 - None" << '\n';
			std::cout << "\t0x01 - UseNBT (Use Normal/Binormal/Tangent)" << '\n';
			std::cout << "\t0x02 - AllowCaching (Allow display list caching)" << '\n';
			std::cout << "\t0x04 - AlwaysRedraw (Force redraw every frame)" << '\n';
			std::cout << "\t0x10 - IsPlatform (Has platform collision)" << '\n';
			std::cout << "Enter new flags (hex format, e.g., 0x01 or decimal): ";

			std::getline(std::cin, input);
			u32 flags = 0;

			if (input.starts_with("0x") || input.starts_with("0X")) {
				flags = std::stoul(input, nullptr, 16);
			} else {
				flags = std::stoul(input);
			}

			if (!(flags & static_cast<u32>(MODFlags::UseNBT)) && !(flags & static_cast<u32>(MODFlags::AllowCaching))
			    && !(flags & static_cast<u32>(MODFlags::AlwaysRedraw)) && !(flags & static_cast<u32>(MODFlags::IsPlatform))) {
				std::cout << "Unable to change flags, you haven't provided any valid option!" << '\n';
				return;
			}

			gModFile.mHeader.mFlags = flags;

			std::cout << "Flags updated to: 0x" << std::hex << gModFile.mHeader.mFlags << std::dec << '\n';

			// Show which flags are set
			if (flags & static_cast<u32>(MODFlags::UseNBT)) {
				std::cout << "\t- UseNBT enabled" << '\n';
			}
			if (flags & static_cast<u32>(MODFlags::AllowCaching)) {
				std::cout << "\t- AllowCaching enabled" << '\n';
			}
			if (flags & static_cast<u32>(MODFlags::AlwaysRedraw)) {
				std::cout << "\t- AlwaysRedraw enabled" << '\n';
			}
			if (flags & static_cast<u32>(MODFlags::IsPlatform)) {
				std::cout << "\t- IsPlatform enabled" << '\n';
			}
			break;
		}
		default:
			std::cout << "Invalid choice! Please enter 1 or 2." << '\n';
			return;
		}

		if (gModFile.mVerbosePrint) {
			std::cout << "Header editing complete!" << '\n';
		}

	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << '\n';
	}
}

} // namespace mod

void objToDmd()
{
	if (gTokeniser.isEnd()) {
		std::cout << "Input filename not provided!" << '\n';
		return;
	}

	std::string input = gTokeniser.next();

	if (gTokeniser.isEnd()) {
		std::cout << "Output filename not provided, defaulting to out.dmd!" << '\n';
	}

	std::string output = gTokeniser.isEnd() ? "out.dmd" : gTokeniser.next();

	std::ifstream inputFile(input);
	if (!inputFile.is_open()) {
		std::cout << "Error can't open " << input << '\n';
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
			texcoords.emplace_back(std::stof(tokeniser.next()), std::stof(tokeniser.next()));
		} else if (first == "vn") {
			vnormals.emplace_back(std::stof(tokeniser.next()), std::stof(tokeniser.next()), std::stof(tokeniser.next()));
		} else if (first == "v") {
			vertices.emplace_back(std::stof(tokeniser.next()), std::stof(tokeniser.next()), std::stof(tokeniser.next()));
		} else if (first == "f") {
			faces.emplace_back(static_cast<u32>(std::stoul(tokeniser.next())), static_cast<u32>(std::stoul(tokeniser.next())),
			                   static_cast<u32>(std::stoul(tokeniser.next())));
		}
	}

	std::ofstream os(output);
	if (!os.is_open()) {
		std::cout << "Error can't open " << input << '\n';
		return;
	}

	os << "<INFORMATION>\n{";
	os << "\tmagnify\t1\n";
	os << "\tnumjoints\t0\n";
	os << "\tscalingrule\tsoftimage\n";
	os << "\tprimitive\tTriangleStrip\n";
	os << "\tembossbump\toff\n}\n";

	if (!vertices.empty()) { }
}
} // namespace cmd