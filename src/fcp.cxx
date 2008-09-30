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

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "libs/misc/environment.h"
#include "libs/conf/configuration.h"
#include "libs/file/utils.h"
#include "transformation.h"
#include "exception.h"
#include "filter.h"

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

void help(void)
{
	std::cout
		<< "Usage: " << PROGRAM_NAME << " [OPTION]... [TRANSFORMATION]..."<<            std::endl
		<<                                                                              std::endl
		<< "Options: " <<                                                               std::endl
#if USE_CONFIGURATION_FILE
		<< "  -c, --config=FILE       use alternate configuration file" <<              std::endl
		<< "                          [" << configuration_file << "]" <<                std::endl
#endif
		<< "  -r, --rules=FILE        use alternate rules file" <<                      std::endl
		<< "                          [" << rules_file << "]" <<                        std::endl
		<< "  -s, --separator=CHAR    use CHAR as INPUTFILE/OUTPUTFILE separator" <<    std::endl
		<< "  -n, --dry-run           display commands without modifying any files" <<  std::endl
		<< "  -d, --debug             enable debugging traces" <<                       std::endl
		<< "  -v, --verbose           verbosely report processing" <<                   std::endl
		<< "  -h, --help              print this help, then exit" <<                    std::endl
		<< "  -V, --version           print version number, then exit" <<               std::endl
		<<                                                                              std::endl
		<< "Specify TRANSFORMATION using the format INPUTFILE<SEPARATOR>OUTPUTFILE." << std::endl
		<< "Default SEPARATOR is '" << separator << "'." <<                             std::endl
		<<                                                                              std::endl
		<< "Report bugs to <" << PACKAGE_BUGREPORT << ">" <<                            std::endl;
}

void hint(const std::string & message)
{
	BUG_ON(message.size() == 0);

	std::cout
		<< message <<                                                 std::endl
		<< "Try `" << PROGRAM_NAME << " -h' for more information." << std::endl;
}

Graph::DAG * read_rules(const std::string & filename)
{
	TR_DBG("Reading rules from file '%s'\n", filename.c_str());

	Graph::DAG * dag;

	dag = new Graph::DAG();

	std::ifstream stream;

	stream.open(filename.c_str());
	if (!stream) {
		throw Exception("Cannot open '" + filename + "' for reading");
	}

	std::string line   = "";
	size_t      number = 0;
	enum {
		S_IDLE,
		S_HEADER,
		S_BODY,
		S_COMPLETE,
	}           state = S_IDLE;

	std::string tag_in  = "";
	std::string tag_out = "";
	std::string command = "";

	while (!stream.eof()) {
		if (state == S_IDLE) {
			// Read a new line
			std::getline(stream, line);
			number++;
			TR_DBG("  State %d, number %d, line '%s'\n",
			       state, number, line.c_str());

			// Remove comments
			line = line.substr(0, line.find("#"));
			line = String::trim_right(line, " \t");

			TR_DBG("  line %d = '%s'\n", number, line.c_str());

			if (line == "") {
				// Discard empty lines
				continue;
			}

			// Line is not empty, start reading header
			state = S_HEADER;
		} else if (state == S_HEADER) {
			std::string::size_type delimiter_pos;

			delimiter_pos = line.find(":");
			if (delimiter_pos >= std::string::npos) {
				throw Exception("No delimiter found "
						"at line '" + line + "'");
			}

			tag_in = line.substr(0, delimiter_pos);
			if (tag_in == "") {
				throw Exception("Missing input tag "
						"at line '" + line + "'");
			}

			tag_out = line.substr(delimiter_pos + 1);
			if (tag_out == "") {
				throw Exception("Missing output tag "
						"at line '" + line + "'");
			}

			BUG_ON(tag_in  == "");
			BUG_ON(tag_out == "");

			TR_DBG("  tag in  = '%s'\n", tag_in.c_str());
			TR_DBG("  tag out = '%s'\n", tag_out.c_str());

			// Header read, start reading body
			state = S_BODY;
		} else if (state == S_BODY) {
			// Read a new line
			std::getline(stream, line);
			number++;
			TR_DBG("  State %d, number %d, line '%s'\n",
			       state, number, line.c_str());

			// Empty lines complete the body part
			if (line.size() == 0) {
				state = S_COMPLETE;
				continue;
			}
			if (line[0] != '\t') {
				throw Exception("Wrong body "
						"at line '" + line + "'");
			}

			command += line;

		} else if (state == S_COMPLETE) {
			BUG_ON(tag_in  == "");
			BUG_ON(tag_out == "");
			BUG_ON(command == "");

			TR_DBG("  %s -> %s\n", tag_in.c_str(), tag_out.c_str());
			TR_DBG("  %s\n",       command.c_str());

			// Add the filter to the DAG
			dag->add(tag_in, tag_out, command);

			state = S_IDLE;

			tag_in  = "";
			tag_out = "";
			command = "";
		} else {
			BUG();
		}
	}

	stream.close();

	return dag;
}

