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

#include "misc/debug.h"
#include "misc/environment.h"
#include "graph/dag.h"

#define PROGRAM_NAME "fcp"

void version(void)
{
	std::cout
		<< PROGRAM_NAME << " (" << PACKAGE_NAME  << ") " << PACKAGE_VERSION <<               std::endl
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
		<< "Usage: " << PROGRAM_NAME << " [OPTION]... [FILE]..."<<              std::endl
		<<                                                                      std::endl
		<< "Options: " <<                                                       std::endl
		<< "  -n, --dry-run    display commands without modifying any files" << std::endl
		<< "  -d, --debug      enable debugging traces" <<                      std::endl
		<< "  -v, --verbose    verbosely report processing" <<                  std::endl
		<< "  -h, --help       print this help, then exit" <<                   std::endl
		<< "  -V, --version    print version number, then exit" <<              std::endl
		<<                                                                      std::endl
		<< "Report bugs to <" << PACKAGE_BUGREPORT << ">" <<                    std::endl;
}

void hint(const std::string & message)
{
	BUG_ON(message.size() == 0);

	std::cout
		<< message <<                                                 std::endl
		<< "Try `" << PROGRAM_NAME << " -h' for more information." << std::endl;
}

Graph::DAG * readconfig(std::string configuration_file)
{
	TR_DBG("Reading configuration file '%s'\n",
	       configuration_file.c_str());

	Graph::DAG * dag;
	dag = new Graph::DAG();

	return dag;
}

bool transform(const std::string &              input,
	       const std::vector<Graph::Node> & filters,
	       const std::string &              output)
{
	BUG_ON(input.size()   == 0);
	BUG_ON(filters.size() == 0);
	BUG_ON(output.size()  == 0);

	TR_DBG("Transforming %s -> %s\n",
	       input.c_str(),
	       output.c_str());

	return true;
}

int main(int argc, char * argv[])
{
	TR_CONFIG_LVL(TR_LVL_DEFAULT);
	TR_CONFIG_PFX(PROGRAM_NAME);

	try {
		bool dry_run = false;

		int c;
		// int digit_optind = 0;
		while (1) {
			// int this_option_optind = optind ? optind : 1;
			int option_index       = 0;

			static struct option long_options[] = {
				{ "dry-run", 0, 0, 'n' },
				{ "debug",   0, 0, 'd' },
				{ "verbose", 0, 0, 'v' },
				{ "version", 0, 0, 'V' },
				{ "help",    0, 0, 'h' },
				{ 0,         0, 0, 0   }
			};

			c = getopt_long(argc, argv, "ndvVh",
					long_options, &option_index);
			if (c == -1) {
				break;
			}

			switch (c) {
				case 'n':
					dry_run  = true;
					break;
				case 'd':
					TR_CONFIG_LVL(TR_LVL_DEBUG);
					break;
				case 'v':
					TR_CONFIG_LVL(TR_LVL_NOTICE);
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

		// Gather input file names
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

		// Build configuration file path
		std::string homedir = Environment::get("HOME");
		std::string conffile =
			homedir +
			std::string("/") +
			std::string(".") +
			std::string(PACKAGE_TARNAME);

		// Dump (acquired and derived) infos
		TR_DBG("Home directory:     '%s'\n", homedir.c_str());
		TR_DBG("Configuration file: '%s'\n", conffile.c_str());

		// Read configuration file and build dependency DAG

		// Perform all transformations
		std::vector<std::string>::iterator iter;
		for (iter = inputs.begin(); iter != inputs.end(); iter++) {
			std::vector<Graph::Node> filters;

			// Transform input file using filters
			if (!transform(*iter, filters, "temp")) {
				return 1;
			}
		}

		BUG();
	} catch (...) {
		BUG();
	};

	return 0;
}
