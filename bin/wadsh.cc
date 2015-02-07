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

#include <iostream>
#include <sstream>

#include "lua.hh"

int main(int argc, char** argv) {
	WADmake::LuaEnvironment lua;

	if (argc > 1) {
		if (std::strcmp(argv[1], "-") == 0) {
			try {
				std::stringstream input;
				input << std::cin.rdbuf();
				lua.doString(input.str(), "stdin");
			} catch (const std::runtime_error& e) {
				std::cerr << e.what() << std::endl;
				return EXIT_FAILURE;
			}
		} else {
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
#ifdef _WIN32
	std::cerr << "Press Ctrl-Z then Enter to quit the shell." << std::endl;
#else
	std::cerr << "Press Ctrl-D to quit the shell." << std::endl;
#endif

	for (;;) {
		std::string line; 
		std::cout << "> ";
		std::getline(std::cin, line);

		if (std::cin.eof()) {
			std::cout << "EOF" << std::endl;
			break;
		}

		try {
			lua.doString(line, "stdin");
			if (lua.gettop() != 0) {
				std::cout << "<- ";
				lua.writeStack(std::cout) << std::endl;
			}
		} catch (const std::runtime_error& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return EXIT_SUCCESS;
}
