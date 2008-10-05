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
#include <vector>
#include <iostream>

#include "libs/misc/debug.h"
#include "job.h"
#include "file.h"
#include "rule.h"

namespace FCP {
	Job::Job(const std::string &        id,
		 const FCP::File &          input,
		 std::vector<FCP::Rule *> & rules,
		 const FCP::File &          output) :
		id_(id),
		input_(input),
		rules_(rules),
		output_(output)
	{
	}

	Job::~Job(void)
	{
	}

	void Job::run(bool dry_run)
	{
		std::vector<FCP::Rule *>::size_type c;
		std::vector<FCP::Rule *>::iterator  iter;

		c = 1;
		for (iter = rules_.begin(); iter != rules_.end(); iter++) {
			TR_DBG("Running command (%d/%d)\n",
			       c, rules_.size());

			if (dry_run) {
				std::cout
					<< id_ << ": "
					<< (*iter)->command()
					<< std::endl;
			}
			
			c++;
		}
	}
};
