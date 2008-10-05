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
#include <list>
#include <set>

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "libs/misc/environment.h"
#include "libs/conf/configuration.h"
#include "libs/file/utils.h"
#include "exception.h"
#include "filter.h"
#include "rule.h"
#include "transformation.h"
#include "job.h"

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
		<< "  -m, --max-depth=NUM     use NUM as max filter-chain depth" <<                               std::endl
		<< "                          [default " << max_depth << "]" <<                                   std::endl
		<< "  -t, --temp-dir=DIR      use DIR as temporary directory" <<                                  std::endl
		<< "                          [default " << temp_dir << "]" <<                                    std::endl
		<< "  -s, --separator=CHAR    use CHAR as INPUTFILE/OUTPUTFILE separator" <<                      std::endl
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

#define PARSER_DEBUGS 0
#if PARSER_DEBUGS
#define P_DBG(FMT,ARGS...) TR_DBG(FMT, ##ARGS);
#else
#define P_DBG(FMT,ARGS...)
#endif

void parse_rules(const std::string &                             filename,
		 std::map<std::string, std::set<FCP::Rule *> > & rules)
{
	P_DBG("Parsing rules from file '%s'\n", filename.c_str());

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

	std::string              tag_in   = "";
	std::string              tag_out  = "";
	std::vector<std::string> commands;

	commands.clear();

	while (!stream.eof()) {
		if (state == S_IDLE) {
			// Read a new line
			std::getline(stream, line);
			number++;
			P_DBG("  State %d, number %d, line '%s'\n",
			      state, number, line.c_str());

			// Remove comments
			line = line.substr(0, line.find("#"));
			line = String::trim_right(line, " \t");

			P_DBG("  line %d = '%s'\n", number, line.c_str());

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

			P_DBG("  tag in  = '%s'\n", tag_in.c_str());
			P_DBG("  tag out = '%s'\n", tag_out.c_str());

			// Header read, start reading body
			state = S_BODY;
		} else if (state == S_BODY) {
			// Read a new line
			std::getline(stream, line);
			number++;
			P_DBG("  State %d, number %d, line '%s'\n",
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

			commands.push_back(line);

		} else if (state == S_COMPLETE) {
			BUG_ON(tag_in  == "");
			BUG_ON(tag_out == "");
			BUG_ON(commands.size() < 1);

			P_DBG("  %s -> %s\n", tag_in.c_str(), tag_out.c_str());
			P_DBG("  %s\n",       commands.c_str());

			FCP::Rule * r;

			r = new FCP::Rule(tag_in, tag_out, commands);
			BUG_ON(r == 0);

			rules[tag_in].insert(r);

			tag_in   = "";
			tag_out  = "";
			commands.clear();

			state = S_IDLE;
		} else {
			BUG();
		}
	}

	stream.close();
}

bool find_chain(const std::map<std::string, std::set<FCP::Rule *> > & rules,
		const std::string &                                   in,
		const std::string &                                   out,
		int                                                   mdepth,
		std::vector<FCP::Rule *> &                            chain)
{
	BUG_ON(mdepth <= 0);

	mdepth--;
	if (mdepth == 0) {
		// Max filter-chain size exceeded
		return false;
	}

	std::map<std::string, std::set<FCP::Rule *> >::const_iterator r;
	r = rules.find(in);
	if (r == rules.end()) {
		return false;
	}

	std::set<FCP::Rule *>::const_iterator i;

	for (i = (*r).second.begin(); i != (*r).second.end(); i++) {
		if ((*i)->output() == out) {
			//TR_DBG("Got chain!\n");
			chain.push_back(*i);
			return true;
		}

		if (find_chain(rules,
			       (*i)->output(),
			       out,
			       mdepth,
			       chain)) {
			chain.push_back(*i);
			return true;
		}
	}

	return false;
}

void build_chain(const std::map<std::string, std::set<FCP::Rule *> > & rules,
		 const std::string &                                   in,
		 const std::string &                                   out,
		 int                                                   mdepth,
		 std::vector<FCP::Rule *> &                            chain)
{
	BUG_ON(in.size()  == 0);
	BUG_ON(out.size() == 0);
	BUG_ON(mdepth <= 0);

	TR_DBG("Looking for filter-chain '%s' -> '%s' (max depth %d)\n",
	       in.c_str(), out.c_str(), mdepth);

	if (!find_chain(rules, in, out, mdepth, chain)) {
		//TR_DBG("No chain found\n");
		chain.clear();
	} else {
		//TR_DBG("Chain found!\n");
		std::reverse(chain.begin(), chain.end());
	}
}

FCP::Job * transform(const FCP::Transformation & transformation,
		     const std::map<std::string,
		     std::set<FCP::Rule *> > &   rules,
		     int                         mdepth)
{
	BUG_ON(mdepth <= 0);

	TR_DBG("Transforming '%s' -> '%s'\n",
	       transformation.input().name().c_str(),
	       transformation.output().name().c_str());

	// Build the filter-chain
	std::vector<FCP::Rule *> chain;
	build_chain(rules,
		    transformation.input().extension(),
		    transformation.output().extension(),
		    mdepth,
		    chain);
	if (chain.size() == 0) {
		throw Exception("No filter-chain available for "
				"'" + transformation.tag() + "' "
				"transformation");
	}

	TR_DBG("Filter chain:\n");
	std::vector<FCP::Rule *>::iterator iter;
	for (iter = chain.begin(); iter != chain.end(); iter++) {
		TR_DBG("  '%s' -> '%s'\n",
		       (*iter)->input().c_str(),
		       (*iter)->output().c_str());
	}

	// Perform transformation now
	FCP::Job * j;

	j = new FCP::Job(transformation.tag(),
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
			c = getopt_long(argc, argv, "t:c:s:ndvVh",
					long_options, &option_index);
#else
			c = getopt_long(argc, argv, "t:s:ndvVh",
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

		TR_DBG("Separator     '%c'\n", separator);
#if USE_CONFIGURATION_FILE
		TR_DBG("Configuration '%s'\n", configuration_file.c_str());
		BUG_ON(configuration_file.size() == 0);
#endif
		TR_DBG("Rules         '%s'\n", rules_file.c_str());
		BUG_ON(rules_file.size() == 0);
		TR_DBG("Max depth     '%d'\n", max_depth);
		BUG_ON(max_depth <= 0);

		assert((argc - optind) >= 0);

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
		std::map<std::string, std::set<FCP::Rule *> >           rules;
		std::map<std::string, std::set<FCP::Rule *> >::iterator ir;
		std::set<FCP::Rule *>::iterator                         is;

		try {
			parse_rules(rules_file, rules);

			TR_DBG("Known rules:\n");
			for (ir  = rules.begin();
			     ir != rules.end();
			     ir++) {
				TR_DBG("  '%s' ->\n", (*ir).first.c_str());
				for (is  = (*ir).second.begin();
				     is != (*ir).second.end();
				     is++) {
					BUG_ON((*ir).first != (*is)->input());

					TR_DBG("    '%s'\n",
					       (*is)->output().c_str());
				}
			}
		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
			return 1;
		}

		// Get all jobs from transformations
		TR_DBG("Handling transformations\n");
		std::vector<FCP::Job *> jobs;
		try {
			for (it  = transformations.begin();
			     it != transformations.end();
			     it++) {
				FCP::Job * j;

				j = transform(*(*it), rules, max_depth);
				BUG_ON(j == 0);

				jobs.push_back(j);
			}
		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
			return 1;
		}

		// Run all jobs now
		TR_DBG("Running jobs\n");
		std::vector<FCP::Job *>::iterator ij;
		try {
			for (ij = jobs.begin(); ij != jobs.end(); ij++) {
				TR_DBG("Job '%s':\n", (*ij)->id().c_str());
				(*ij)->run(temp_dir);
			}
		} catch (std::exception & e) {
			TR_ERR("%s\n", e.what());
			return 1;
		}

		// Clean up everything
		TR_DBG("Operations complete, cleaning up ...\n");

		for (ij  = jobs.begin();
		     ij != jobs.end();
		     ij++) {
			delete (*ij);
		}
		for (it  = transformations.begin();
		     it != transformations.end();
		     it++) {
			delete (*it);
		}
		for (ir  = rules.begin();
		     ir != rules.end();
		     ir++) {
			for (is  = (*ir).second.begin();
			     is != (*ir).second.end();
			     is++) {
				delete (*is);
			}
		}

	} catch (std::exception & e) {
		TR_ERR("Got exception '%s'\n", e.what());
		return 1;
	} catch (...) {
		BUG();
		return 1; // Should be useless ...
	};

	return 0;
}
