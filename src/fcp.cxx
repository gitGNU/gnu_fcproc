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

#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <set>

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "libs/misc/environment.h"
#include "libs/conf/configuration.h"
#include "libs/file/utils.h"
#include "libs/misc/exception.h"
#include "filter.h"
#include "rules.h"
#include "transformation.h"
#include "chain.h"
#include "getopt.h"

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

#define USE_CONFIGURATION_FILE 0

char        separator           = ':';
#if USE_CONFIGURATION_FILE
std::string configuration_file  =
												     Environment::get("HOME") +
												     std::string("/") +
												     std::string(".") +
												     std::string(PACKAGE_TARNAME) +
												     std::string("/") +
												     std::string("configuration");
#endif
std::string rules_file          = Environment::get("HOME") +
												     std::string("/") +
												     std::string(".") +
												     std::string(PACKAGE_TARNAME) +
												     std::string("/") +
												     std::string("rules");
int         max_depth           = 16;
std::string temp_dir            = Environment::get("HOME") +
												     std::string("/") +
												     std::string(".") +
												     std::string(PACKAGE_TARNAME) +
												     std::string("/") +
												     std::string("cache");

void help(void)
{
	std::cout
		<< "Usage: " << PROGRAM_NAME << " [OPTION]... [TRANSFORMATION]..."<<                              std::endl
		<<                                                                                                std::endl
		<< "Options: " <<                                                                                 std::endl
#if USE_CONFIGURATION_FILE
		<< "  -c, --config=FILE       use alternate configuration file" <<                                std::endl
		<< "                          [" << configuration_file << "]" <<                                  std::endl
#endif
		<< "  -r, --rules=FILE        use alternate rules file" <<                                        std::endl
		<< "                          [default " << rules_file << "]" <<                                  std::endl
		<< "  -m, --max-depth=NUM     use NUM as max filter-chains depth" <<                              std::endl
		<< "                          [default " << max_depth << "]" <<                                   std::endl
		<< "  -t, --temp-dir=DIR      use DIR as temporary directory" <<                                  std::endl
		<< "                          [default " << temp_dir << "]" <<                                    std::endl
		<< "  -s, --separator=CHAR    use CHAR as INPUTFILE/OUTPUTFILE separator" <<                      std::endl
		<< "                          [default `" << separator << "']" <<                                 std::endl
		<< "  -n, --dry-run           display commands without modifying any files" <<                    std::endl
		<< "  -d, --debug             enable debugging traces" <<                                         std::endl
		<< "  -v, --verbose           verbosely report processing" <<                                     std::endl
		<< "  -h, --help              print this help, then exit" <<                                      std::endl
		<< "  -V, --version           print version number, then exit" <<                                 std::endl
		<<                                                                                                std::endl
		<< "Specify TRANSFORMATION using the format INPUTFILE<SEPARATOR>OUTPUTFILE." <<                   std::endl
		<< "Default SEPARATOR is '" << separator << "'. INPUTFILE and OUTPUTFILE must be different." <<   std::endl
		<<                                                                                                std::endl
		<< "Report bugs to <" << PACKAGE_BUGREPORT << ">" <<                                              std::endl;
}

void hint(const std::string & message)
{
	BUG_ON(message.size() == 0);

	std::cout
		<< message <<                                                 std::endl
		<< "Try `" << PROGRAM_NAME << " -h' for more information." << std::endl;
}

FCP::Chain * transform(const FCP::Transformation & transformation,
		       FCP::Rules &                rules,
		       int                         mdepth)
{
	BUG_ON(mdepth <= 0);

	TR_DBG("Transforming '%s' -> '%s'\n",
	       transformation.input().name().c_str(),
	       transformation.output().name().c_str());

	// Build the filters-chain
	std::vector<FCP::Rule *> chain;
	rules.chains(transformation.input().extension(),
		     transformation.output().extension(),
		     mdepth,
		     chain);
	if (chain.size() == 0) {
		throw Exception("No filters-chain available for "
				"'" + transformation.tag() + "' "
				"transformation");
	}

	TR_DBG("Filters chain:\n");
	std::vector<FCP::Rule *>::iterator iter;
	for (iter = chain.begin(); iter != chain.end(); iter++) {
		TR_DBG("  '%s' -> '%s'\n",
		       (*iter)->input().c_str(),
		       (*iter)->output().c_str());
	}

	// Perform transformation now
	FCP::Chain * j;

	j = new FCP::Chain(transformation.tag(),
			   transformation.input(),
			   chain,
			   transformation.output());
	BUG_ON(j == 0);

	return j;
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
				{ "temp-dir",  1, 0, 't' },
				{ "config",    1, 0, 'c' },
				{ "rules",     1, 0, 'r' },
				{ "max-depth", 1, 0, 'm' },
				{ "separator", 1, 0, 's' },
				{ "dry-run",   0, 0, 'n' },
				{ "debug",     0, 0, 'd' },
				{ "verbose",   0, 0, 'v' },
				{ "version",   0, 0, 'V' },
				{ "help",      0, 0, 'h' },
				{ 0,           0, 0, 0   }
			};

