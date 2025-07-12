#ifndef _UNIT_TESTS_HPP
#define _UNIT_TESTS_HPP

#pragma once

#include <filesystem>
#include <iostream>
#include <commands.hpp>
#include <util/misc.hpp>

namespace test {
namespace fs = std::filesystem;

inline std::vector<fs::path> BuildUnitPathList(const std::string& path = "unit")
{
	// Assume it's in unit/ folder
	fs::path workingPath = fs::current_path() / path;

	if (!fs::exists(workingPath)) {
		std::cout << workingPath.string() << " doesn't exist!" << std::endl;
		return {};
	}

	std::vector<fs::path> pathList;

	using rdi = fs::recursive_directory_iterator;
	for (const fs::directory_entry& dentry : rdi(workingPath)) {
		pathList.push_back(dentry.path());
	}

	return pathList;
}

inline bool Unit_TestReadWrite(const fs::path& path)
{
	std::error_code ec;
	const auto relativePath = fs::relative(path, fs::current_path(), ec);
	if (ec) {
		std::cout << "Error in converting to relative path: " << ec.message() << std::endl;
		return false;
	}

	cmd::gTokeniser.read(relativePath.string());
	cmd::mod::loadFile();

	cmd::gTokeniser.read("out.mod");
	cmd::mod::writeFile();

	cmd::mod::resetActiveModel();

	// Ensure the file is the same, byte for byte
	return util::AreFilesIdentical(path, "out.mod");
}

inline bool Unit_TestMaterialReadWrite(const fs::path& path)
{
	// Build a list of
	std::error_code ec;
	const auto relativePath = fs::relative(path, fs::current_path(), ec);
	if (ec) {
		std::cout << "Error in converting to relative path: " << ec.message() << std::endl;
		return false;
	}

	// Load relativePath
	cmd::gTokeniser.read(relativePath.string());
	cmd::mod::loadFile();

	cmd::mod::exportMaterials();

	// Delete the material chunk and import our export (testing our export)
	cmd::gTokeniser.read("0x30");
	cmd::mod::deleteChunk();

	cmd::mod::importMaterials();

	// Load out.mod
	cmd::gTokeniser.read("out.mod");
	cmd::mod::writeFile();

	// Cleanup and check parity
	cmd::mod::resetActiveModel();

	return util::AreFilesIdentical(path, "out.mod");
}

//
inline void UnitTest(const std::string& path = "unit")
{
	// Run some unit tests before we can allow the user to destroy their files
	auto pathList = BuildUnitPathList(path);
	for (const auto& p : pathList) {
		if (Unit_TestReadWrite(p) && Unit_TestMaterialReadWrite(p)) {
			std::cout << "[PASS]\t" << p.filename() << std::endl;
		} else {
			std::cout << "[FAIL]\t" << p.filename() << std::endl;
		}
	}
}
} // namespace test

#endif