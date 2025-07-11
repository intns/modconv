#include <commands.hpp>
#include <functional>
#include <iostream>
#include <util/misc.hpp>

#if 0
// Leftover from testing, not to be abandoned yet
static void test()
{
	cmd::gTokeniser.read("iwagon.mod");
	cmd::mod::loadFile();

	std::cout << std::endl;

	// Export materials
	cmd::mod::exportMaterials();
	std::cout << std::endl;

	// Delete materials
	cmd::gTokeniser.read("0x30");
	cmd::mod::deleteChunk();
	std::cout << std::endl;

	// Re-import materials
	cmd::mod::importMaterials();
	std::cout << std::endl;

	// Write to new file
	cmd::gTokeniser.read("out.mod");
	cmd::mod::writeFile();
	cmd::mod::closeFile();
	std::cout << std::endl;

	// ... and then diff using HxD
	std::exit(EXIT_SUCCESS);
}
#endif

int main(int argc, char** argv)
{
	std::cout << "MODConv by intns, 2021 [Last updated [DD/MM/YYYY] 12/07/2025]" << std::endl;
	std::cout << "Don't forget to write the file after editing it" << std::endl;

	cmd::showCommands();

	while (true) {
		std::string input = "";
		std::getline(std::cin, input);

		if (!input.size()) {
			break;
		}

		cmd::gTokeniser.read(input);
		const std::string& token = cmd::gTokeniser.next();

		bool foundCmd = false;
		for (const cmd::Command& cmd : cmd::gCommands) {
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