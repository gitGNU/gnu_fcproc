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
	Rules::Rules(const std::vector<std::string> & filenames)
	{
		if (regcomp(&re_.empty_,
			    "^[ \t]*$",
			    REG_NOSUB)) {
			throw Exception("Cannot compile empty regexp");
		}

		if (regcomp(&re_.comment_,
			    "^[ \t]*#.*$",
			    REG_NOSUB)) {
			throw Exception("Cannot compile comment regexp");
		}

		if (regcomp(&re_.include_,
			    "^[ \t]*include[ \t]+\"(.*)\"[ \t]*$",
			    REG_EXTENDED)) {
			throw Exception("Cannot compile include regexp");
		}

		if (regcomp(&re_.header_,
			    "^(.*):(.*)[ \t]*$",
			    REG_EXTENDED)) {
			throw Exception("Cannot compile header regexp");
		}

		if (regcomp(&re_.body_,
			    "^\t(.*)$",
			    REG_EXTENDED)) {
			throw Exception("Cannot compile body regexp");
		}

		std::vector<std::string>::const_iterator iter;
		for (iter  = filenames.begin();
		     iter != filenames.end();
		     iter++) {
			parse((* iter));
		}

		regfree(&re_.body_);
		regfree(&re_.header_);
		regfree(&re_.include_);
		regfree(&re_.comment_);
		regfree(&re_.empty_);

#if 0
		std::map<std::string,
			std::map<std::string,
			std::vector<std::string> > >::const_iterator in;
		std::map<std::string,
			std::vector<std::string> >::const_iterator   out;

		TR_DBG("Known rules:\n");
		for (in  = rules_.begin();
		     in != rules_.end();
		     in++) {
			TR_DBG("  '%s' ->\n", in->first.c_str());
			for (out  = in->second.begin();
			     out != in->second.end();
			     out++) {
				// There should be no loops
				BUG_ON(in->first == out->first);

				TR_DBG("    '%s'\n", out->first.c_str());
			}
		}
#endif
	}

	Rules::~Rules(void)
	{
	}

	void Rules::dump(std::ostream & stream)
	{
		std::map<std::string,
			std::map<std::string,
			std::vector<std::string> > >::const_iterator i;
		for (i  = rules_.begin();
		     i != rules_.end();
		     i++) {
			std::map<std::string,
				std::vector<std::string> >::const_iterator j;
			for (j  = i->second.begin();
			     j != i->second.end();
			     j++) {
				stream << i->first
				       << " -> "
				       << j->first
				       << std::endl;
			}
		}
	}

