#include <MOD.hpp>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <util/tokeniser.hpp>

        return 2 * y * x;
    case 1:
        return 4 * (y * x / 8);
    case 3:
        return y * x / 2;
    case 4:
    case 5:
    case 8:
    case 9:
        return y * x;
    case 7:
        return 4 * y * x;
    default:
        return 0;
    }
}

#ifdef _MSC_VER
#include <conio.h>
static void exitfunc()
{
    std::cout << "Press any key to exit";
#ifdef _MSC_VER
    ((void)_getch());
#else
    ((void)getch());
#endif
}
#else
static void exitfunc()
{
    std::cout << "Press enter to exit";
    ((void)std::getchar());
}
#endif

int main(int argc, char** argv)
{
    std::atexit(exitfunc);

    if (argc != 2) {
        std::cout << "MODConv by Axiot\nUsage: MODConv.exe [commands file]";
        return EXIT_FAILURE;
    }

    std::ifstream cmdStream(argv[1], std::ios::in | std::ios::binary);
    const u32 cfSz = static_cast<u32>(std::filesystem::file_size(argv[1]));
    std::string fileStr(cfSz, '\0');
    cmdStream.read(fileStr.data(), cfSz);
    util::tokeniser tokeniser(fileStr);

    MOD modFile;
    while (!tokeniser.isEnd()) {
        const std::string& token = tokeniser.next();

        if (token == "load") {
            const std::string& filename = tokeniser.next();
            util::fstream_reader reader;
            reader.open(filename, std::ios_base::binary);
            if (!reader.is_open()) {
                std::cerr << "Unable to open " << filename << std::endl;
                continue;
            }
            std::cout << "Loading " << filename << std::endl;
            reader.endianness() = util::fstream_reader::Endianness::Big;
            reader.seekg(0, std::ios_base::beg);
            modFile.read(reader);
            reader.close();
            std::cout << std::endl;
        } else if (token == "write") {
            const std::string& filename = tokeniser.next();
            util::fstream_writer writer(util::fstream_writer::Endianness::Big);
            writer.open(filename, std::ios_base::binary);
            if (!writer.is_open()) {
                std::cerr << "Unable to open " << filename << std::endl;
                continue;
            }
            std::cout << "Writing " << filename << std::endl << std::endl;
            modFile.write(writer);
            writer.close();
        } else if (token == "tex_swap") {
            if (!modFile.m_textures.size()) {
                std::cout << "Loaded MOD file has no textures\n" << std::endl;
                continue;
            }

            std::cout << "There are " << modFile.m_textures.size() << " textures, which one do you want to swap?"
                      << std::endl;

            std::string input = "";
            std::getline(std::cin, input);

            try {
                const u32 toSwap = std::stoi(input);
                if (toSwap >= modFile.m_textures.size()) {
                    std::cout << "Error given index is too high!\n" << std::endl;
                    continue;
                }

                std::cout << "Input the path to the TXE file you want to replace Texture " << toSwap << " with: ";
                std::getline(std::cin, input);
                if (!std::filesystem::exists(input)) {
                    std::cout << "Error invalid path given!\n" << std::endl;
                    continue;
                }

                if (std::filesystem::path(input).extension() != ".txe") {
                    std::cout << "Error path is not a TXE file!\n" << std::endl;
                    continue;
                }

                util::fstream_reader txeReader(0, util::fstream_reader::Endianness::Big);
                txeReader.open(input, std::ios_base::binary);
                if (!txeReader.is_open()) {
                    std::cout << "Error couldn't open file!\n" << std::endl;
                    continue;
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
                texture.m_imageData.resize(calcTxeSize(texture.m_format, texture.m_width, texture.m_height));
                txeReader.read(reinterpret_cast<char*>(texture.m_imageData.data()), texture.m_imageData.size());
                txeReader.close();
                modFile.m_textures[toSwap] = texture;
            } catch (...) {
                std::cout << "Error while trying to swap textures!" << std::endl;
            }

            std::cout << std::endl;
        } else if (token == "tex_export") {
            if (!modFile.m_textures.size()) {
                std::cout << "Loaded MOD file has no textures" << std::endl;
                continue;
            }

            std::cout << "There are " << modFile.m_textures.size()
                      << " textures, which one do you want to export? (to export all just type \"all\")" << std::endl;

            if (std::filesystem::exists("texdump/")) {
                std::filesystem::remove_all("texdump/");
            }
            std::filesystem::create_directory("texdump/");

            std::string input = "";
            std::getline(std::cin, input);
            if (input == "all") {
                u32 i = 0;
                for (Texture& tex : modFile.m_textures) {
                    util::fstream_writer writer(util::fstream_writer::Endianness::Big);
                    std::string fileName = "texdump/out" + std::to_string(i++) + ".txe";
                    std::cout << "Writing " << fileName << std::endl;
                    writer.open(fileName);
                    if (!writer.is_open()) {
                        std::cout << "Error unable to open " << fileName << std::endl;
                        continue;
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
            } else {
                try {
                    const u32 idx = std::stoi(input);
                    if (idx >= modFile.m_textures.size()) {
                        std::cout << "Error given index is too high!\n" << std::endl;
                        continue;
                    }

                    util::fstream_writer writer(util::fstream_writer::Endianness::Big);
                    std::string fileName = "texdump/out" + std::to_string(idx) + ".txe";
                    std::cout << "Writing " << fileName << std::endl;
                    writer.open(fileName);
                    if (!writer.is_open()) {
                        std::cout << "Error unable to open " << fileName << std::endl;
                        continue;
                    }

                    Texture& tex = modFile.m_textures[idx];
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
                } catch (...) {
                    std::cout << "Error while trying to swap textures!" << std::endl;
                }
            }

            std::cout << std::endl;
        } else if (token == "ini_export") {
            std::cout << "Enter INI output path: ";
            std::string input = "";
            std::getline(std::cin, input);

            std::ofstream outStream(input);
            if (!outStream.is_open()) {
                std::cout << "Error can't open " << input << std::endl;
                continue;
            }

            outStream.write(reinterpret_cast<char*>(modFile.m_eofBytes.data()), modFile.m_eofBytes.size());
            outStream.close();

            std::cout << std::endl;
        } else if (token == "ini_import") {
            std::cout << "Enter INI input path: ";
            std::string input = "";
            std::getline(std::cin, input);

            std::ifstream inStream(input);
            if (!inStream.is_open()) {
                std::cout << "Error can't open " << input << std::endl;
                continue;
            }

            modFile.m_eofBytes.clear();
            std::string str((std::istreambuf_iterator<char>(inStream)), std::istreambuf_iterator<char>());
            for (const auto& c : str) {
                modFile.m_eofBytes.push_back(c);
            }

            std::cout << std::endl;
        } else if (token == "export_material") {
            std::cout << "Enter output path: ";
            std::string input = "";
            std::getline(std::cin, input);

            if (!modFile.m_materials.m_materials.size() && !modFile.m_materials.m_texEnvironments.size()) {
                std::cout << "Loaded file has no materials!" << std::endl;
                continue;
            }

            std::ostringstream oss = {};
            oss << "MATERIAL_FILE" << std::endl;

            if (modFile.m_materials.m_materials.size()) {
                oss << "MAT_SECTION" << std::endl;
            }

            u32 matIdx = 0;
            for (mat::Material& mat : modFile.m_materials.m_materials) {
                oss << "MAT " << matIdx++ << std::endl;
                oss << mat;
            }

            if (modFile.m_materials.m_texEnvironments.size()) {
                oss << "TEV_SECTION" << std::endl;
            }
            u32 tevIdx = 0;
            for (mat::TEVInfo& mat : modFile.m_materials.m_texEnvironments) {
                oss << "TEV " << tevIdx++ << std::endl;
                oss << mat;
            }

            std::ofstream output(input);
            if (!output.is_open()) {
                std::cout << "Error can't open " << input << std::endl;
                continue;
            }

            output.write(oss.str().c_str(), oss.str().size());
            output.close();
        } else if (token == "close") {
            modFile.reset();
            std::cout << std::endl;
        } else {
            std::cout << "Unknown token (" << token << ")\n" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}