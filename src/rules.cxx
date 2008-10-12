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
#include "libs/misc/exception.h"
#include "rules.h"

namespace FCP {
	Rules::Rules(const std::string & filename)
	{
		if (regcomp(&re_empty_,
			    "^[ \t]*$",
			    REG_NOSUB)) {
			throw Exception("Cannot compile empty regexp");
		}

		if (regcomp(&re_comment_,
			    "^[ \t]*#.*$",
			    REG_NOSUB)) {
			throw Exception("Cannot compile comment regexp");
		}

		if (regcomp(&re_include_,
			    "^[ \t]*include[ \t]+\"(.*)\"[ \t]*$",
			    REG_EXTENDED)) {
			throw Exception("Cannot compile include regexp");
		}

		if (regcomp(&re_header_,
			    "^(.*):(.*)[ \t]*$",
			    REG_EXTENDED)) {
			throw Exception("Cannot compile header regexp");
		}

		if (regcomp(&re_body_,
			    "^\t(.*)$",
			    REG_EXTENDED)) {
			throw Exception("Cannot compile body regexp");
		}

		parse(filename);

		regfree(&re_body_);
		regfree(&re_header_);
		regfree(&re_include_);
		regfree(&re_comment_);
		regfree(&re_empty_);

		std::map<std::string, std::set<FCP::Rule *> >::iterator ir;
		std::set<FCP::Rule *>::iterator                         is;

		TR_DBG("Known rules:\n");
		for (ir  = rules_.begin();
		     ir != rules_.end();
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
	}

	Rules::~Rules(void)
	{
	}

#define PARSER_DEBUGS 0
#if PARSER_DEBUGS
#define P_DBG(FMT,ARGS...) TR_DBG(FMT, ##ARGS);
#else
#define P_DBG(FMT,ARGS...)
#endif

#define DUMP_REGMATCH(X) {						\
		P_DBG("    regexmatch_t.rm_so = %d\n", (X).rm_so);	\
		P_DBG("    regexmatch_t.rm_eo = %d\n", (X).rm_eo);	\
	}

#if 0
#define DUMP_REGMATCHES(X) {			\
	DUMP_REGMATCH((X)[0]);			\
	DUMP_REGMATCH((X)[1]);			\
	DUMP_REGMATCH((X)[2]);			\
}
#else
#define DUMP_REGMATCHES(X)
#endif

	void Rules::parse(const std::string & filename)
	{
		P_DBG("Parsing rules from file '%s'\n",
		      filename.c_str());

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
			S_RULE_HEADER,
			S_RULE_BODY,
			S_RULE_COMPLETE,
		} state;

		std::string              tag_in   = "";
		std::string              tag_out  = "";
		std::vector<std::string> commands;

		commands.clear();

		state = S_IDLE;
		while (!stream.eof()) {
			if (state == S_IDLE) {
				P_DBG("State %d, number %d, line '%s'\n",
				      state, number, line.c_str());

				// Read a new line
				std::getline(stream, line); number++;

				P_DBG("  line %d = '%s'\n",
				      number, line.c_str());

				// Is this an empty line ?
				if (regexec(&re_empty_,
					    line.c_str(),
					    3, re_match_, 0) == 0) {
					P_DBG("  Got empty line\n");
					continue;
				}

				// Is this a comment line ?
				if (regexec(&re_comment_,
					    line.c_str(),
					    3, re_match_, 0) == 0) {
					P_DBG("  Got comment line\n");
					continue;
				}

				// Is this an include line ?
				if (regexec(&re_include_,
					    line.c_str(),
					    3, re_match_, 0) == 0) {

					DUMP_REGMATCHES(re_match_);

					std::string include;
					include =
						line.substr(re_match_[1].rm_so,
							    re_match_[1].rm_eo -
							    re_match_[1].rm_so);

					P_DBG("  Got include is '%s'\n",
					      include.c_str());
					parse(include);
					continue;
				}

				// Line is not empty, start reading header
				state = S_RULE_HEADER;
				continue;

			} else if (state == S_RULE_HEADER) {
				P_DBG("State %d, number %d, line '%s'\n",
				      state, number, line.c_str());

				// Is this an header line ?
				if (regexec(&re_header_,
					    line.c_str(),
					    3, re_match_, 0) != 0) {
					throw Exception("Missing header "
							"in file "
							"'" + filename + "'"
							" at line "
							"'" +
							String::itos(number) +
							"'");
				}

				P_DBG("  Got header\n");

				DUMP_REGMATCHES(re_match_);

				tag_in = line.substr(re_match_[1].rm_so,
						     re_match_[1].rm_eo -
						     re_match_[1].rm_so);
				if (tag_in == "") {
					throw Exception("Missing input tag "
							"in file "
							"'" + filename + "'"
							" at line "
							"'" +
							String::itos(number) +
							"'");
				}

				tag_out = line.substr(re_match_[2].rm_so,
						      re_match_[2].rm_eo -
						      re_match_[2].rm_so);
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
				P_DBG("State %d, number %d, line '%s'\n",
				      state, number, line.c_str());

				// Read a new line
				std::getline(stream, line); number++;

				P_DBG("  State %d, number %d, line '%s'\n",
				      state, number, line.c_str());

				// Empty lines complete the body part
				if (line.size() == 0) {
					if (commands.size() == 0) {
						throw Exception("Missing body "
								"in file "
								"'" + filename + "'"
								" at line "
								"'" +
								String::itos(number) +
								"'");
					}

					state = S_RULE_COMPLETE;
					continue;
				}

				if (regexec(&re_body_,
					    line.c_str(),
					    3, re_match_, 0) != 0) {
					throw Exception("Wrong body "
							"in file "
							"'" + filename + "'"
							" at line "
							"'" +
							String::itos(number) +
							"'");
				}

				DUMP_REGMATCHES(re_match_);

				line = line.substr(re_match_[1].rm_so,
						   re_match_[1].rm_eo -
						   re_match_[1].rm_so);

				commands.push_back(line);
				continue;

			} else if (state == S_RULE_COMPLETE) {
				P_DBG("State %d, number %d, line '%s'\n",
				      state, number, line.c_str());

				BUG_ON(tag_in  == "");
				BUG_ON(tag_out == "");
				BUG_ON(commands.size() < 1);

				P_DBG("  %s -> %s\n",
				      tag_in.c_str(), tag_out.c_str());

				FCP::Rule * r;

				r = new FCP::Rule(tag_in, tag_out, commands);
				BUG_ON(r == 0);

				rules_[tag_in].insert(r);

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

		// Do we need to push the last command ?
		if (commands.size() && tag_in != "" && tag_out != "") {
			// Yes we do

			FCP::Rule * r;

			r = new FCP::Rule(tag_in, tag_out, commands);
			BUG_ON(r == 0);

			rules_[tag_in].insert(r);
		}

		stream.close();
	}

	bool Rules::build_chain(const std::string &         in,
				const std::string &         out,
				int                         mdepth,
				std::vector<FCP::Rule *> &  chain)
	{
		BUG_ON(mdepth <= 0);

		mdepth--;
		if (mdepth == 0) {
			// Max filters-chain size exceeded
			return false;
		}

		std::map<std::string,
			std::set<FCP::Rule *> >::const_iterator r;
		r = rules_.find(in);
		if (r == rules_.end()) {
			return false;
		}

		std::set<FCP::Rule *>::const_iterator i;

		for (i = (*r).second.begin(); i != (*r).second.end(); i++) {
			if ((*i)->output() == out) {
				//TR_DBG("Got chain!\n");
				chain.push_back(*i);
				return true;
			}

			if (build_chain((*i)->output(), out, mdepth, chain)) {
				chain.push_back(*i);
				return true;
			}
		}

		return false;
	}

	void Rules::chains(const std::string &        in,
			   const std::string &        out,
			   int                        mdepth,
			   std::vector<FCP::Rule *> & chain)
	{
		BUG_ON(in.size()  == 0);
		BUG_ON(out.size() == 0);
		BUG_ON(mdepth <= 0);

		TR_DBG("Looking for filters-chain '%s' -> '%s' "
		       "(max depth %d)\n",
		       in.c_str(), out.c_str(), mdepth);

		if (!build_chain(in, out, mdepth, chain)) {
			//TR_DBG("No chain found\n");
			chain.clear();
		} else {
			//TR_DBG("Chain found!\n");
			std::reverse(chain.begin(), chain.end());
		}
	}
}
