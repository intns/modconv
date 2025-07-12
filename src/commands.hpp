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

static std::vector<Command> gCommands = {
	{ .mCommand = "load", .mParameters = { "input filename" }, .mDescription = "loads a MOD file", .mFunction = cmd::mod::loadFile },
	{ .mCommand = "write", .mParameters = { "output filename" }, .mDescription = "writes the MOD file", .mFunction = cmd::mod::writeFile },
	{ .mCommand = "close", .mParameters = {}, .mDescription = "closes the MOD file", .mFunction = cmd::mod::resetActiveModel },

	{ .mCommand = "NEW_LINE" },

	{ .mCommand     = "delete_chunk",
	  .mParameters  = { "target chunk (0x10, 0x12, 0x30, etc.)" },
	  .mDescription = "deletes a chunk type [dangerous]",
	  .mFunction    = cmd::mod::deleteChunk },
	{ .mCommand     = "edit_header",
	  .mParameters  = {},
	  .mDescription = "edits header information (date of creation / flags)",
	  .mFunction    = cmd::mod::editHeader },

	{ .mCommand = "NEW_LINE" },

	{ .mCommand     = "import_texture",
	  .mParameters  = { /*handles input internally*/ },
	  .mDescription = "swaps a texture with an external TXE file",
	  .mFunction    = cmd::mod::importTexture },
	{ .mCommand     = "import_ini",
	  .mParameters  = { "input filename" },
	  .mDescription = "imports an external ini",
	  .mFunction    = cmd::mod::importIni },
	{ .mCommand     = "import_material",
	  .mParameters  = { "input filename" },
	  .mDescription = "imports materials from an external file",
	  .mFunction    = cmd::mod::importMaterials },

	{ .mCommand = "NEW_LINE" },

	{ .mCommand     = "export_materials",
	  .mParameters  = { "output filename" },
	  .mDescription = " exports all materials to a file ",
	  .mFunction    = cmd::mod::exportMaterials },
	{ .mCommand     = "export_textures",
	  .mParameters  = { "output directory" },
	  .mDescription = "exports all textures to a directory",
	  .mFunction    = cmd::mod::exportTextures },
	{ .mCommand     = "export_ini",
	  .mParameters  = { "output filename" },
	  .mDescription = "exports the ini to a file",
	  .mFunction    = cmd::mod::exportIni },
	{ .mCommand     = "export_obj",
	  .mParameters  = { "output filename" },
	  .mDescription = "exports the model to an obj file [WIP]",
	  .mFunction    = cmd::mod::exportObj },
	{ .mCommand     = "export_dmd",
	  .mParameters  = { "output filename" },
	  .mDescription = "exports the model to a dmd file [WIP]",
	  .mFunction    = cmd::mod::exportDmd },
	{ .mCommand = "NEW_LINE" },

	{ .mCommand     = "delete_chunk",
	  .mParameters  = { "target chunk (0x10, 0x12, 0x30, etc.)" },
	  .mDescription = "deletes a chunk type [dangerous]",
	  .mFunction    = cmd::mod::deleteChunk },

	{ .mCommand = "NEW_LINE" },

	{ .mCommand = "help", .mParameters = {}, .mDescription = "re-generate this command list", .mFunction = showCommands }
};

inline void showCommands()
{
	std::cout << '\n' << "Commands:" << '\n';
	for (const Command& cmd : gCommands) {
		if (cmd.mCommand == "NEW_LINE") {
			std::cout << '\n';
			continue;
		}

		std::cout << " " << cmd.mCommand << " ";

		for (auto mParameter : cmd.mParameters) {
			std::cout << "[" << mParameter << "] ";
		}

		std::cout << "- " << cmd.mDescription << '\n';
	}
	std::cout << '\n';
}

} // namespace cmd

#endif
