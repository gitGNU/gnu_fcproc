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

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "exception.h"
#include "rule.h"

#define PARSER_DEBUGS 0
#if PARSER_DEBUGS
#define P_DBG(FMT,ARGS...) TR_DBG(FMT, ##ARGS);
#else
#define P_DBG(FMT,ARGS...)
#endif

namespace FCP {
	Rule::Rule(const std::string &              tag_in,
		   const std::string &              tag_out,
		   const std::vector<std::string> & commands) :
		tag_in_(tag_in),
		tag_out_(tag_out),
		commands_(commands)
	{
	}

	Rule::~Rule(void)
	{
	}

	const std::string & Rule::input(void)
	{
		return tag_in_;
	}

	const std::string & Rule::output(void)
	{
		return tag_out_;
	}

	const std::vector<std::string> & Rule::commands(void)
	{
		return commands_;
	}

	void parse_rules(const std::string &                             fname,
			 std::map<std::string, std::set<FCP::Rule *> > & rules)
	{
		P_DBG("Parsing rules from file '%s'\n", fname.c_str());

		std::ifstream stream;

		stream.open(fname.c_str());
		if (!stream) {
			throw Exception("Cannot open file "
					"'" + fname + "' "
					"for reading");
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

				P_DBG("  line %d = '%s'\n",
				      number, line.c_str());

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
							"at line "
							"'" + line + "'");
				}

				tag_in = line.substr(0, delimiter_pos);
				if (tag_in == "") {
					throw Exception("Missing input tag "
							"at line "
							"'" + line + "'");
				}

				tag_out = line.substr(delimiter_pos + 1);
				if (tag_out == "") {
					throw Exception("Missing output tag "
							"at line "
							"'" + line + "'");
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
							"at line "
							"'" + line + "'");
				}
				line = String::trim_both(line, " \t");
				commands.push_back(line);

			} else if (state == S_COMPLETE) {
				BUG_ON(tag_in  == "");
				BUG_ON(tag_out == "");
				BUG_ON(commands.size() < 1);

				P_DBG("  %s -> %s\n",
				      tag_in.c_str(), tag_out.c_str());
				P_DBG("  %s\n",
				      commands.c_str());

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
};
