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
	gModFile.reset();

	reader.seekg(0, std::ios_base::beg);
	gModFile.read(reader);
	reader.close();

	if (gModFile.mVerbosePrint) {
		std::cout << "Done!" << '\n';
	}
}

void writeFile()
{
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

void importObj()
{
	if (gTokeniser.isEnd()) {
		std::cout << "OBJ filename not provided!" << std::endl;
		return;
	}

	std::string objFile = gTokeniser.next();
	std::ifstream inputFile(objFile);
	if (!inputFile.is_open()) {
		std::cout << "Error: can't open " << objFile << std::endl;
		return;
	}

	// Clear existing geometry data
	gModFile.mVertices.clear();
	gModFile.mVertexNormals.clear();
	for (auto& texCoords : gModFile.mTextureCoords) {
		texCoords.clear();
	}
	gModFile.mMeshes.clear();

	// Parse OBJ file
	std::vector<Vector3f> tempVertices;
	std::vector<Vector3f> tempNormals;
	std::vector<Vector2f> tempTexCoords;

	struct IndexedVertex {
		int posIdx;
		int texIdx;
		int nrmIdx;

		bool operator==(const IndexedVertex& other) const
		{
			return posIdx == other.posIdx && texIdx == other.texIdx && nrmIdx == other.nrmIdx;
		}
	};

	struct IndexedVertexHash {
		std::size_t operator()(const IndexedVertex& v) const
		{
			return std::hash<int>()(v.posIdx) ^ (std::hash<int>()(v.texIdx) << 1) ^ (std::hash<int>()(v.nrmIdx) << 2);
		}
	};

	std::unordered_map<IndexedVertex, u16, IndexedVertexHash> vertexMap;
	std::vector<std::vector<u16>> meshFaces;

	std::string line;
	while (std::getline(inputFile, line)) {
		util::tokeniser tokeniser(line);
		if (tokeniser.isEnd())
			continue;

		std::string type = tokeniser.next();

		if (type == "v") {
			float x = std::stof(tokeniser.next());
			float y = std::stof(tokeniser.next());
			float z = std::stof(tokeniser.next());
			tempVertices.push_back({ x, y, z });
		} else if (type == "vn") {
			float x = std::stof(tokeniser.next());
			float y = std::stof(tokeniser.next());
			float z = std::stof(tokeniser.next());
			tempNormals.push_back({ x, y, z });
		} else if (type == "vt") {
			float u = std::stof(tokeniser.next());
			float v = 1.0f - std::stof(tokeniser.next()); // Flip Y for MOD format
			tempTexCoords.push_back({ u, v });
		} else if (type == "f") {
			std::vector<u16> faceIndices;

			while (!tokeniser.isEnd()) {
				std::string vertexStr = tokeniser.next();
				IndexedVertex vertex  = { -1, -1, -1 };

				// Parse vertex indices
				size_t firstSlash = vertexStr.find('/');
				if (firstSlash == std::string::npos) {
					vertex.posIdx = std::stoi(vertexStr) - 1;
				} else {
					vertex.posIdx = std::stoi(vertexStr.substr(0, firstSlash)) - 1;

					size_t secondSlash = vertexStr.find('/', firstSlash + 1);
					if (secondSlash == std::string::npos) {
						vertex.texIdx = std::stoi(vertexStr.substr(firstSlash + 1)) - 1;
					} else {
						if (secondSlash > firstSlash + 1) {
							vertex.texIdx = std::stoi(vertexStr.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1;
						}
						vertex.nrmIdx = std::stoi(vertexStr.substr(secondSlash + 1)) - 1;
					}
				}

				// Get or create vertex index
				auto it = vertexMap.find(vertex);
				u16 index;
				if (it == vertexMap.end()) {
					index             = static_cast<u16>(gModFile.mVertices.size());
					vertexMap[vertex] = index;

					// Add vertex data
					gModFile.mVertices.push_back(tempVertices[vertex.posIdx]);

					if (vertex.nrmIdx >= 0) {
						if (gModFile.mVertexNormals.size() < gModFile.mVertices.size()) {
							gModFile.mVertexNormals.resize(gModFile.mVertices.size());
						}
						gModFile.mVertexNormals[index] = tempNormals[vertex.nrmIdx];
					}

					if (vertex.texIdx >= 0) {
						if (gModFile.mTextureCoords[0].size() < gModFile.mVertices.size()) {
							gModFile.mTextureCoords[0].resize(gModFile.mVertices.size());
						}
						gModFile.mTextureCoords[0][index] = tempTexCoords[vertex.texIdx];
					}
				} else {
					index = it->second;
				}

				faceIndices.push_back(index);
			}

			meshFaces.push_back(faceIndices);
		}
	}

	inputFile.close();

	// Create mesh from faces
	if (!meshFaces.empty()) {
		Mesh mesh;
		mesh.mBoneIndex = 0;

		// Set vertex descriptor based on what data we have
		mesh.mVtxDescriptor = 0;
		if (!gModFile.mVertexNormals.empty()) {
			mesh.mVtxDescriptor |= (1 << 11); // Has normals
		}
		if (!gModFile.mTextureCoords[0].empty()) {
			mesh.mVtxDescriptor |= (1 << 3); // Has texcoord0
		}

		// Create a single packet with all faces
		MeshPacket packet;

		// Convert faces to triangle strips (simple conversion)
		DisplayList dlist;
		dlist.mFlags.byteView.cullMode = DLCullMode::Back;

		// Build display list data
		util::vector_reader::Endianness endian = util::vector_reader::Endianness::Big;

		for (const auto& face : meshFaces) {
			if (face.size() >= 3) {
				// Use triangle primitive (0xA8)
				dlist.mData.push_back(0xA8);

				// Vertex count (as u16)
				u16 vertCount = static_cast<u16>(face.size());
				dlist.mData.push_back((vertCount >> 8) & 0xFF);
				dlist.mData.push_back(vertCount & 0xFF);

				// Write vertex indices
				for (u16 idx : face) {
					// Position index
					dlist.mData.push_back((idx >> 8) & 0xFF);
					dlist.mData.push_back(idx & 0xFF);

					// Normal index (if present)
					if (!gModFile.mVertexNormals.empty()) {
						dlist.mData.push_back((idx >> 8) & 0xFF);
						dlist.mData.push_back(idx & 0xFF);
					}

					// Texture coordinate index (if present)
					if (!gModFile.mTextureCoords[0].empty()) {
						dlist.mData.push_back((idx >> 8) & 0xFF);
						dlist.mData.push_back(idx & 0xFF);
					}
				}
			}
		}

		dlist.mCommandCount = meshFaces.size();
		packet.mDisplayLists.push_back(dlist);

		mesh.mPackets.push_back(packet);
		gModFile.mMeshes.push_back(mesh);
	}

	std::cout << "Done! Imported " << gModFile.mVertices.size() << " vertices and " << meshFaces.size() << " faces from " << objFile
	          << std::endl;
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
			    && !(flags & static_cast<u32>(MODFlags::AlwaysRedraw)) && !(flags & static_cast<u32>(MODFlags::IsPlatform))
			    && !(flags & static_cast<u32>(MODFlags::UseClassicScaling))) {
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
			if (flags & static_cast<u32>(MODFlags::UseClassicScaling)) {
				std::cout << "\t- UseClassicScaling enabled" << '\n';
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

	os << std::fixed << std::setprecision(6);

	// <INFORMATION> section
	os << "<INFORMATION>\n{\n";
	os << "\tnumjoints\t" << gModFile.mJoints.size() << '\n';
	os << "\tprimitive\tTriangleStrip\n";
	os << "\tembossbump\t" << (gModFile.mHeader.mFlags & static_cast<u32>(MODFlags::UseNBT) ? "on" : "off") << '\n';
	os << "\tscalingrule\tsoftimage\n";
	os << "}\n\n";

	// <JOINT> sections
	for (size_t i = 0; i < gModFile.mJoints.size(); ++i) {
		const auto& joint = gModFile.mJoints[i];
		os << "<JOINT>\n{\n";

		// Index and name
		os << "\tindex\t" << i << '\n';
		std::string jointName = (i < gModFile.mJointNames.size()) ? gModFile.mJointNames[i] : "joint_" + std::to_string(i);
		os << "\tname\t" << jointName << '\n';

		// Parent
		os << "\tparent\t" << joint.mParentIndex << "\t(null)\n";

		// Kind (default to "mesh", or change if you store type elsewhere)
		os << "\tkind\tmesh\n";

		// Transform
		os << "\tscaling\t" << joint.mScale.x << " " << joint.mScale.y << " " << joint.mScale.z << '\n';
		os << "\trotation\t" << joint.mRotation.x << " " << joint.mRotation.y << " " << joint.mRotation.z << '\n';
		os << "\ttranslation\t" << joint.mPosition.x << " " << joint.mPosition.y << " " << joint.mPosition.z << '\n';

		// Bounding box and radius
		os << "\tvolume_min\t" << joint.mMinBounds.x << " " << joint.mMinBounds.y << " " << joint.mMinBounds.z << '\n';
		os << "\tvolume_max\t" << joint.mMaxBounds.x << " " << joint.mMaxBounds.y << " " << joint.mMaxBounds.z << '\n';
		os << "\tvolume_r\t" << joint.mVolumeRadius << '\n';

		// ndisplays and display lines
		if (!joint.mLinkedPolygons.empty()) {
			os << "\tndisplays\t" << joint.mLinkedPolygons.size() << '\n';
			for (const auto& poly : joint.mLinkedPolygons) {
				os << "\tdisplay\t" << poly.mMaterialIndex << " " << poly.mMeshIndex << '\n';
			}
		}

		os << "}\n\n";
	}

	// <TEX_ATTR> sections
	for (const auto& attr : gModFile.mTextureAttributes) {
		os << "<TEX_ATTR>\n{\n";
		os << "\tindex\t" << attr.mIndex << '\n';
		os << "\timage\t" << attr.mIndex << '\n';

		// Tiling: clamp or repeat per axis
		const char* uTiling = (attr.mTilingType & 0x01) ? "clamp" : "repeat";
		const char* vTiling = (attr.mTilingType & 0x0100) ? "clamp" : "repeat";
		os << "\ttiling\t" << uTiling << " " << vTiling << '\n';
		os << "}\n\n";
	}

	// <ENVELOPE> section
	if (!gModFile.mVertexEnvelopes.empty()) {
		os << "<ENVELOPE>\n{\n";
		os << "\tsize\t" << gModFile.mVertexEnvelopes.size() << '\n';
		for (const auto& env : gModFile.mVertexEnvelopes) {
			os << "\tevl_mtx_num\t" << env.mIndices.size();
			os << "\n\tevl_mtx_idx";
			for (const auto& index : env.mIndices) {
				os << "\t" << index;
			}
			os << "\n\tevl_mtx_wgt";
			for (const auto& weight : env.mWeights) {
				os << "\t" << weight;
			}
			os << " \n";
		}
		os << "}\n\n";
	}

	// <VTX_MATRIX> section
	if (!gModFile.mVertexMatrices.empty()) {
		os << "<VTX_MATRIX>\n{\n";
		os << "\tsize\t" << gModFile.mVertexMatrices.size() << '\n';
		for (const auto& vtxMtx : gModFile.mVertexMatrices) {
			os << "\tmatrix\t" << (vtxMtx.mHasPartialWeights ? "weight" : "full") << " " << vtxMtx.mIndex << '\n';
		}
		os << "}\n\n";
	}

	// <MATERIAL> section
	for (u32 i = 0; i < gModFile.mMaterials.mMaterials.size(); ++i) {
		const auto& mat = gModFile.mMaterials.mMaterials[i];
		os << "<MATERIAL>\n{\n";
		os << "\tindex\t" << i << '\n';
		os << "\tname\tmat_" << std::to_string(i) << '\n';

		// Mode
		std::string mode = "STD";
		switch ((mat.mFlags >> 8) & 0xFF) {
		case 1:
			mode = "STD"; // Opaque
			break;
		case 2:
			mode = "TEX"; // Alpha Test
			break;
		case 4:
			mode = "XLU"; // Transparent
			break;
		default:
			break;
		}
		os << "\tmode\t" << mode << '\n';

		// Diffuse
		const auto& c = mat.mColourInfo.mDiffuseColour;
		os << "\tdiffuse\t" << static_cast<int>(c.r) << " " << static_cast<int>(c.g) << " " << static_cast<int>(c.b) << " "
		   << static_cast<int>(c.a) << '\n';

		// Texture binding
		os << "\ttexture0\t" << mat.mTextureIndex << '\n';

		os << "}\n\n";
	}

	// <VTX_POS> and <DEFORMED_XYZ>
	if (!gModFile.mVertices.empty()) {
		Vector3f minbounds = gModFile.mVertices[0];
		Vector3f maxbounds = gModFile.mVertices[0];
		for (const Vector3f& vertex : gModFile.mVertices) {
			minbounds.x = std::min(minbounds.x, vertex.x);
			minbounds.y = std::min(minbounds.y, vertex.y);
			minbounds.z = std::min(minbounds.z, vertex.z);
			maxbounds.x = std::max(maxbounds.x, vertex.x);
			maxbounds.y = std::max(maxbounds.y, vertex.y);
			maxbounds.z = std::max(maxbounds.z, vertex.z);
		}

		for (const std::string& blockName : { "<VTX_POS>", "<DEFORMED_XYZ>", "<ENVELOPE_XYZ>" }) {
			os << blockName << "\n{\n";
			os << "\tsize\t" << gModFile.mVertices.size() << '\n';
			os << "\tmin\t" << minbounds.x << " " << minbounds.y << " " << minbounds.z << '\n';
			os << "\tmax\t" << maxbounds.x << " " << maxbounds.y << " " << maxbounds.z << '\n' << '\n';
			for (const Vector3f& v : gModFile.mVertices) {
				os << "\tfloat\t" << v.x << " " << v.y << " " << v.z << '\n';
			}
			os << "}\n\n";
		}
	}

	// <VTX_NRM> and <ENVELOPE_NRM>
	if (!gModFile.mVertexNormals.empty()) {
		for (const std::string& blockName : { "<VTX_NRM>", "<ENVELOPE_NRM>" }) {
			os << blockName << "\n{\n";
			os << "\tsize\t" << gModFile.mVertexNormals.size() << '\n' << '\n';
			for (const Vector3f& vn : gModFile.mVertexNormals) {
				os << "\tfloat\t" << vn.x << " " << vn.y << " " << vn.z << '\n';
			}
			os << "}\n\n";
		}
	}

	// <TEXCOORDn>
	for (u32 i = 0; i < gModFile.mTextureCoords.size(); ++i) {
		const auto& texCoords = gModFile.mTextureCoords[i];
		if (texCoords.empty())
			continue;

		os << "<TEXCOORD" << i << ">\n{\n";
		os << "\tsize\t" << texCoords.size() << '\n';
		os << "\n";
		for (const auto& vt : texCoords) {
			os << "\tfloat\t" << vt.x << " " << vt.y << '\n';
		}
		os << "}\n\n";
	}

	// <COLOR0>
	if (!gModFile.mVertexColours.empty()) {
		os << "<COLOR0>\n{\n";
		os << "\tsize\t" << gModFile.mVertexColours.size() << '\n' << '\n';
		for (const ColourU8& c : gModFile.mVertexColours) {
			os << "\tbyte\t" << static_cast<u32>(c.r) << " " << static_cast<u32>(c.g) << " " << static_cast<u32>(c.b) << " "
			   << static_cast<u32>(c.a) << '\n';
		}
		os << "}\n\n";
	}

	// <POLYGON>
	for (size_t i = 0; i < gModFile.mMeshes.size(); ++i) {
		const auto& mesh = gModFile.mMeshes[i];
		os << "<POLYGON>\n{\n";
		os << "\tindex\t" << i << '\n';
		os << "\tlight\ton\n";
		os << "\tembossbump\t" << ((mesh.mVtxDescriptor & 0x10000) ? "on" : "off") << '\n';
		os << "\tvcd\t";
		// Generate VCD line from descriptor
		// Bit 0: PNMTXIDX, Bit 2: Color0, Bit 3-10: TexCoord0-7
		os << ((mesh.mVtxDescriptor & 0x1) ? 1 : 0) << " 1 ";                                      // PNMTXIDX, Position
		os << ((!gModFile.mVertexNormals.empty() || !gModFile.mVertexNbt.empty()) ? 1 : 0) << " "; // Normal
		os << ((mesh.mVtxDescriptor & 0x4) ? 1 : 0) << " 0 ";                                      // Color0, Color1
		for (int j = 0; j < 8; ++j) {
			os << ((mesh.mVtxDescriptor & (1 << (j + 3))) ? 1 : 0) << " "; // TexCoord j
		}
		os << "0 0 0 0 0 0 0 0\n"; // Unused fields

		for (const auto& packet : mesh.mPackets) {
			os << "\tnmtx_lists\t" << (packet.mIndices.empty() ? 0 : 1) << '\n';
			os << "\tnmtxs\t" << packet.mIndices.size() << '\n';
			os << "\tmtx_list";
			for (s16 idx : packet.mIndices) {
				os << "\t" << idx;
			}
			os << '\n';

			for (const auto& dlist : packet.mDisplayLists) {
				switch (dlist.mFlags.byteView.cullMode) {
				case DLCullMode::Front:
					os << "\tface\tfront\n";
					break;
				case DLCullMode::Back:
					os << "\tface\tback\n";
					break;
				case DLCullMode::Both:
					os << "\tface\tboth\n";
					break;
				case DLCullMode::None:
					os << "\tface\tnone\n";
					break;
				}

				util::vector_reader reader(dlist.mData, 0, util::vector_reader::Endianness::Big);
				while (reader.getRemaining() > 0) {
					u8 opcode = reader.readU8();
					if (opcode >= 0x90 && opcode <= 0xB8) { // Is a primitive
						u16 vertCount = reader.readU16();
						os << "\tnodes\t" << vertCount << '\n';

						for (u16 v = 0; v < vertCount; ++v) {
							std::vector<int> vcd_data(21, -1);

							// Parse vertex data from stream based on descriptor
							if (mesh.mVtxDescriptor & 0x1)
								vcd_data[0] = reader.readU8(); // PNMTXIDX
							if (mesh.mVtxDescriptor & 0x2)
								reader.readU8(); // TEXMTXIDX (skip)

							vcd_data[1] = reader.readU16(); // POS
							if (!gModFile.mVertexNormals.empty() || !gModFile.mVertexNbt.empty())
								vcd_data[2] = reader.readU16(); // NRM
							if (mesh.mVtxDescriptor & 0x4)
								vcd_data[3] = reader.readU16(); // COL0

							for (int j = 0; j < 8; ++j) {
								if (mesh.mVtxDescriptor & (1 << (j + 3)))
									vcd_data[10 + j] = reader.readU16(); // TEX j
							}

							os << "\tvcd_dat";
							for (int val : vcd_data) {
								os << "\t" << val;
							}
							os << '\n';
						}
					}
				}
			}
		}
		os << "}\n\n";
	}

	os.close();
	if (gModFile.mVerbosePrint) {
		std::cout << "Done! Exported model to " << filename << std::endl;
	}
}

} // namespace mod
} // namespace cmd