#define PARSER_DEBUGS 1
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

	std::string Rules::readline(std::ifstream & stream)
	{
		std::string ret;
		bool        stop;

		stop = false;
		while (!stop) {
			std::string tmp;

			std::getline(stream, tmp);
			tmp = String::trim_right(tmp, " \t");
			if (tmp.find_last_of("\\") != tmp.size()) {
				stop = true;
			}

			ret = ret + tmp;
		}

		return ret;
	}

	void Rules::parse(const std::string & filename)
	{
		// Always dump the file under examination
		TR_DBG("Parsing rules from file '%s'\n", filename.c_str());

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
				line = readline(stream); number++;

				P_DBG("  line %d = '%s'\n",
				      number, line.c_str());

				// Is this an empty line ?
				if (regexec(&re_.empty_,
					    line.c_str(),
					    3, re_.match_, 0) == 0) {
					P_DBG("  Got empty line\n");
					continue;
				}

				// Is this a comment line ?
				if (regexec(&re_.comment_,
					    line.c_str(),
					    3, re_.match_, 0) == 0) {
					P_DBG("  Got comment line\n");
					continue;
				}

				// Is this an include line ?
				if (regexec(&re_.include_,
					    line.c_str(),
					    3, re_.match_, 0) == 0) {

					DUMP_REGMATCHES(re_.match_);

					std::string include;
					include =
						line.substr(re_.match_[1].rm_so,
							    re_.match_[1].rm_eo -
							    re_.match_[1].rm_so);

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
				if (regexec(&re_.header_,
					    line.c_str(),
					    3, re_.match_, 0) != 0) {
					throw Exception("Missing header "
							"in file "
							"'" + filename + "'"
							" at line "
							"'" +
							String::itos(number) +
							"'");
				}

				P_DBG("  Got header\n");

				DUMP_REGMATCHES(re_.match_);

				tag_in = line.substr(re_.match_[1].rm_so,
						     re_.match_[1].rm_eo -
						     re_.match_[1].rm_so);
				if (tag_in == "") {
					throw Exception("Missing input tag "
							"in file "
							"'" + filename + "'"
							" at line "
							"'" +
							String::itos(number) +
							"'");
				}

				tag_out = line.substr(re_.match_[2].rm_so,
						      re_.match_[2].rm_eo -
						      re_.match_[2].rm_so);
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
				line = readline(stream); number++;

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

				if (regexec(&re_.body_,
					    line.c_str(),
					    3, re_.match_, 0) != 0) {
					throw Exception("Wrong body "
							"in file "
							"'" + filename + "'"
							" at line "
							"'" +
							String::itos(number) +
							"'");
				}

				DUMP_REGMATCHES(re_.match_);

				line = line.substr(re_.match_[1].rm_so,
						   re_.match_[1].rm_eo -
						   re_.match_[1].rm_so);

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

				(rules_[tag_in])[tag_out] = commands;

				P_DBG("  Added rule\n");

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
			(rules_[tag_in])[tag_out] = commands;
		}

		stream.close();
	}

	bool Rules::chain(std::set<std::pair<std::string,
			  std::string> > &                         loop,
			  const std::string &                      in,
			  const std::string &                      out,
			  int                                      mdepth,
			  std::vector<std::vector<std::string> > & commands)
	{
		BUG_ON(mdepth < 0);

		TR_DBG("Looking for '%s' -> '%s' (mdepth = %d)\n",
		       in.c_str(), out.c_str(), mdepth);

		mdepth--;
		if (mdepth < 0) { // Allow mdepth == 1
			TR_DBG("Max filters-chain size exceeded\n");
			return false;
		}

		std::map<std::string,
			std::map<std::string,
			std::vector<std::string> > >::const_iterator r;
		r = rules_.find(in);
		if (r == rules_.end()) {
			TR_DBG("No rules available for '%s' -> '%s'\n",
			       in.c_str(), out.c_str());
			return false;
		}

		// Loop detection
		std::pair<std::string, std::string> t(in, out);
		if (loop.find(t) != loop.end()) {
			TR_DBG("Got a loop while "
			       "looking for '%s' -> '%s'\n",
			       in.c_str(), out.c_str());
			return false;
		}
		loop.insert(t);

		std::map<std::string,
			std::vector<std::string> >::const_iterator c;
		for (c = r->second.begin(); c != r->second.end(); c++) {
			if (c->first == out) {
				TR_DBG("Got chain end!\n");
				commands.push_back(c->second);
				return true;
			}

			TR_DBG("Looking for '%s' -> '%s'\n",
			       c->first.c_str(), out.c_str());
			if (chain(loop, c->first, out, mdepth, commands)) {
				commands.push_back(c->second);
				TR_DBG("Got '%s' -> '%s'\n",
				       c->first.c_str(), out.c_str());
				return true;
			}
		}

		return false;
	}

	std::vector<FCP::Filter *> Rules::chain(const FS::File & input,
						const FS::File & output,
						int              mdepth)
	{
		BUG_ON(mdepth <= 0);

		TR_DBG("Looking for filters-chain for '%s' -> '%s' "
		       "(max depth %d)\n",
		       input.name().c_str(), output.name().c_str(), mdepth);

		std::set<std::pair<std::string, std::string> > loop;
		std::vector<FCP::Filter *>                     ret;
		std::vector<std::vector<std::string> >         commands;

		if (!chain(loop,
			   input.extension(),
			   output.extension(),
			   mdepth,
			   commands)) {
			TR_DBG("No filters-chain found ...\n");
			commands.clear();
			return ret;
		}

		TR_DBG("filters-chain found!\n");
		std::reverse(commands.begin(), commands.end());

		// Transform the command sequence into a proper filters-chain
		std::vector<std::vector<std::string> >::iterator i;
		for (i = commands.begin(); i != commands.end(); i++) {
			FCP::Filter * f;

			f = new FCP::Filter(input, output, (*i));
			ret.push_back(f);
		}

		return ret;
	}
}
