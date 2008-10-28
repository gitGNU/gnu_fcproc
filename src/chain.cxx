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

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <csignal>

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "libs/misc/exception.h"
#include "chain.h"
#include "file.h"
#include "filter.h"

namespace FCP {
	Chain::Chain(const std::string &          id,
		     const FCP::File &            input,
		     std::vector<FCP::Filter *> & filters,
		     const FCP::File &            output) :
		id_(id),
		input_(input),
		output_(output),
		temp_count_(0)
	{
		std::vector<FCP::Filter *>::iterator ir;
		for (ir  = filters.begin();
		     ir != filters.end();
		     ir++) {
			std::vector<std::string>::const_iterator ic;
			for (ic  = (*ir)->commands().begin();
			     ic != (*ir)->commands().end();
			     ic++) {
				std::string command;

				command = (*ic);

				commands_.push_back(command);
			}
		}
	}

	Chain::~Chain(void)
	{
	}

	const std::string & Chain::id(void)
	{
		return id_;
	}

	const std::vector<std::string> & Chain::commands(void)
	{
		return commands_;
	}

	std::string Chain::mktemp(const std::string & dir)
	{
		std::string       t;
		std::stringstream s;

		s << temp_count_;

		t = dir                   +
			std::string("/")       +
			id_ + std::string("-") +
			s.str();

		temp_count_++;

		return t;
	}

	void Chain::setup(const std::string & dir)
	{
		std::vector<std::string>::iterator ic;

		//TR_DBG("Replacing variables\n")
		for (ic  = commands_.begin();
		     ic != commands_.end();
		     ic++) {
			std::string command;

			command = (*ic);

			command = String::replace(command,
						  "$I",
						  input_.name());
			command = String::replace(command,
						  "$O",
						  output_.name());

			//TR_DBG("  Command '%s'\n", command.c_str());

			// Ugly
			for (;;) {
				//TR_DBG("    Replace in progress\n");
				std::string::size_type s;
				std::string::size_type e;
				std::string            v;

				s = 0;
				e = 0;

				//
				// XXX FIXME:
				//     Add environment variable substitution
				//

				s = command.find("$T");
				//TR_DBG("      s = %d, e = %d\n", s, e);
				if ((s == 0) || (s == std::string::npos)) {
					break;
				}

				e = command.find_first_not_of("0123456789",
							      s + 1);
				//TR_DBG("      s = %d, e = %d\n", s, e);
				if ((e == 0) || (e == std::string::npos)) {
					break;
				}

				v = command.substr(s, e - s + 2);
				BUG_ON(v.size() == 0);

				//TR_DBG("      v = '%s'\n", v.c_str());

				std::string t;
				t = temps_[v];
				if (t == "") {
					t  = mktemp(dir);
					temps_[v] = t;
				}

				command = String::replace(command, v, t);

				//TR_DBG("    Replaced '%s' with '%s'\n",
				//       v.c_str(), t.c_str());
			}

			//TR_DBG("    Command is now '%s'\n", command.c_str());

			(*ic) = command;
		}
	}

	void Chain::run(bool dry_run)
	{
		std::vector<std::string>::iterator ic;
		std::vector<std::string>::size_type count;
		std::vector<std::string>::size_type all;

		all   = commands_.size();
		count = 1;
		for (ic  = commands_.begin();
		     ic != commands_.end();
		     ic++, count++) {
			if (dry_run) {
				TR_VRB("%s\n", (*ic).c_str());
				continue;
			}

			int ret;

			BUG_ON((*ic).size() == 0);
			ret = system((*ic).c_str());
			if (ret == -1) {
				throw Exception("Got fork() failure");
			}
			if (WIFSIGNALED(ret) &&
			    (WTERMSIG(ret) == SIGINT ||
			     WTERMSIG(ret) == SIGQUIT)) {
				throw Exception("Interrupted");
			}
			if (WEXITSTATUS(ret) != 0) {
				throw Exception("Got problems running "
						"command '" + (*ic) + "'");
			}
		}
	}
};