#if USE_CONFIGURATION_FILE
			c = getopt_long(argc, argv, "t:c:r:m:s:ndvVh",
					long_options, &option_index);
#else
			c = getopt_long(argc, argv, "t:r:m:s:ndvVh",
					long_options, &option_index);
#endif
			if (c == -1) {
				break;
			}

			switch (c) {
#if USE_CONFIGURATION_FILE
				case 'c':
					configuration_file = optarg;
					break;
#endif
				case 'r':
					rules_file = optarg;
					break;
				case 't':
					temp_dir = optarg;
					break;
				case 's':
					if (strlen(optarg) > 1) {
						hint("Separator too long");
						return 1;
					}
					separator = optarg[0];
					break;
				case 'm':
					max_depth = atoi(optarg);
					if (max_depth <= 1) {
						hint("Wrong max-depth");
						return 1;
					}
					break;
				case 'n':
					dry_run  = true;
					break;
				case 'd':
					TR_CONFIG_LVL(TR_LVL_DEBUG);
					break;
				case 'v':
					TR_CONFIG_LVL(TR_LVL_VERBOSE);
					break;
				case 'V':
					version();
					return 0;
				case 'h':
					help();
					return 0;
				case '?':
					hint("Unrecognized option");
					return 1;
				default:
					BUG();
					return 1;
			}
		}

		// Gather input file names
		if (optind >= argc) {
			hint("Missing transformation(s)");
			return 1;
		}

		TR_DBG("Separator     '%c'\n", separator);
#if USE_CONFIGURATION_FILE
		TR_DBG("Configuration '%s'\n", configuration_file.c_str());
		BUG_ON(configuration_file.size() == 0);
#endif
		TR_DBG("Rules         '%s'\n", rules_file.c_str());
		BUG_ON(rules_file.size() == 0);
		TR_DBG("Max depth     '%d'\n", max_depth);
		BUG_ON(max_depth <= 0);

		BUG_ON((argc - optind) < 0);

		std::vector<FCP::Transformation *>           transformations;
		std::vector<FCP::Transformation *>::iterator it;

		transformations.resize(argc - optind);

		TR_DBG("Transformations:\n");
		int i;
		for (i = optind; i < argc; i++) {
			int j;

			j = i - optind;
			try {
				FCP::Transformation * t;

				t = new FCP::Transformation(argv[i], separator);
				BUG_ON(t == 0);

				if (t->input() == t->output()) {
					TR_ERR("Transformation '%s' "
					       "input and output "
					       "must be different\n",
					       t->tag().c_str());
					return 1;
				}

				transformations[j] = t;

				TR_DBG("  '%s' = '%s' -> '%s'\n",
				       t->tag().c_str(),
				       t->input().name().c_str(),
				       t->output().name().c_str());

			} catch (std::exception & e) {
				TR_ERR("%s\n", e.what());
				return 1;
			}
		}

#if USE_CONFIGURATION_FILE
		// Read configuration file
		try {
			Configuration::File config;
			std::ifstream       stream(configuration_file.c_str());

			stream >> config;

		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
			return 1;
		}
#endif

		// Read rules file
		FCP::Rules * rules;

		try {
			rules = new FCP::Rules(rules_file);
		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
			return 1;
		}

		// Get all filters from transformations
		TR_DBG("Handling transformations\n");
		std::vector<FCP::Chain *> filters;
		try {
			for (it  = transformations.begin();
			     it != transformations.end();
			     it++) {
				FCP::Chain * j;

				j = transform(*(*it), *rules, max_depth);
				BUG_ON(j == 0);

				filters.push_back(j);
			}
		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
			return 1;
		}

		// Run all filters now
		TR_DBG("Running filters\n");
		std::vector<FCP::Chain *>::iterator ij;
		try {
			for (ij = filters.begin(); ij != filters.end(); ij++) {
				TR_DBG("Filters chain '%s':\n",
				       (*ij)->id().c_str());
				(*ij)->setup(temp_dir);

				(*ij)->run(dry_run);
			}
		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
			return 1;
		}

		// Clean up everything
		TR_DBG("Operations complete, cleaning up ...\n");

		for (ij  = filters.begin();
		     ij != filters.end();
		     ij++) {
			delete (*ij);
		}
		for (it  = transformations.begin();
		     it != transformations.end();
		     it++) {
			delete (*it);
		}

		delete rules;

	} catch (std::exception & e) {
		TR_ERR("Got exception '%s'\n", e.what());
		return 1;
	} catch (...) {
		BUG();
		return 1; // Should be useless ...
	};

	return 0;
}
