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
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <fstream>

#include "regex.h"

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "exception.h"
#include "rules.h"

#define PARSER_DEBUGS 1
#if PARSER_DEBUGS
#define P_DBG(FMT,ARGS...) TR_DBG(FMT, ##ARGS);
#else
#define P_DBG(FMT,ARGS...)
#endif

namespace FCP {
	namespace Rules {
		// XXX FIXME: Rewrite parse() ... it is really ugly
		void parse(const std::string &       filename,
			   std::map<std::string,
			   std::set<FCP::Rule *> > & rules)
		{
			P_DBG("Parsing rules from file '%s'\n", filename.c_str());

			std::ifstream stream;

			stream.open(filename.c_str());
			if (!stream) {
				throw Exception("Cannot open file "
						"'" + filename + "' "
						"for reading");
			}

			std::string line   = "";
			size_t      number = 0;
			enum {
				S_IDLE,
				S_INCLUDE,
				S_RULE_HEADER,
				S_RULE_BODY,
				S_RULE_COMPLETE,
			} state;

			std::string              tag_in   = "";
			std::string              tag_out  = "";
			std::vector<std::string> commands;

			regmatch_t re_match;
			regex_t *  re_comment = 0;
			regex_t *  re_include = 0;
			regex_t *  re_header  = 0;

			{
				int ret;

				ret = regcomp(re_comment,
					      "^[ \t]*#.*$", 0);
				if (ret != 0) {
					throw Exception("Cannot compile "
							"comment regexp");
				}
				BUG_ON(re_comment != 0);

				ret = regcomp(re_include,
					      "^[ \t]*include[ \t]+\"(.*)\"[ \t]*$", 0);
				if (ret != 0) {
					throw Exception("Cannot compile "
							"include regexp");
				}
				BUG_ON(re_include != 0);

				ret = regcomp(re_header,
					      "^(.*):(.*)[ \t]*$", 0);
				if (ret != 0) {
					throw Exception("Cannot compile include regexp");
				}
				BUG_ON(re_include != 0);

				commands.clear();

				state = S_IDLE;
				while (!stream.eof()) {
					if (state == S_IDLE) {
						// Read a new line
						std::getline(stream, line); number++;

						P_DBG("  State %d, number %d, line '%s'\n",
						      state, number, line.c_str());

						// Remove comments from line
						line = line.substr(0, line.find("#"));
						line = String::trim_right(line, " \t");

						P_DBG("  line %d = '%s'\n",
						      number, line.c_str());

						if (line == "") {
							// Discard empty lines
							continue;
						}

						// Is this an include line ?
						if (regexec(re_include, line.c_str(),
							    1, &re_match, 0)) {
							std::string include;
							include = line.substr(re_match.rm_so,
									      re_match.rm_eo);
							P_DBG("  include is '%s'\n",
							      include.c_str());
						}

						// Line is not empty, start reading header
						state = S_RULE_HEADER;
						continue;

					} else if (state == S_INCLUDE) {
						std::string incfile;

						incfile = "";

						parse(incfile, rules);

						state = S_IDLE;
						continue;

					} else if (state == S_RULE_HEADER) {
						std::string::size_type delimiter_pos;

						delimiter_pos = line.find(":");
						if (delimiter_pos >= std::string::npos) {
							throw Exception("No delimiter found "
									"in file "
									"'" + filename + "'"
									" at line "
									"'" +
									String::itos(number) +
									"'");
						}

						tag_in = line.substr(0, delimiter_pos);
						if (tag_in == "") {
							throw Exception("Missing input tag "
									"in file "
									"'" + filename + "'"
									" at line "
									"'" +
									String::itos(number) +
									"'");
						}

						tag_out = line.substr(delimiter_pos + 1);
						if (tag_out == "") {
							throw Exception("Missing output tag "
									"in file "
									"'" + filename + "'"
									" at line "
									"'" +
									String::itos(number) +
									"'");
						}

						BUG_ON(tag_in  == "");
						BUG_ON(tag_out == "");

						P_DBG("  tag in  = '%s'\n", tag_in.c_str());
						P_DBG("  tag out = '%s'\n", tag_out.c_str());

						// Header read, start reading body
						state = S_RULE_BODY;
						continue;

					} else if (state == S_RULE_BODY) {
						// Read a new line
						std::getline(stream, line); number++;

						P_DBG("  State %d, number %d, line '%s'\n",
						      state, number, line.c_str());

						// Empty lines complete the body part
						if (line.size() == 0) {
							state = S_RULE_COMPLETE;
							continue;
						}
						if (line[0] != '\t') {
							throw Exception("Wrong body "
									"in file "
									"'" + filename + "'"
									" at line "
									"'" +
									String::itos(number) +
									"'");
						}
						line = String::trim_both(line, " \t");
						commands.push_back(line);
						continue;

					} else if (state == S_RULE_COMPLETE) {
						BUG_ON(tag_in  == "");
						BUG_ON(tag_out == "");
						BUG_ON(commands.size() < 1);

						P_DBG("  %s -> %s\n",
						      tag_in.c_str(), tag_out.c_str());

						FCP::Rule * r;

						r = new FCP::Rule(tag_in, tag_out, commands);
						BUG_ON(r == 0);

						rules[tag_in].insert(r);

						tag_in   = "";
						tag_out  = "";
						commands.clear();

						state = S_IDLE;
						continue;

					} else {
						BUG();
					}

					BUG();
				}

				regfree(re_include);
				stream.close();
			}
		}
	}
}
