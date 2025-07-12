#include <commands.hpp>
#include <functional>
#include <iostream>
#include <util/misc.hpp>
#include <string_view>
#include <sstream>
#include <iomanip>

#include <unit_tests.hpp>

// Note: Unit tests run in silent mode by default to reduce output noise.
// Use --test-verbose to see detailed test output.

void printUsage(const char* programName)
{
	namespace fs = std::filesystem;
	std::cout << "\nUsage: " << fs::path(programName).filename() << " [options] [commands...]\n\n";
	std::cout << "Options:\n";
	std::cout << "  --help, -h        Show this help message\n";
	std::cout << "  --test [dir]      Run unit tests (silent by default, optional test directory)\n";
	std::cout << "  --test-verbose    Enable verbose output for unit tests\n";
	std::cout << "  --quiet, -q       Disable verbose output\n";
	std::cout << "  --verbose, -v     Enable verbose output (default)\n\n";

	std::cout << "Commands can be chained together. For example:\n";
	std::cout << "  " << fs::path(programName).filename() << " load input.mod export_obj output.obj write output.mod\n\n";

	std::cout << "Available commands:\n";

	// Group commands by category
	std::cout << "\nFile Operations:\n";
	std::cout << "  load <filename>              Load a MOD file\n";
	std::cout << "  write <filename>             Write the MOD file\n";
	std::cout << "  close                        Close the MOD file\n";

	std::cout << "\nImport Operations:\n";
	std::cout << "  import_texture               Swap a texture with an external TXE file (interactive)\n";
	std::cout << "  import_ini <filename>        Import an external INI file\n";
	std::cout << "  import_material <filename>   Import materials from an external file\n";

	std::cout << "\nExport Operations:\n";
	std::cout << "  export_materials <filename>  Export all materials to a file\n";
	std::cout << "  export_textures <directory>  Export all textures to a directory\n";
	std::cout << "  export_ini <filename>        Export the INI to a file\n";
	std::cout << "  export_obj <filename>        Export the model to an OBJ file\n";
	std::cout << "  export_dmd <filename>        Export the model to a DMD file\n";

	std::cout << "\nModification Operations:\n";
	std::cout << "  delete_chunk <chunk_id>      Delete a chunk type (e.g., 0x10, 0x30)\n";
	std::cout << "  edit_header                  Edit header information (interactive)\n";

	std::cout << "\nOther:\n";
	std::cout << "  help                         Show available commands (in interactive mode)\n\n";

	std::cout << "When no commands are provided, the program starts in interactive mode.\n";
}

bool processCommand(const std::string& commandStr)
{
	// Parse the command string
	cmd::gTokeniser.read(commandStr);

	if (cmd::gTokeniser.isEnd()) {
		return true; // Empty command, skip
	}

	const std::string& token = cmd::gTokeniser.next();

	// Find and execute the command
	for (const cmd::Command& cmd : cmd::gCommands) {
		if (cmd.mCommand == "NEW_LINE") {
			continue;
		}

		if (cmd.mCommand == token) {
			try {
				cmd.mFunction();
				return true;
			} catch (const std::exception& e) {
				std::cerr << "Error executing command '" << token << "': " << e.what() << std::endl;
				return false;
			}
		}
	}

	std::cerr << "Unknown command: " << token << std::endl;
	return false;
}

int main(int argc, char** argv)
{
	std::cout << "\t----------------------------" << std::endl;
	std::cout << "\t-- modconv by intns, 2025 --" << std::endl;
	std::cout << "\t----------------------------" << std::endl << std::endl;

	// Default to verbose mode
	cmd::gModFile.mVerbosePrint = true;

	if (argc > 1) {
		// Parse command-line arguments
		std::vector<std::string> args;
		bool quietMode   = false;
		bool testVerbose = false;

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];

			// Handle options
			if (arg == "--help" || arg == "-h") {
				printUsage(argv[0]);
				return EXIT_SUCCESS;
			} else if (arg == "--test") {
				// Save current verbose state and set to silent for tests
				bool savedVerbose           = cmd::gModFile.mVerbosePrint;
				cmd::gModFile.mVerbosePrint = testVerbose;

				// Check if there's an optional test directory argument
				if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("-")) {
					test::UnitTest(argv[++i]);
				} else {
					test::UnitTest();
				}

				// Restore verbose state
				cmd::gModFile.mVerbosePrint = savedVerbose;
				return EXIT_SUCCESS;
			} else if (arg == "--test-verbose") {
				testVerbose = true;
			} else if (arg == "--quiet" || arg == "-q") {
				quietMode                   = true;
				cmd::gModFile.mVerbosePrint = false;
			} else if (arg == "--verbose" || arg == "-v") {
				quietMode                   = false;
				cmd::gModFile.mVerbosePrint = true;
			} else if (arg.starts_with("--") || arg.starts_with("-")) {
				std::cerr << "Unknown option: " << arg << std::endl;
				std::cerr << "Use --help for usage information." << std::endl;
				return EXIT_FAILURE;
			} else {
				// It's a command or parameter
				args.push_back(arg);
			}
		}

		// If we have commands to process
		if (!args.empty()) {
			// Build command strings by grouping command with its parameters
			std::vector<std::string> commandStrings;
			std::string currentCommand;

			for (const auto& arg : args) {
				// Check if this is a known command
				bool isCommand = false;
				for (const cmd::Command& cmd : cmd::gCommands) {
					if (cmd.mCommand == arg && cmd.mCommand != "NEW_LINE") {
						isCommand = true;
						break;
					}
				}

				if (isCommand) {
					// If we have a previous command, save it
					if (!currentCommand.empty()) {
						commandStrings.push_back(currentCommand);
					}
					currentCommand = arg;
				} else {
					// It's a parameter, append to current command
					if (!currentCommand.empty()) {
						currentCommand += " " + arg;
					} else {
						std::cerr << "Error: Parameter '" << arg << "' without a command." << std::endl;
						return EXIT_FAILURE;
					}
				}
			}

			// Don't forget the last command
			if (!currentCommand.empty()) {
				commandStrings.push_back(currentCommand);
			}

			// Process all commands
			bool success = true;
			for (size_t i = 0; i < commandStrings.size(); ++i) {
				if (!quietMode && commandStrings.size() > 1) {
					std::cout << "\n[" << (i + 1) << "/" << commandStrings.size() << "] Executing: " << commandStrings[i] << std::endl;
				}

				if (!processCommand(commandStrings[i])) {
					std::cerr << "Command failed. Stopping execution." << std::endl;
					success = false;
					break;
				}
			}

			if (!quietMode) {
				std::cout << "\nBatch processing " << (success ? "completed successfully." : "failed.") << std::endl;
			}

			return success ? EXIT_SUCCESS : EXIT_FAILURE;
		}

		// If no commands provided after parsing options, fall through to interactive mode
	}

	// Interactive mode
	std::cout << "Type 'help' for available commands." << std::endl;
	std::cout << "Remember to write the file after editing it!" << std::endl;

	// cmd::showCommands();

	while (true) {
		std::cout << "> ";
		std::cout.flush();

		std::string input;
		std::getline(std::cin, input);

		if (!input.size() || std::cin.eof()) {
			break;
		}

		processCommand(input);
		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}
