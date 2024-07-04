#include <commands.hpp>
#include <common.hpp>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <util/misc.hpp>
#include <util/vector_reader.hpp>

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

	util::fstream_reader reader(0, util::fstream_reader::Endianness::Big);
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

		util::fstream_reader txeReader(0, util::fstream_reader::Endianness::Big);
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

	if (!gModFile.mVertices.size() && !gModFile.mVertexNormals.size() && !gModFile.mCollisionTriangles.mCollInfo.size()) {
		std::cout << "Loaded file has nothing to export!" << std::endl;
		return;
	}

	const std::string& filename = gTokeniser.isEnd() ? gModFileName + ".obj" : gTokeniser.next();
	std::ofstream os(filename);
	if (!os.is_open()) {
		std::cout << "Error can't open " << filename << std::endl;
		return;
	}

	MODHeader& header = gModFile.mHeader;
	os << "# Date " << (u32)header.mDateTime.mYear << " " << (u32)header.mDateTime.mMonth << " " << (u32)header.mDateTime.mDay << std::endl;

	if (gModFile.mVertices.size()) {
		os << "\n# Vertices" << std::endl;
		for (const Vector3f& vpos : gModFile.mVertices) {
			os << "v " << vpos << std::endl;
		}
	}

	if (gModFile.mVertexNormals.size()) {
		os << "\n# Vertex normals" << std::endl;
		for (const Vector3f& vnrm : gModFile.mVertexNormals) {
			os << "vn " << vnrm << std::endl;
		}
	}

	for (u32 i = 0; i < gModFile.mTextureCoords.size(); i++) {
		std::vector<Vector2f>& coords = gModFile.mTextureCoords[i];
		if (!coords.size()) {
			continue;
		}

		os << "\n# Texture coordinate " << i << std::endl;
		for (const Vector2f& vt : coords) {
			os << "vt " << vt.x << " " << vt.y << std::endl;
		}
	}

	if (gModFile.mCollisionTriangles.mCollInfo.size()) {
		os << "\no collision_mesh" << std::endl;
		// TODO: Implement this
	}

	/*os << "# Mesh data" << std::endl;
	for (std::size_t i = 0; i < gModFile.m_meshes.size(); i++) {
	    const Mesh& mesh = gModFile.m_meshes[i];
	    os << "o mesh" << i << std::endl;
	    for (const MeshPacket& packet : mesh.m_packets) {
	        for (const DisplayList& dlist : packet.m_displaylists) {
	            util::vector_reader reader(dlist.m_dlistData, 0,
	util::vector_reader::Endianness::Big);

	            while (reader.getRemaining()) {
	                u8 opcode = reader.readU8();

	                if (opcode != 0x98 && opcode != 0xA0) {
	                    continue;
	                }

	                enum Vtx {
	                    PosMatIdx,
	                    Tex0MatIdx,
	                    Tex1MatIdx,
	                    Tex2MatIdx,
	                    Tex3MatIdx,
	                    Tex4MatIdx,
	                    Tex5MatIdx,
	                    Tex6MatIdx,
	                    Tex7MatIdx,
	                    Position,
	                    Normal,
	                    Color0,
	                    Color1,
	                    Tex0Coord,
	                    Tex1Coord,
	                    Tex2Coord,
	                    Tex3Coord,
	                    Tex4Coord,
	                    Tex5Coord,
	                    Tex6Coord,
	                    Tex7Coord
	                };

	                enum VtxFmt { NOT_PRESENT, DIRECT, INDEX8, INDEX16 };

	                u16 faceCount = reader.readU16();
	                for (u32 j = 0; j < faceCount; j++) { }
	            }
	        }
	    }
	}*/

	os.flush();
	os.close();

	std::cout << "Done!" << std::endl;
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
		for (u32 i = 0; i < 10; i++) {
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

	std::string filename = "material_dump.txt";
	if (!gTokeniser.isEnd()) {
		filename = gTokeniser.next();
	} else {
		std::cout << "Filename not provided, defaulting to material_dump.txt!\n";
	}

	if (gModFile.mMaterials.mMaterials.empty() && gModFile.mMaterials.mTevEnvironmentInfo.empty()) {
		std::cout << "Loaded file has no materials!\n";
		return;
	}

	std::filesystem::path filepath { filename };
	std::ofstream output(filepath, std::ios::out | std::ios::trunc);
	if (!output.is_open()) {
		std::cout << "Error can't open " << filepath << '\n';
		return;
	}

	output << std::setprecision(6) << std::fixed;
	output << "MATERIAL_FILE\n";

	if (!gModFile.mMaterials.mMaterials.empty()) {
		output << "MAT_SECTION\n";
		u32 matIdx = 0;
		for (const auto& mat : gModFile.mMaterials.mMaterials) {
			output << "MAT " << matIdx++ << '\n';
			output << mat;
		}
	}

	if (!gModFile.mMaterials.mTevEnvironmentInfo.empty()) {
		output << "TEV_SECTION\n";
		u32 tevIdx = 0;
		for (const auto& tev : gModFile.mMaterials.mTevEnvironmentInfo) {
			output << "TEV " << tevIdx++ << '\n';
			output << tev;
		}
	}

	output.close();
	std::cout << "Done!\n";
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
			faces.push_back({ std::stoul(tokeniser.next()), std::stoul(tokeniser.next()), std::stoul(tokeniser.next()) });
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