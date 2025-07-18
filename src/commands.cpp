#include <filesystem>
#include <iostream>
#include <iomanip>
#include <map>
#include <functional>
#include <set>
#include <numeric>

#include "util/vector_reader.hpp"
#include "util/misc.hpp"
#include "common.hpp"
#include "commands.hpp"

using namespace mat;

namespace cmd {
MOD gModFile;
std::string gModFileName;
util::tokeniser gTokeniser;

namespace mod {
inline bool isModFileOpen() { return static_cast<bool>(!gModFileName.empty()); }

void importMod()
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

void exportMod()
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

void resetModel()
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

void listChunks()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!" << '\n';
		return;
	}

	const int nameWidth = 20;

	auto printRow
	    = [&](const std::string& name, const std::string& data) { std::cout << std::left << std::setw(nameWidth) << name << data << '\n'; };

	std::cout << "\n--- File Information ---\n";
	printRow("MOD File:", gModFileName);
	printRow("File Size:", std::to_string(std::filesystem::file_size(gModFileName)) + " bytes");

	// --- Header (Always Present) ---
	std::cout << "\n--- Header ---\n";
	const auto& header = gModFile.mHeader;
	std::stringstream dateStream;
	dateStream << header.mDateTime.mYear << "/" << std::setfill('0') << std::setw(2) << static_cast<u32>(header.mDateTime.mMonth) << "/"
	           << std::setfill('0') << std::setw(2) << static_cast<u32>(header.mDateTime.mDay);
	printRow("Creation Date:", dateStream.str());

	std::stringstream flagsStream;
	flagsStream << "0x" << std::hex << header.mFlags << std::dec << " (" << MaterialFlagsToString(header.mFlags) << ")";
	printRow("Flags:", flagsStream.str());

	// --- Geometry Section ---
	u32 totalTexCoords = 0;
	for (const auto& coords : gModFile.mTextureCoords) {
		totalTexCoords += static_cast<u32>(coords.size());
	}
	const bool hasGeometry = !gModFile.mVertices.empty() || !gModFile.mVertexNormals.empty() || !gModFile.mVertexNbt.empty()
	                      || !gModFile.mVertexColours.empty() || totalTexCoords > 0;

	if (hasGeometry) {
		std::cout << "\n--- Geometry ---\n";
		if (!gModFile.mVertices.empty()) {
			Vector3f minBounds = gModFile.mVertices[0], maxBounds = gModFile.mVertices[0];
			for (const auto& v : gModFile.mVertices) {
				minBounds.x = std::min(minBounds.x, v.x);
				minBounds.y = std::min(minBounds.y, v.y);
				minBounds.z = std::min(minBounds.z, v.z);
				maxBounds.x = std::max(maxBounds.x, v.x);
				maxBounds.y = std::max(maxBounds.y, v.y);
				maxBounds.z = std::max(maxBounds.z, v.z);
			}
			std::stringstream boundsStr;
			boundsStr << std::fixed << std::setprecision(2) << gModFile.mVertices.size() << " [Bounds: (" << minBounds.x << ", "
			          << minBounds.y << ", " << minBounds.z << ") to (" << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z
			          << ")]";
			printRow("Vertices (0x10):", boundsStr.str());
		}
		if (!gModFile.mVertexNormals.empty())
			printRow("Normals (0x11):", std::to_string(gModFile.mVertexNormals.size()));
		if (!gModFile.mVertexNbt.empty())
			printRow("NBT (0x12):", std::to_string(gModFile.mVertexNbt.size()) + " [Normal/Binormal/Tangent vectors]");
		if (!gModFile.mVertexColours.empty())
			printRow("Colors (0x13):", std::to_string(gModFile.mVertexColours.size()));
		if (totalTexCoords > 0) {
			for (u32 i = 0; i < 8; ++i) {
				if (!gModFile.mTextureCoords[i].empty()) {
					std::stringstream title;
					title << "TexCoord" << i << " (0x" << std::hex << (0x18 + i) << std::dec << "):";
					printRow(title.str(), std::to_string(gModFile.mTextureCoords[i].size()));
				}
			}
		}
	}

