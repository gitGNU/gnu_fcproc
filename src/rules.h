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

#ifndef RULES_H
#define RULES_H

#include "config.h"

#include <string>
#include <map>
#include <vector>

#include "regex.h"

#include "filter.h"

namespace FCP {
	class Rules {
	public:
		Rules(const std::string & filename);
		~Rules(void);

		void chains(const std::string &          in,
			    const std::string &          out,
			    int                          mdepth,
			    std::vector<FCP::Filter *> & chain);

	protected:

	private:
		struct {
			regmatch_t match_[3];
			regex_t    empty_;
			regex_t    comment_;
			regex_t    include_;
			regex_t    header_;
			regex_t    body_;
		} re_;

		std::map<std::string,
			 std::set<FCP::Filter *> > rules_;

		void parse(const std::string & filename);

		bool build_chain(std::set<std::pair<std::string,
						    std::string> > & loopset,
				 const std::string &                 in,
				 const std::string &                 out,
				 int                                 mdepth,
				 std::vector<FCP::Filter *> &        chain);
	};
}

#endif // RULES_H
