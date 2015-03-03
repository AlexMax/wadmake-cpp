/*
 *  wadmake: a WAD manipulation utility.
 *  Copyright (C) 2015  Alex Mayfield
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config_wadsh.h"

#include <cstring>
#include <iostream>
#include <sstream>

#ifdef READLINE_FOUND
#include <readline/readline.h>
#endif

#include "lua.hh"

class endOfFile : public std::exception { };

#ifdef _WIN32
static const char* exitMessage = "Press Ctrl-Z then Enter on an empty line to quit the shell.";
#else
static const char* exitMessage = "Press Ctrl-D on an empty line to quit the shell.";
#endif

#ifdef READLINE_FOUND

static std::string getLine(const char* prompt) {
	std::string line;

	// readline() returns malloc'ed pointer, must be freed by hand
	char* buffer = readline(prompt);

	// Buffer pointer is set to NULL on EOF
	if (buffer == NULL) {
		std::free(buffer);
		throw endOfFile();
	}

	line = buffer;
	std::free(buffer);
	return line;
}

#else

static std::string getLine(const char* prompt) {
	std::string line;
	std::cout << prompt;
	std::getline(std::cin, line);
	if (std::cin.eof()) {
		throw endOfFile();
	}
	return line;
}

#endif

int main(int argc, char** argv) {
	WADmake::LuaEnvironment lua;

	if (argc > 1) {
		if (std::strcmp(argv[1], "-") == 0) {
			// First parameter is -, read script from stdin
			try {
				std::stringstream input;
				input << std::cin.rdbuf();
				lua.doString(input.str(), "stdin");
			} catch (const std::runtime_error& e) {
				std::cerr << e.what() << std::endl;
				return EXIT_FAILURE;
			}
		} else {
			// Assume all parameters are files containing scripts to run
			try {
				lua.doFile(argv[1]);
			} catch (const std::runtime_error& e) {
				std::cerr << e.what() << std::endl;
				return EXIT_FAILURE;
			}
		}

		return EXIT_SUCCESS;
	}

	std::cerr << "WADmake shell" << std::endl;
	std::cerr << exitMessage << std::endl;

	for (;;) {
		std::string line;
		try {
			line = getLine("> ");
		} catch (const endOfFile& e) {
			std::cerr << "EOF" << std::endl;
			break;
		}

		try {
			lua.doString(line, "stdin");
			if (lua.gettop() != 0) {
				// Implicitly write any return values to stderr
				std::cerr << "<- ";
				lua.writeStack(std::cerr) << std::endl;
			}
		} catch (const std::runtime_error& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return EXIT_SUCCESS;
}
