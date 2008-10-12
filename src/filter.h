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

#ifndef FILTER_H
#define FILTER_H

#include "config.h"

#include <vector>
#include <string>
#include <map>

#include "file.h"
#include "rule.h"

namespace FCP {
	class Filter {
	public:
		Filter(const std::string &        id,
		       const FCP::File &          input,
		       std::vector<FCP::Rule *> & rules,
		       const FCP::File &          output);
		~Filter(void);

		const std::string &              id(void);
		const std::vector<std::string> & commands(void);
		void                             setup(const std::string & dir);
		void                             run(bool dry_run);

	protected:

	private:
		std::string              id_;
		FCP::File                input_;
		std::vector<std::string> commands_;
		FCP::File                output_;
		std::map<std::string,
			 std::string>    temps_;

		int                      temp_count_;

		std::string         mktemp(const std::string & dir);
	};
}

#endif // FILTER_H
