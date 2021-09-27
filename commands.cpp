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
    static inline const bool isModFileOpen()
    {
        return static_cast<bool>(gModFileName.size() != 0);
    }

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

        if (!gModFile.m_textures.size()) {
            std::cout << "Loaded MOD file has no textures" << std::endl;
            return;
        }

        for (u32 i = 0; i < gModFile.m_textures.size(); i++) {
            std::cout << "Texture [" << i << "]" << std::endl;
        }
        std::cout << "Which one do you want to swap? (number): ";

        std::string input = "";
        std::getline(std::cin, input);

        try {
            const u32 toSwap = std::stoi(input);
            if (toSwap >= gModFile.m_textures.size()) {
                std::cout << "Error given index is incorrect!" << std::endl;
                return;
            }

            std::cout << "Path of the TXE file you want to replace Texture "
                      << toSwap << " with: ";
            std::getline(std::cin, input);
            if (!std::filesystem::exists(input)) {
                std::cout << "Error invalid path given!" << std::endl;
                return;
            }

            if (std::filesystem::path(input).extension() != ".txe") {
                std::cout << "Error path is not a TXE file!" << std::endl;
                return;
            }

            util::fstream_reader txeReader(
                0, util::fstream_reader::Endianness::Big);
            txeReader.open(input, std::ios_base::binary);
            if (!txeReader.is_open()) {
                std::cout << "Error couldn't open file!" << std::endl;
                return;
            }

            Texture texture;
            texture.m_width  = txeReader.readU16();
            texture.m_height = txeReader.readU16();
            texture.m_format = txeReader.readU16();
            txeReader.readU16();
            txeReader.readU32();
            for (u32 i = 0; i < 10; i++) {
                txeReader.readU16();
            }
            texture.m_imageData.resize(util::CalculateTxeSize(
                texture.m_format, texture.m_width, texture.m_height));
            txeReader.read(reinterpret_cast<char*>(texture.m_imageData.data()),
                           texture.m_imageData.size());
            txeReader.close();
            gModFile.m_textures[toSwap] = texture;
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

        gModFile.m_eofBytes.clear();
        std::string str((std::istreambuf_iterator<char>(inStream)),
                        std::istreambuf_iterator<char>());
        for (const auto& c : str) {
            gModFile.m_eofBytes.push_back(c);
        }

        std::cout << "Done!" << std::endl;
    }

    void exportObj()
    {
        if (!isModFileOpen()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        if (!gModFile.m_vertices.size() && !gModFile.m_vnormals.size()
            && !gModFile.m_colltris.m_collinfo.size()) {
            std::cout << "Loaded file has nothing to export!" << std::endl;
            return;
        }

        const std::string& filename
            = gTokeniser.isEnd() ? gModFileName + ".obj" : gTokeniser.next();
        std::ofstream os(filename);
        if (!os.is_open()) {
            std::cout << "Error can't open " << filename << std::endl;
            return;
        }

        MODHeader& header = gModFile.m_header;
        os << "# Date " << (u32)header.m_dateTime.m_year << " "
           << (u32)header.m_dateTime.m_month << " "
           << (u32)header.m_dateTime.m_day << std::endl;

        if (gModFile.m_vertices.size()) {
            os << "\n# Vertices" << std::endl;
            for (const Vector3f& vpos : gModFile.m_vertices) {
                os << "v " << vpos << std::endl;
            }
        }

        if (gModFile.m_vnormals.size()) {
            os << "\n# Vertex normals" << std::endl;
            for (const Vector3f& vnrm : gModFile.m_vnormals) {
                os << "vn " << vnrm << std::endl;
            }
        }

        for (u32 i = 0; i < gModFile.m_texcoords.size(); i++) {
            std::vector<Vector2f>& coords = gModFile.m_texcoords[i];
            if (!coords.size()) {
                continue;
            }

            os << "\n# Texture coordinate " << i << std::endl;
            for (const Vector2f& vt : coords) {
                os << "vt " << vt.x << " " << vt.y << std::endl;
            }
        }

        if (gModFile.m_colltris.m_collinfo.size()) {
            os << "\no collision_mesh" << std::endl;
            for (const BaseCollTriInfo& colInfo :
                 gModFile.m_colltris.m_collinfo) {
                os << "f " << colInfo.m_indice.x + 1 << " "
                   << colInfo.m_indice.y + 1 << " " << colInfo.m_indice.z + 1
                   << " " << std::endl;
            }
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

        const std::string& filename
            = gTokeniser.isEnd() ? gModFileName + ".dmd" : gTokeniser.next();
        std::ofstream os(filename);
        if (!os.is_open()) {
            std::cout << "Error can't open " << filename << std::endl;
            return;
        }

        os << "<INFORMATION>\n{" << std::endl;
        os << "\tnumjoints\t" << gModFile.m_joints.size() << std::endl;
        os << "\tprimitive\tTriangleStrip" << std::endl;
        os << "\tembossbump\t" << (gModFile.m_vertexnbt.size() ? "on" : "off")
           << std::endl;
        os << "\tscalingrule\tsoftimage" << std::endl;
        os << "}" << std::endl << std::endl;

        if (gModFile.m_vertices.size()) {
            Vector3f minbounds = gModFile.m_vertices[0];
            Vector3f maxbounds = gModFile.m_vertices[0];
            for (const Vector3f& vertex : gModFile.m_vertices) {
                maxbounds.x = std::max(maxbounds.x, vertex.x);
                maxbounds.y = std::max(maxbounds.y, vertex.y);
                maxbounds.z = std::max(maxbounds.z, vertex.z);

                minbounds.x = std::min(minbounds.x, vertex.x);
                minbounds.y = std::min(minbounds.y, vertex.y);
                minbounds.z = std::min(minbounds.z, vertex.z);
            }

            os << "<ENVELOPE_XYZ>\n{" << std::endl;
            os << "\tsize\t" << gModFile.m_vertices.size() << std::endl;
            os << "\tmin\t" << minbounds << std::endl;
            os << "\tmax\t" << maxbounds << std::endl << std::endl;
            for (const Vector3f& c : gModFile.m_vertices) {
                os << "\tfloat\t" << c << std::endl;
            }
            os << "}" << std::endl << std::endl;
        }

        if (gModFile.m_vnormals.size()) {
            os << "<ENVELOPE_NRM>\n{" << std::endl;
            os << "\tsize\t" << gModFile.m_vnormals.size() << std::endl
               << std::endl;
            for (const Vector3f& c : gModFile.m_vnormals) {
                os << "\tfloat\t" << c << std::endl;
            }
            os << "}" << std::endl << std::endl;
        }

        for (u32 i = 0; i < gModFile.m_texcoords.size(); i++) {
            std::vector<Vector2f>& texcoords = gModFile.m_texcoords[i];
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
            os << "\tmax\t" << maxbounds.x << " " << maxbounds.y << std::endl
               << std::endl;

            os << std::fixed << std::setprecision(6);
            for (const Vector2f& coord : texcoords) {
                os << "\tfloat\t" << coord.x << " " << coord.y << std::endl;
            }
            os << "}" << std::endl << std::endl;
        }

        if (gModFile.m_vcolours.size()) {
            os << "<COLOR0>\n{" << std::endl;
            os << "\tsize\t" << gModFile.m_vcolours.size() << std::endl
               << std::endl;
            for (const ColourU8& c : gModFile.m_vcolours) {
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

        if (!gModFile.m_textures.size()) {
            std::cout << "Loaded MOD file has no textures" << std::endl;
            return;
        }

        std::string pathStr = std::filesystem::path(
                                  gTokeniser.isEnd() ? "./" : gTokeniser.next())
                                  .string();
        if (!pathStr.ends_with('/')) {
            pathStr += "/";
        }

        if (!std::filesystem::exists(pathStr)) {
            std::filesystem::create_directory(pathStr);
        }

        u32 i = 0;
        for (Texture& tex : gModFile.m_textures) {
            util::fstream_writer writer(util::fstream_writer::Endianness::Big);
            const std::string& filename
                = pathStr + "tex" + std::to_string(i++) + ".txe";
            std::cout << "Writing " << filename << std::endl;
            writer.open(filename);
            if (!writer.is_open()) {
                std::cout << "Error unable to open " << filename << std::endl;
                return;
            }

            writer.writeU16(tex.m_width);
            writer.writeU16(tex.m_height);
            writer.writeU16(tex.m_format);
            writer.writeU16(0);
            writer.writeU32(0);
            for (u32 i = 0; i < 10; i++) {
                writer.writeU16(0);
            }

            writer.write(reinterpret_cast<char*>(tex.m_imageData.data()),
                         tex.m_imageData.size());
            writer.close();
        }

        std::cout << "Done!" << std::endl;
    }

    void exportMaterials()
    {
        if (!gModFileName.size()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        if (gTokeniser.isEnd()) {
            std::cout
                << "Filename not provided, defaulting to material_dump.txt!"
                << std::endl;
        }

        const std::string& filename
            = gTokeniser.isEnd() ? "material_dump.txt" : gTokeniser.next();

        if (!gModFile.m_materials.m_materials.size()
            && !gModFile.m_materials.m_texEnvironments.size()) {
            std::cout << "Loaded file has no materials!" << std::endl;
            return;
        }

        std::ostringstream oss = {};
        oss << std::setprecision(6) << std::fixed;
        oss << "MATERIAL_FILE" << std::endl;

        if (gModFile.m_materials.m_materials.size()) {
            oss << "MAT_SECTION" << std::endl;
        }

        u32 matIdx = 0;
        for (mat::Material& mat : gModFile.m_materials.m_materials) {
            oss << "MAT " << matIdx++ << std::endl;
            oss << mat;
        }

        if (gModFile.m_materials.m_texEnvironments.size()) {
            oss << "TEV_SECTION" << std::endl;
        }

        u32 tevIdx = 0;
        for (mat::TEVInfo& mat : gModFile.m_materials.m_texEnvironments) {
            oss << "TEV " << tevIdx++ << std::endl;
            oss << mat;
        }

        std::ofstream output(filename);
        if (!output.is_open()) {
            std::cout << "Error can't open " << filename << std::endl;
            return;
        }

        output.write(oss.str().c_str(), oss.str().size());
        output.close();

        std::cout << "Done!" << std::endl;
    }

    void exportIni()
    {
        if (!isModFileOpen()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        if (gTokeniser.isEnd()) {
            std::cout << "Filename not provided, defaulting to ini_dump.txt!"
                      << std::endl;
        }

        const std::string& filename
            = gTokeniser.isEnd() ? "ini_dump.txt" : gTokeniser.next();
        std::ofstream outStream(filename);
        if (!outStream.is_open()) {
            std::cout << "Error can't open " << filename << std::endl;
            return;
        }

        outStream.write(reinterpret_cast<char*>(gModFile.m_eofBytes.data()),
                        gModFile.m_eofBytes.size());
        outStream.close();

        std::cout << "Done!" << std::endl;
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
        std::cout << "Output filename not provided, defaulting to out.dmd!"
                  << std::endl;
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
            texcoords.push_back(
                { std::stof(tokeniser.next()), std::stof(tokeniser.next()) });
        } else if (first == "vn") {
            vnormals.push_back({ std::stof(tokeniser.next()),
                                 std::stof(tokeniser.next()),
                                 std::stof(tokeniser.next()) });
        } else if (first == "v") {
            vertices.push_back({ std::stof(tokeniser.next()),
                                 std::stof(tokeniser.next()),
                                 std::stof(tokeniser.next()) });
        } else if (first == "f") {
            faces.push_back({ std::stoul(tokeniser.next()),
                              std::stoul(tokeniser.next()),
                              std::stoul(tokeniser.next()) });
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