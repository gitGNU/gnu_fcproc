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

#include "libs/misc/debug.h"
#include "libs/fs/utils.h"
#include "libs/misc/exception.h"
#include "directory.h"

namespace FCP {
	Directory::Directory(const std::string & name) :
		name_(name)
	{
		if (name_ == "") {
			throw Exception("Missing directory name");
		}
	}

	Directory::~Directory(void)
	{
	}

	const std::string & Directory::name(void) const
	{
		return name_;
	}

	void Directory::create(void) const
	{
		::Directory::mkdir(name_);
	}

	void Directory::remove(void) const
	{
		::Directory::rmdir(name_);
	}

	bool Directory::exists(void) const
	{
		return ::Directory::exists(name_);
	}
};