	// --- Materials & Textures Section ---
	const bool hasMtlAndTex = !gModFile.mTextures.empty() || !gModFile.mTextureAttributes.empty() || !gModFile.mMaterials.mMaterials.empty()
	                       || !gModFile.mMaterials.mTevEnvironmentInfo.empty();

	if (hasMtlAndTex) {
		std::cout << "\n--- Materials & Textures ---\n";
		if (!gModFile.mTextures.empty()) {
			std::map<TextureFormat, int> formatCounts;
			size_t totalBytes = 0;
			for (const auto& tex : gModFile.mTextures) {
				formatCounts[tex.mFormat]++;
				totalBytes += tex.mImageData.size();
			}
			std::stringstream texStr;
			texStr << gModFile.mTextures.size() << " textures, " << totalBytes << " bytes total. Formats: ";
			for (auto const& [format, count] : formatCounts) {
				// This part would ideally convert format enum to string
				texStr << count << "x" << static_cast<int>(format) << " ";
			}
			printRow("Textures (0x20):", texStr.str());
		}
		if (!gModFile.mTextureAttributes.empty())
			printRow("Tex Attrs (0x22):", std::to_string(gModFile.mTextureAttributes.size()));
		if (!gModFile.mMaterials.mMaterials.empty()) {
			std::set<s16> usedMaterials;
			for (const auto& joint : gModFile.mJoints) {
				for (const auto& poly : joint.mLinkedPolygons) {
					if (poly.mMaterialIndex >= 0)
						usedMaterials.insert(poly.mMaterialIndex);
				}
			}
			printRow("Materials (0x30):", std::to_string(gModFile.mMaterials.mMaterials.size()) + " ("
			                                  + std::to_string(usedMaterials.size()) + " used in geometry)");
		}
		if (!gModFile.mMaterials.mTevEnvironmentInfo.empty())
			printRow("TEV Environments:", std::to_string(gModFile.mMaterials.mTevEnvironmentInfo.size()));
	}

	// --- Rigging & Animation Section ---
	const bool hasRigging = !gModFile.mVertexMatrices.empty() || !gModFile.mVertexEnvelopes.empty();
	if (hasRigging) {
		std::cout << "\n--- Rigging & Animation ---\n";
		if (!gModFile.mVertexMatrices.empty()) {
			u32 partialWeights = 0;
			for (const auto& vtxMtx : gModFile.mVertexMatrices) {
				if (vtxMtx.mHasPartialWeights)
					partialWeights++;
			}
			printRow("Vtx Matrices (0x40):", std::to_string(gModFile.mVertexMatrices.size()) + " [" + std::to_string(partialWeights)
			                                     + " partial, " + std::to_string(gModFile.mVertexMatrices.size() - partialWeights)
			                                     + " full]");
		}
		if (!gModFile.mVertexEnvelopes.empty()) {
			u32 totalInfluences = 0;
			for (const auto& env : gModFile.mVertexEnvelopes) {
				totalInfluences += static_cast<u32>(env.mIndices.size());
			}
			double avgInfluences = static_cast<double>(totalInfluences) / gModFile.mVertexEnvelopes.size();
			std::stringstream envStr;
			envStr << gModFile.mVertexEnvelopes.size() << " [Avg " << std::fixed << std::setprecision(1) << avgInfluences
			       << " influences/vertex]";
			printRow("Matrix Envelopes (0x41):", envStr.str());
		}
	}

	// --- Mesh & Skeleton Section ---
	const bool hasMeshAndSkel = !gModFile.mMeshes.empty() || !gModFile.mJoints.empty() || !gModFile.mJointNames.empty();
	if (hasMeshAndSkel) {
		std::cout << "\n--- Mesh & Skeleton ---\n";
		if (!gModFile.mMeshes.empty()) {
			u32 totalPackets = 0, totalDisplayLists = 0;
			for (const auto& mesh : gModFile.mMeshes) {
				totalPackets += static_cast<u32>(mesh.mPackets.size());
				for (const auto& packet : mesh.mPackets) {
					totalDisplayLists += static_cast<u32>(packet.mDisplayLists.size());
				}
			}
			printRow("Meshes (0x50):", std::to_string(gModFile.mMeshes.size()) + " [" + std::to_string(totalPackets) + " packets, "
			                               + std::to_string(totalDisplayLists) + " display lists]");
		}
		if (!gModFile.mJoints.empty()) {
			u32 totalPolygons = 0;
			for (const auto& joint : gModFile.mJoints) {
				totalPolygons += static_cast<u32>(joint.mLinkedPolygons.size());
			}
			printRow("Joints (0x60):", std::to_string(gModFile.mJoints.size()) + " [" + std::to_string(totalPolygons) + " polygon links]");
		}
		if (!gModFile.mJointNames.empty())
			printRow("Joint Names (0x61):", std::to_string(gModFile.mJointNames.size()));
	}

