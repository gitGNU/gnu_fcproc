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

namespace String {
	std::string trim_left(const std::string & s,
			      const std::string & t)
	{
		static std::string tmp;

		tmp = s;

		return tmp.erase(0, tmp.find_first_not_of(t));
	}

	std::string trim_right(const std::string & s,
			       const std::string & t)
	{
		static std::string tmp;

		tmp = s;

		return tmp.erase(tmp.find_last_not_of(t) + 1);
	}

	std::string trim_both(const std::string & s,
			      const std::string & t)
	{
		return trim_left(trim_right(s, t), t);
	}
}
