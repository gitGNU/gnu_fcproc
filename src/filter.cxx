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

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "libs/misc/exception.h"
#include "filter.h"

namespace FCP {
	Filter::Filter(const std::string &              tag_in,
		       const std::string &              tag_out,
		       const std::vector<std::string> & commands) :
		tag_in_(tag_in),
		tag_out_(tag_out),
		commands_(commands)
	{
	}

	Filter::~Filter(void)
	{
	}

	const std::string & Filter::input(void)
	{
		return tag_in_;
	}

	const std::string & Filter::output(void)
	{
		return tag_out_;
	}

	const std::vector<std::string> & Filter::commands(void)
	{
		return commands_;
	}
};