	// --- Collision Section ---
	if (!gModFile.mCollisionTriangles.mCollInfo.empty()) {
		std::cout << "\n--- Collision ---\n";
		printRow("Coll Tris (0x100):", std::to_string(gModFile.mCollisionTriangles.mCollInfo.size()));
		if (!gModFile.mCollisionTriangles.mRoomInfo.empty())
			printRow("Rooms:", std::to_string(gModFile.mCollisionTriangles.mRoomInfo.size()));
		if (gModFile.mCollisionGridInfo.mCellCountX > 0) {
			std::stringstream gridStr;
			gridStr << "Enabled [" << gModFile.mCollisionGridInfo.mCellCountX << "x" << gModFile.mCollisionGridInfo.mCellCountY
			        << " cells, " << gModFile.mCollisionGridInfo.mGroups.size() << " groups]";
			printRow("Coll Grid (0x110):", gridStr.str());
		}
	}

	// --- Miscellaneous Section ---
	if (!gModFile.mEndOfFileData.empty()) {
		std::cout << "\n--- Miscellaneous ---\n";
		printRow("End of File (0xFFFF):", std::to_string(gModFile.mEndOfFileData.size()) + " bytes [INI/Config data]");
	}

	// --- Empty Chunks Tracking (for debugging) ---
	if (!gModFile.mEmptyChunks.empty()) {
		std::cout << "\n--- Diagnostics ---\n";
		std::stringstream emptyStream;
		bool first = true;
		for (const auto& chunk : gModFile.mEmptyChunks) {
			if (!first)
				emptyStream << ", ";
			emptyStream << "0x" << std::hex << static_cast<u32>(chunk) << std::dec;
			first = false;
		}
		printRow("Empty Chunks Found:", emptyStream.str());
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
			texCoordOffsets[i]    = totalTexCoords;
			const auto& texCoords = gModFile.mTextureCoords[i];

			if (!texCoords.empty()) {
				anyTexCoordsExported = true;
				for (const auto& vt : texCoords) {
					os << "vt " << vt.x << " " << (1.0f - vt.y) << "\n";
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

				float r = mat.mColourInfo.mDiffuseColour.r / 255.0f;
				float g = mat.mColourInfo.mDiffuseColour.g / 255.0f;
				float b = mat.mColourInfo.mDiffuseColour.b / 255.0f;
				float a = mat.mColourInfo.mDiffuseColour.a / 255.0f;

				mtlFile << "Kd " << r << " " << g << " " << b << "\n";
				mtlFile << "d " << a << "\n";

				if (mat.mFlags & static_cast<u32>(mat::MaterialFlags::IsEnabled)) {
					if (mat.mLightingInfo.mFlags & static_cast<u32>(mat::LightingInfoFlags::EnableColor0)) {
						mtlFile << "Ka 0.2 0.2 0.2\n";
					}
					if (mat.mLightingInfo.mFlags & static_cast<u32>(mat::LightingInfoFlags::EnableSpecular)) {
						mtlFile << "Ks 0.5 0.5 0.5\n";
						mtlFile << "Ns 32.0\n";
					}
				}

				if (mat.mFlags & static_cast<u32>(mat::MaterialFlags::TransparentBlend)) {
					mtlFile << "illum 4\n";
				} else if (mat.mFlags & static_cast<u32>(mat::MaterialFlags::AlphaClip)) {
					mtlFile << "illum 3\n";
				} else {
					mtlFile << "illum 2\n";
				}

				if (mat.mTextureIndex >= 0 && mat.mTextureIndex < gModFile.mTextures.size()) {
					mtlFile << "# Texture index: " << mat.mTextureIndex << "\n";
				}

				mtlFile << "\n";
			}
			mtlFile.close();
		}
	}

