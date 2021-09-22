#include <cmds.hpp>
#include <filesystem>
#include <iostream>
#include <util/misc.hpp>

namespace cmd {
MOD gModFile;
std::string gModFileName;
util::tokeniser gTokeniser;

namespace mod {
    static inline const bool isModOpen() { return static_cast<bool>(gModFileName.size() != 0); }

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
        if (!isModOpen()) {
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
        if (!isModOpen()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        gModFile.reset();
        gModFileName = "";
    }

    void importTexture()
    {
        if (!isModOpen()) {
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
            texture.m_width  = txeReader.readU16();
            texture.m_height = txeReader.readU16();
            texture.m_format = txeReader.readU16();
            txeReader.readU16();
            txeReader.readU32();
            for (u32 i = 0; i < 10; i++) {
                txeReader.readU16();
            }
            texture.m_imageData.resize(util::CalculateTxeSize(texture.m_format, texture.m_width, texture.m_height));
            txeReader.read(reinterpret_cast<char*>(texture.m_imageData.data()), texture.m_imageData.size());
            txeReader.close();
            gModFile.m_textures[toSwap] = texture;
        } catch (...) {
            std::cout << "Error while trying to swap textures!" << std::endl;
        }

        std::cout << "Done!" << std::endl;
    }

    void importIni()
    {
        if (!isModOpen()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        std::string filename = gTokeniser.next();
        std::ifstream inStream(filename);
        if (!inStream.is_open()) {
            std::cout << "Error can't open " << filename << std::endl;
            return;
        }

        gModFile.m_eofBytes.clear();
        std::string str((std::istreambuf_iterator<char>(inStream)), std::istreambuf_iterator<char>());
        for (const auto& c : str) {
            gModFile.m_eofBytes.push_back(c);
        }
    }

    void exportObj()
    {
        if (!isModOpen()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        if (!gModFile.m_vertices.size() && !gModFile.m_vnormals.size() && !gModFile.m_colltris.m_collinfo.size()) {
            std::cout << "Loaded file has nothing to export!" << std::endl;
            return;
        }

        std::string filename = gTokeniser.next();
        std::ofstream os(filename);
        if (!os.is_open()) {
            std::cout << "Error can't open " << filename << std::endl;
            return;
        }

        Header& header = gModFile.m_header;
        os << "# Date " << (u32)header.m_dateTime.m_year << " " << (u32)header.m_dateTime.m_month << " "
           << (u32)header.m_dateTime.m_day << std::endl;

        os << "o mesh0" << std::endl;

        os << "# Vertices" << std::endl;
        for (const Vector3f& vpos : gModFile.m_vertices) {
            os << "v " << vpos;
        }

        os << "# Vertex normals" << std::endl;
        for (const Vector3f& vnrm : gModFile.m_vnormals) {
            os << "vn " << vnrm;
        }

        os << "# Collision faces" << std::endl;
        for (const BaseCollTriInfo& colInfo : gModFile.m_colltris.m_collinfo) {
            os << "f " << colInfo.m_indice.x + 1 << " " << colInfo.m_indice.y + 1 << " " << colInfo.m_indice.z + 1
               << " " << std::endl;
        }
        os.flush();
        os.close();

        std::cout << "Done!" << std::endl;
    }

    void exportTextures()
    {
        if (!isModOpen()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        if (!gModFile.m_textures.size()) {
            std::cout << "Loaded MOD file has no textures" << std::endl;
            return;
        }

        std::string pathStr = std::filesystem::path(gTokeniser.next()).string();
        if (!pathStr.ends_with('/')) {
            pathStr += "/";
        }

        if (!std::filesystem::exists(pathStr)) {
            std::filesystem::create_directory(pathStr);
        }

        u32 i = 0;
        for (Texture& tex : gModFile.m_textures) {
            util::fstream_writer writer(util::fstream_writer::Endianness::Big);
            std::string fileName = pathStr + "tex" + std::to_string(i++) + ".txe";
            std::cout << "Writing " << fileName << std::endl;
            writer.open(fileName);
            if (!writer.is_open()) {
                std::cout << "Error unable to open " << fileName << std::endl;
                return;
            }

            writer.writeU16(tex.m_width);
            writer.writeU16(tex.m_height);
            writer.writeU16(tex.m_format);
            writer.writeU16(0);

            writer.writeU32(0);
            writer.writeU32(0);
            writer.writeU32(0);
            writer.writeU32(0);
            writer.writeU32(0);

            writer.write(reinterpret_cast<char*>(tex.m_imageData.data()), tex.m_imageData.size());
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

        const std::string& filename = gTokeniser.next();
        if (!filename.size()) {
            std::cout << "Filename not provided!" << std::endl;
        }

        if (!gModFile.m_materials.m_materials.size() && !gModFile.m_materials.m_texEnvironments.size()) {
            std::cout << "Loaded file has no materials!" << std::endl;
            return;
        }

        std::ostringstream oss = {};
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
        if (!isModOpen()) {
            std::cout << "You haven't opened a MOD file!" << std::endl;
            return;
        }

        std::string filename = gTokeniser.next();
        std::ofstream outStream(filename);
        if (!outStream.is_open()) {
            std::cout << "Error can't open " << filename << std::endl;
            return;
        }

        outStream.write(reinterpret_cast<char*>(gModFile.m_eofBytes.data()), gModFile.m_eofBytes.size());
        outStream.close();
    }
} // namespace mod

} // namespace cmd