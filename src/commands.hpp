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
void importMod();
void exportMod();
void resetModel();

void listChunks();
void importObj();
void importMaterials();
void importTexture();
void importIni();

void exportObj();
void exportMaterials();
void exportTextures();
void exportIni();
void exportDmd();

void exportCollision();
void importCollision();

void deleteChunk();
void editHeader();
} // namespace mod

void showCommands();

struct Command {
	std::string_view mCommand;
	std::vector<std::string_view> mParameters;
	std::string_view mDescription;
	std::function<void()> mFunction;

	Command(std::string_view cmd, std::vector<std::string_view> params, std::string_view desc, std::function<void()> func)
	    : mCommand(cmd)
	    , mParameters(params)
	    , mDescription(desc)
	    , mFunction(func)
	{
	}

	explicit Command(std::string_view cmd)
	    : mCommand(cmd)
	    , mParameters {}
	    , mDescription {}
	    , mFunction {}
	{
	}
};

static std::vector<Command> gCommands = {
	Command("load", { "input filename" }, "loads a MOD file", cmd::mod::importMod),
	Command("write", { "output filename" }, "writes the MOD file", cmd::mod::exportMod),
	Command("reset", {}, "resets the currently loaded MOD file", cmd::mod::resetModel),

	Command("NEW_LINE"),

	Command("list_chunks", {}, "lists all chunks in the currently loaded MOD file", cmd::mod::listChunks),
	Command("delete_chunk", { "target chunk (0x10, 0x12, 0x30, etc.)" }, "deletes a chunk type [dangerous]", cmd::mod::deleteChunk),
	Command("edit_header", {}, "edits header information (date of creation / flags)", cmd::mod::editHeader),

	Command("NEW_LINE"),

	Command("import_col", { "input filename" }, "imports collision data from a file", cmd::mod::importCollision),
	Command("import_mat", { "input filename" }, "imports materials from an external file", cmd::mod::importMaterials),
	Command("import_obj", { "input filename" }, "imports an external obj", cmd::mod::importObj),
	Command("import_ini", { "input filename" }, "imports an external ini", cmd::mod::importIni),
	Command("import_tex", {}, "swaps a texture with an external TXE file", cmd::mod::importTexture),

	Command("NEW_LINE"),

	Command("export_col", { "output filename" }, "exports collision data to a file", cmd::mod::exportCollision),
	Command("export_mat", { "output filename " }, "exports all materials to a file ", cmd::mod::exportMaterials),
	Command("export_obj", { "output filename " }, "exports the model to an OBJ file [WIP]", cmd::mod::exportObj),
	Command("export_ini", { "output filename " }, "exports the ini to a file", cmd::mod::exportIni),
	Command("export_tex", { "output directory" }, "exports all textures to a directory", cmd::mod::exportTextures),
	Command("export_dmd", { "output filename " }, "exports the model to a DMD file [WIP]", cmd::mod::exportDmd),

	Command("NEW_LINE"),

	Command("help", {}, "re-generate this command list", showCommands),
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

		for (auto& mParameter : cmd.mParameters) {
			std::cout << "[" << mParameter << "] ";
		}

		std::cout << "- " << cmd.mDescription << '\n';
	}
	std::cout << '\n';
}

} // namespace cmd

#endif