	// Parse and export mesh data using DisplayListReader
	os << "# Mesh data\n";
	u32 meshIndex  = 0;
	u32 totalFaces = 0;

	std::set<s16> usedMaterials;

	for (const auto& mesh : gModFile.mMeshes) {
		os << "g mesh_" << meshIndex << "\n";
		os << "# Bone index: " << mesh.mBoneIndex << "\n";
		os << "# Vertex descriptor: 0x" << std::hex << mesh.mVtxDescriptor << std::dec << "\n";

		const bool hasNormal = (!gModFile.mVertexNormals.empty() || !gModFile.mVertexNbt.empty());

		std::vector<bool> hasTexCoord(8);
		for (int i = 0; i < 8; i++) {
			hasTexCoord[i] = (mesh.mVtxDescriptor & (1 << (i + 3))) != 0;
		}

		for (size_t packetIdx = 0; packetIdx < mesh.mPackets.size(); ++packetIdx) {
			const auto& packet = mesh.mPackets[packetIdx];

			// Handle material assignment
			s16 currentMaterialIndex = -1;
			if (!packet.mIndices.empty()) {
				s16 potentialMatIdx = packet.mIndices[0];
				if (potentialMatIdx >= 0 && potentialMatIdx < gModFile.mMaterials.mMaterials.size()) {
					currentMaterialIndex = potentialMatIdx;
					usedMaterials.insert(currentMaterialIndex);
					os << "usemtl material_" << currentMaterialIndex << "\n";
				}
			}

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

			// Process display lists using DisplayListReader
			for (const auto& dlist : packet.mDisplayLists) {
				util::vector_reader reader(dlist.mData, 0, util::vector_reader::Endianness::Big);
				DisplayListReader dlReader(reader, mesh.mVtxDescriptor);
				auto batches = dlReader.parse();

				// Export all triangles from parsed batches
				for (const auto& batch : batches) {
					for (const auto& tri : batch.getTriangles()) {
						// Build face string for each vertex
						std::vector<std::string> faceVertices;

						for (int v = 0; v < 3; ++v) {
							const VertexAttrib& vertex = tri[v];
							std::string vertexStr;

							// Position index (always present, 1-based for OBJ)
							vertexStr = std::to_string(vertex.mPosition + 1);

							// Find first valid texcoord
							u32 texIdx          = 0;
							bool hasAnyTexCoord = false;
							for (int i = 0; i < 8; i++) {
								if (hasTexCoord[i] && vertex.mTexcoords[i] != 0) {
									texIdx         = static_cast<u32>(vertex.mTexcoords[i]) + texCoordOffsets[i];
									hasAnyTexCoord = true;
									break;
								}
							}

							// Build vertex string (pos/tex/normal format)
							if (hasAnyTexCoord && hasNormal) {
								vertexStr += "/" + std::to_string(texIdx + 1) + "/" + std::to_string(vertex.mNormal + 1);
							} else if (hasAnyTexCoord) {
								vertexStr += "/" + std::to_string(texIdx + 1);
							} else if (hasNormal) {
								vertexStr += "//" + std::to_string(vertex.mNormal + 1);
							}

							faceVertices.push_back(vertexStr);
						}

						// Write the triangle
						os << "f " << faceVertices[0] << " " << faceVertices[1] << " " << faceVertices[2] << "\n";
						totalFaces++;
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
		dlist.mFlags = DLFlags::Back;

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

	std::string filename = gTokeniser.isEnd() ? "./materials.json" : gTokeniser.next();

	if (!mat::saveMaterialsToFile(filename, gModFile.mMaterials.mMaterials, gModFile.mMaterials.mTevEnvironmentInfo)) {
		std::cout << "[FAIL]" << std::endl;
	} else {
		std::cout << "[SUCCESS]" << std::endl;
	}
}

void importMaterials()
{
	std::string filename = gTokeniser.isEnd() ? "./materials.json" : gTokeniser.next();

	std::vector<Material> materials;
	std::vector<TEVInfo> tevInfos;

	if (mat::loadMaterialsFromFile(filename, materials, tevInfos)) {
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

		for (const char* blockName : { "<VTX_POS>", "<DEFORMED_XYZ>", "<ENVELOPE_XYZ>" }) {
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
		for (const char* blockName : { "<VTX_NRM>", "<ENVELOPE_NRM>" }) {
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
				switch (dlist.mFlags) {
				case DLFlags::Front:
					os << "\tface\tfront\n";
					break;
				case DLFlags::Back:
					os << "\tface\tback\n";
					break;
				case DLFlags::Both:
					os << "\tface\tboth\n";
					break;
				default:
					os << "\tface\tboth\n"; // Default case
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

void exportCollision()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!\n";
		return;
	}

	if (gModFile.mCollisionTriangles.mCollInfo.empty()) {
		std::cout << "Loaded file has no collision data!\n";
		return;
	}

	std::string filename = gTokeniser.isEnd() ? "./collision.json" : gTokeniser.next();

	if (!collision::saveCollisionToFile(filename, gModFile.mCollisionTriangles, gModFile.mCollisionGridInfo)) {
		std::cout << "Failed to export collision data to " << filename << "\n";
		return;
	}

	if (gModFile.mVerbosePrint) {
		std::cout << "Successfully exported collision data to " << filename << "\n";
		std::cout << "Exported " << gModFile.mCollisionTriangles.mCollInfo.size() << " collision triangles\n";
		std::cout << "Exported " << gModFile.mCollisionTriangles.mRoomInfo.size() << " room entries\n";
		std::cout << "Exported collision grid with " << gModFile.mCollisionGridInfo.mGroups.size() << " groups\n";

		// Analyze collision data
		std::map<int, int> surfaceTypeCounts;
		std::map<int, int> slipCodeCounts;
		int baldTriangles = 0;

		for (const auto& tri : gModFile.mCollisionTriangles.mCollInfo) {
			collision::MapCodeBitfield mapCode(static_cast<u32>(tri.mMapCode));
			surfaceTypeCounts[mapCode.getAttribute()]++;
			slipCodeCounts[mapCode.getSlipCode()]++;
			if (mapCode.getBaldFlag())
				baldTriangles++;
		}

		std::cout << "\nCollision Analysis:\n";
		std::cout << "Surface Types:\n";
		for (const auto& [type, count] : surfaceTypeCounts) {
			std::cout << "  " << collision::EnumConverters::MapAttributesToString(type) << ": " << count << " triangles\n";
		}

		std::cout << "Slip Codes:\n";
		for (const auto& [slip, count] : slipCodeCounts) {
			std::cout << "  " << collision::EnumConverters::SlipCodeToString(slip) << ": " << count << " triangles\n";
		}

		std::cout << "Bald (non-walkable) triangles: " << baldTriangles << "\n";
	}
}

void importCollision()
{
	if (!isModFileOpen()) {
		std::cout << "You haven't opened a MOD file!\n";
		return;
	}

	std::string filename = gTokeniser.isEnd() ? "./collision.json" : gTokeniser.next();

	CollTriInfo triangles;
	CollGrid grid;

	if (collision::loadCollisionFromFile(filename, triangles, grid)) {
		// Update the global mod file with imported collision data
		gModFile.mCollisionTriangles = std::move(triangles);
		gModFile.mCollisionGridInfo  = std::move(grid);

		if (gModFile.mVerbosePrint) {
			std::cout << "Successfully imported collision data from " << filename << "\n";
			std::cout << "Loaded " << gModFile.mCollisionTriangles.mCollInfo.size() << " collision triangles\n";
			std::cout << "Loaded " << gModFile.mCollisionTriangles.mRoomInfo.size() << " room entries\n";
			std::cout << "Loaded collision grid with " << gModFile.mCollisionGridInfo.mGroups.size() << " groups\n";
		}
	} else {
		std::cout << "Failed to import collision data from " << filename << "\n";
	}
}

} // namespace mod
} // namespace cmd