int main(int argc, char * argv[])
{
	TR_CONFIG_LVL(TR_LVL_DEFAULT);
	TR_CONFIG_PFX(PROGRAM_NAME);

	try {
		bool dry_run   = false;

		int c;
		// int digit_optind = 0;
		while (1) {
			// int this_option_optind = optind ? optind : 1;
			int option_index       = 0;

			static struct option long_options[] = {
				{ "config",    1, 0, 'c' },
				{ "rules",     1, 0, 'r' },
				{ "separator", 1, 0, 's' },
				{ "dry-run",   0, 0, 'n' },
				{ "debug",     0, 0, 'd' },
				{ "verbose",   0, 0, 'v' },
				{ "version",   0, 0, 'V' },
				{ "help",      0, 0, 'h' },
				{ 0,           0, 0, 0   }
			};

#if USE_CONFIGURATION_FILE
			c = getopt_long(argc, argv, "c:s:ndvVh",
					long_options, &option_index);
#else
			c = getopt_long(argc, argv, "s:ndvVh",
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
				case 's':
					if (strlen(optarg) > 1) {
						hint("Separator too long");
						return 1;
					}
					separator = optarg[0];
					break;
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
					return 1;
				default:
					BUG();
					return 1;
			}
		}

		// Gather input file names
		if (optind >= argc) {
			hint("Missing input file name(s)");
			return 1;
		}

		TR_DBG("Separator          '%c'\n", separator);
#if USE_CONFIGURATION_FILE
		TR_DBG("Configuration file '%s'\n",
		       configuration_file.c_str());
		BUG_ON(configuration_file.size() == 0);
#endif
		TR_DBG("Rules file         '%s'\n",
		       rules_file.c_str());
		BUG_ON(rules_file.size() == 0);

		std::vector<Transformation *> transformations;

		assert((argc - optind) >= 0);
		transformations.resize(argc - optind);

		TR_DBG("Transformations:\n");

		int i, j;
		for (i = optind; i < argc; i++) {
			j = i - optind;
			try {
				transformations[j] =
					new Transformation(argv[i], separator);
			} catch (std::exception & e) {
				TR_ERR("%s\n", e.what());
				return 1;
			}

			BUG_ON(transformations[j] == 0);

			TR_DBG("  %s = '%s' -> '%s'\n",
			       transformations[j]->tag().c_str(),
			       transformations[j]->input().name().c_str(),
			       transformations[j]->output().name().c_str());
		}

#if USE_CONFIGURATION_FILE
		// Read configuration file
		try {
			Configuration::File config;
			std::ifstream       stream(configuration_file.c_str());

			stream >> config;

		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
		} catch (...) {
			BUG();
		}
#endif

		// Read rules file
		Graph::DAG * dag = 0;
		try {
			dag = read_rules(rules_file);
		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
		} catch (...) {
			BUG();
		}
		BUG_ON(!dag);

		std::vector<Transformation *>::iterator iter;

		// Inject filter-chains into each transformation
		for (iter  = transformations.begin();
		     iter != transformations.end();
		     iter++) {
			// Extract filters chain
			std::vector<Graph::Node *> filters;
			filters = dag->chain((*iter)->input().type(),
					     (*iter)->output().type());

			// Check filter chain size before starting execution
			if (filters.size() == 0) {
				TR_ERR("No filter chain available "
				       "for transformation '%s'\n",
				       (*iter)->tag().c_str());
				return 1;
			}

			// Inject the chain, finally
			(*iter)->filters(filters);
		}

		// Perform all transformations
		for (iter  = transformations.begin();
		     iter != transformations.end();
		     iter++) {
			if (dry_run) {
				std::vector<Graph::Node *> filters;
				filters = (*iter)->filters();

				std::vector<Graph::Node *>::iterator it;
				for (it  = filters.begin();
				     it != filters.end();
				     it++) {
					std::cout << (*it)->command()
						  << std::endl;
				}
			}

			if (!(*iter)->execute()) {
				TR_ERR("Cannot perform transformation '%s'\n",
				       (*iter)->tag().c_str());
			}
		}

		TR_DBG("Operation complete\n");
	} catch (std::exception & e) {
		TR_ERR("Got exception '%s'\n", e.what());
		return 1;
	} catch (...) {
		BUG();
	};

	return 0;
}
