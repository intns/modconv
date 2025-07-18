#ifndef _UNIT_TESTS_HPP
#define _UNIT_TESTS_HPP

#pragma once

#include <filesystem>
#include <iostream>
#include "commands.hpp"
#include "util/misc.hpp"

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
	for (const auto& dentry : rdi(workingPath)) {
		if (!dentry.is_regular_file())
			continue;

		const auto& ext = dentry.path().extension();
		if (ext == ".mod") {
			pathList.push_back(dentry.path());
		}
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
	cmd::mod::importMod();

	cmd::gTokeniser.read("out.mod");
	cmd::mod::exportMod();

	cmd::mod::resetModel();

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
	cmd::mod::importMod();

	cmd::mod::exportMaterials();

	// Delete the material chunk and import our export (testing our export)
	cmd::gTokeniser.read("0x30");
	cmd::mod::deleteChunk();

	cmd::mod::importMaterials();

	// Load out.mod
	cmd::gTokeniser.read("out.mod");
	cmd::mod::exportMod();

	// Cleanup and check parity
	cmd::mod::resetModel();

	return util::AreFilesIdentical(path, "out.mod");
}

inline bool Unit_TestCollisionReadWrite(const fs::path& path)
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
	cmd::mod::importMod();

	MOD& mod = cmd::gModFile;

	if (mod.mCollisionGridInfo.mGroups.empty() && mod.mCollisionTriangles.mCollInfo.empty()) {
		return true;
	}

	cmd::mod::exportCollision();

	// Delete the material chunk and import our export (testing our export)
	cmd::gTokeniser.read("0x100");
	cmd::mod::deleteChunk();

	cmd::gTokeniser.read("0x110");
	cmd::mod::deleteChunk();

	cmd::mod::importCollision();

	// Load out.mod
	cmd::gTokeniser.read("out.mod");
	cmd::mod::exportMod();

	// Cleanup and check parity
	cmd::mod::resetModel();

	return util::AreFilesIdentical(path, "out.mod");
}

//
inline void UnitTest(const std::string& path = "unit")
{
	// Run some unit tests before we can allow the user to destroy their files
	auto pathList = BuildUnitPathList(path);
	for (const auto& p : pathList) {
		if (!Unit_TestReadWrite(p) || !Unit_TestMaterialReadWrite(p) || !Unit_TestCollisionReadWrite(p)) {
			std::cout << p << std::endl;
			throw std::runtime_error("Fuck");
		}
	}
}
} // namespace test

#endif