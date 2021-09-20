#include <MOD.hpp>
#include <filesystem>
#include <iostream>
#include <util/tokeniser.hpp>

static const u32 calcTxeSize(u32 fmt, u32 x, u32 y)
{
    switch (fmt) {
    case 0:
    case 2:
    case 6:
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

int main(int argc, char** argv)
{
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
                return EXIT_FAILURE;
            }
            std::cout << "Loading " << filename << std::endl;
            reader.endianness() = util::fstream_reader::Endianness::Big;
            reader.seekg(0, std::ios_base::beg);
            modFile.read(reader);
            reader.close();
        } else if (token == "write") {
            const std::string& filename = tokeniser.next();
            util::fstream_writer writer(util::fstream_writer::Endianness::Big);
            writer.open(filename, std::ios_base::binary);
            if (!writer.is_open()) {
                std::cerr << "Unable to open " << filename << std::endl;
                return EXIT_FAILURE;
            }
            std::cout << std::endl << "Writing " << filename << std::endl;
            modFile.write(writer);
            writer.close();
        } else if (token == "texswap") {
            if (!modFile.m_textures.size()) {
                std::cout << "Loaded MOD file has no textures" << std::endl;
                continue;
            }

            std::cout << "There are " << modFile.m_textures.size() << " textures, which one do you want to swap?"
                      << std::endl;

            std::string input = "";
            std::getline(std::cin, input);

            try {
                const u32 toSwap = std::stoi(input);
                if (toSwap >= modFile.m_textures.size()) {
                    std::cout << "Error given index is too high!" << std::endl;
                    continue;
                }

                std::cout << "Input the path to the TXE file you want to replace Texture " << toSwap << " with: ";
                std::getline(std::cin, input);
                if (!std::filesystem::exists(input)) {
                    std::cout << "Error invalid path given!" << std::endl;
                    continue;
                }

                if (std::filesystem::path(input).extension() != ".txe") {
                    std::cout << "Error path is not a TXE file!" << std::endl;
                    continue;
                }

                util::fstream_reader txeReader(0, util::fstream_reader::Endianness::Big);
                txeReader.open(input, std::ios_base::binary);
                if (!txeReader.is_open()) {
                    std::cout << "Error couldn't open file!" << std::endl;
                    continue;
                }

                Texture texture;
                texture.m_width  = txeReader.readU16();
                texture.m_height = txeReader.readU16();
                texture.m_format = txeReader.readU16();
                texture.m_unknown = texture.m_format >> 8;
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
                continue;
            }
        } else if (token == "close") {
            std::cout << std::endl;
            modFile.reset();
        }
    }

    ((void)std::getchar());

    return EXIT_SUCCESS;
}