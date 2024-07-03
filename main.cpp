#include <commands.hpp>
#include <functional>
#include <iostream>
#include <util/misc.hpp>

// clang-format off
namespace {
void showCommands();

struct Command {
    std::string_view mCommand = "";
    std::vector<std::string_view> mParameters;
    std::string_view mDescription = "";
    std::function<void()> mFunction;
};

static std::vector<Command> gCommands = {
    { "load", { "input filename" }, "loads a MOD file", cmd::mod::loadFile },
    { "write", { "output filename" }, "writes the MOD file", cmd::mod::writeFile },
    { "close", {}, "closes the MOD file", cmd::mod::closeFile },

    { "NEW_LINE" },

    { "import_texture", {}, "swaps a texture with an external TXE file", cmd::mod::importTexture },
    { "import_ini", { "input filename" }, "imports an external ini", cmd::mod::importIni },

    { "NEW_LINE" },

    { "export_materials", { "output filename" }, " exports all materials to a file ", cmd::mod::exportMaterials },
    { "export_textures", { "output directory"}, "exports all textures to a directory", cmd::mod::exportTextures },
    { "export_ini", { "output filename" }, "exports the ini to a file", cmd::mod::exportIni },
    { "export_obj", { "output filename" }, "exports the model to an obj file [WIP]", cmd::mod::exportObj },
    { "export_dmd", { "output filename" }, "exports the model to a dmd file [WIP]", cmd::mod::exportDmd },

    { "NEW_LINE" },

    { "obj_to_dmd", { "input filename", "output filename" }, "converts an obj file to dmd [WIP]", cmd::objToDmd },
    
    { "NEW_LINE" },

    { "help", {}, "re-generate this command list", showCommands }
};

void showCommands()
{
    std::cout << std::endl << "Commands:" << std::endl;
    for (const Command& cmd : gCommands) {
        if (cmd.mCommand == "NEW_LINE") {
            std::cout << std::endl;
            continue;
        }

        std::cout << " " << cmd.mCommand << " ";

        for (std::size_t i = 0; i < cmd.mParameters.size(); i++) {
            std::cout << "[" << cmd.mParameters[i] << "] ";
        }

        std::cout << "- " << cmd.mDescription << std::endl;
    }
    std::cout << std::endl;
}
} // namespace
// clang-format on

int main(int argc, char** argv)
{
	std::atexit(util::ExitHook);

	std::cout << "MODConv by Axiot, 2021 [Last updated [DD/MM/YYYY] 03/07/2024]" << std::endl;
	std::cout << "Don't forget to write the file after editing it" << std::endl;

	showCommands();

	while (true) {
		std::string input = "";
		std::getline(std::cin, input);

		if (!input.size()) {
			break;
		}

		cmd::gTokeniser.read(input);
		const std::string& token = cmd::gTokeniser.next();

		bool foundCmd = false;
		for (const Command& cmd : gCommands) {
			if (cmd.mCommand == "NEW_LINE") {
				continue;
			}

			if (cmd.mCommand == token) {
				cmd.mFunction();
				foundCmd = true;
				break;
			}
		}

		if (!foundCmd) {
			std::cout << "Unknown command " << token << std::endl;
		}

		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}