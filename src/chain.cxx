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
		filters_(filters)
	{
		TR_DBG("Chain '%s' created\n", id.c_str());
	}

	Chain::~Chain(void)
	{
	}

	const std::string & Chain::id(void)
	{
		return id_;
	}

	void Chain::run(const std::string & dir,
			bool                dry)
	{
		TR_DBG("Running filters-chain '%s'\n", id_.c_str());

		if (!dry) {
			// Check the input and output file times in order to
			// avoid rebuilding up-to-date files

			if (!input_.ispresent()) {
				throw Exception("Missing input file "
						"'" + input_.name() + "'");
			}

			if (output_.ispresent() &&
			    (input_.mtime() >= output_.mtime())) {
				TR_DBG("Output file '%s' is up-to-date\n",
				       output_.name().c_str());
				return;
			}
		}

		std::vector<FCP::Filter *>::iterator i;
		for (i  = filters_.begin(); i != filters_.end(); i++) {
			if (dry) {
				std::vector<std::string> commands;

				commands = (*i)->commands(id_,
							  input_,
							  output_,
							  dir);

				std::vector<std::string>::iterator ic;
				for (ic  = commands.begin();
				     ic != commands.end();
				     ic++) {
					TR_VRB("%s\n", (*ic).c_str());
				}

				continue;
			}

			(*i)->run(id_, input_, output_, dir);
		}
	}
};
