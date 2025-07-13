#ifndef __COMMANDS_HPP
#define __COMMANDS_HPP

#pragma once

#include "util/tokeniser.hpp"
#include <functional>
#include <iostream>
#include "MOD.hpp"
#include <string>

namespace cmd {
extern MOD gModFile;
extern std::string gModFileName;
extern util::tokeniser gTokeniser;

namespace mod {
void loadFile();
void writeFile();
void resetActiveModel();

void importObj();
void importMaterials();
void importTexture();
void importIni();

void exportMaterials();
void exportTextures();
void exportObj();
void exportDmd();
void exportIni();

void deleteChunk();
void editHeader();
} // namespace mod

void showCommands();

struct Command {
	std::string_view mCommand;
	std::vector<std::string_view> mParameters;
	std::string_view mDescription;
	std::function<void()> mFunction;
};

static std::vector<Command> gCommands
    = { { "load", { "input filename" }, "loads a MOD file", cmd::mod::loadFile },
	    { "write", { "output filename" }, "writes the MOD file", cmd::mod::writeFile },
	    { "close", {}, "closes the MOD file", cmd::mod::resetActiveModel },

	    { "NEW_LINE" },

	    { "import_material", { "input filename" }, "imports materials from an external file", cmd::mod::importMaterials },
	    { "import_texture", { /*handles input internally*/ }, "swaps a texture with an external TXE file", cmd::mod::importTexture },
	    { "import_obj", { "input filename" }, "imports an external obj", cmd::mod::importObj },
	    { "import_ini", { "input filename" }, "imports an external ini", cmd::mod::importIni },

	    { "NEW_LINE" },

	    { "export_materials", { "output filename" }, " exports all materials to a file ", cmd::mod::exportMaterials },
	    { "export_textures", { "output directory" }, "exports all textures to a directory", cmd::mod::exportTextures },
	    { "export_obj", { "output filename" }, "exports the model to an obj file [WIP]", cmd::mod::exportObj },
	    { "export_ini", { "output filename" }, "exports the ini to a file", cmd::mod::exportIni },
	    { "export_dmd", { "output filename" }, "exports the model to a dmd file [WIP]", cmd::mod::exportDmd },

	    { "NEW_LINE" },

	    { "delete_chunk", { "target chunk (0x10, 0x12, 0x30, etc.)" }, "deletes a chunk type [dangerous]", cmd::mod::deleteChunk },
	    { "edit_header", {}, "edits header information (date of creation / flags)", cmd::mod::editHeader },

	    { "NEW_LINE" },

	    { "help", {}, "re-generate this command list", showCommands } };

inline void showCommands()
{
	std::cout << '\n' << "Commands:" << '\n';
	for (const Command& cmd : gCommands) {
		if (cmd.mCommand == "NEW_LINE") {
			std::cout << '\n';
			continue;
		}

		std::cout << " " << cmd.mCommand << " ";

		for (auto& mParameter : cmd.mParameters) {
			std::cout << "[" << mParameter << "] ";
		}

		std::cout << "- " << cmd.mDescription << '\n';
	}
	std::cout << '\n';
}

} // namespace cmd

#endif
