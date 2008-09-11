//
// Copyright (C) 2007, 2008 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "config.h"

#include <getopt.h>
#include <cassert>
#include <string>
#include <iostream>
#include <vector>

#include "debug.h"
#include "globals.h"

void version(void)
{
	std::cout
		<< PACKAGE << " (" << PACKAGE_NAME  << ") " << PACKAGE_VERSION <<                    std::endl
		<<                                                                                   std::endl
		<< "Copyright (C) 2007, 2008 Francesco Salvestrini" <<                               std::endl
		<<                                                                                   std::endl
		<< "This is free software.  You may redistribute copies of it under the terms of" << std::endl
		<< "the GNU General Public License <http://www.gnu.org/licenses/gpl.html>." <<       std::endl
		<< "There is NO WARRANTY, to the extent permitted by law." <<                        std::endl;
}

void help(void)
{
	std::cout
		<< "Usage: " << PACKAGE << " [OPTION]... "<<               std::endl
		<<                                                         std::endl
		<< "Options: " <<                                          std::endl
		<< "  -d, --debug      enable debugging traces" <<         std::endl
		<< "  -v, --verbose    verbosely report processing" <<     std::endl
		<< "  -h, --help       print this help, then exit" <<      std::endl
		<< "  -V, --version    print version number, then exit" << std::endl
		<<                                                         std::endl
		<< "Report bugs to <" << PACKAGE_BUGREPORT << ">" <<       std::endl;
}

void hint(const std::string & message)
{
	BUG_ON(message.size() == 0);

	std::cout
		<< message <<                                            std::endl
		<< "Try `" << PACKAGE << " -h' for more information." << std::endl;
}

bool transform(const std::string & input)
{
	BUG_ON(input.size() == 0);

	if (debug) {
		std::cout << "Transforming " << input << std::endl;
	}

	return true;
}

int main(int argc, char * argv[])
{
	int c;
	int digit_optind = 0;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index       = 0;

		static struct option long_options[] = {
			{ "debug",   0, 0, 'd' },
			{ "verbose", 0, 0, 'v' },
			{ "version", 0, 0, 'V' },
			{ "help",    0, 0, 'h' },
			{ 0,         0, 0, 0   }
		};

		c = getopt_long(argc, argv, "dvh",
				long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
			case 'd':
				debug = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'V':
				version();
				return 0;
			case 'h':
				help();
				return 0;
			case '?':
				hint("Unrecognized option");
				break;

			default:
				BUG();
				break;
		}
	}

	if (optind >= argc) {
		hint("Missing input file name(s)");
		return 1;
	}

	std::vector<std::string> inputs;
	int                      count;

	count = argc - optind;
	assert(count >= 0);

	inputs.resize(count);

	int i;
	for (i = optind; i < argc; i++) {
		inputs[i - optind] = argv[i];
	}

	std::vector<std::string>::iterator iter;
	for (iter = inputs.begin(); iter != inputs.end(); iter++) {
		if (!transform(*iter)) {
			return 1;
		}
	}

	return 0;
}
