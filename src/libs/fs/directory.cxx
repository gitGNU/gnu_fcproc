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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "libs/fs/directory.h"

namespace FS {
	Directory::Directory(const std::string & name)
	{
		if (name == "") {
			throw Exception("Missing directory name");
		}

		// Remove trailing '/'
		std::string::size_type p;

		p = name.rfind("/");
		//TR_DBG("name = '%s', p = %d, size = %d\n",
		//       name.c_str(), p, name.size());

		if (p == (name.size() - 1)) {
			name_ = name.substr(0, p);
			return;
		}

		name_ = name;
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
		// XXX FIXME: Consider using the gnulib replacement
		if (::mkdir(name_.c_str(), 0700) != 0) {
			throw Exception("Cannot create "
					"'" + name_ + "' "
					"directory "
					"(" +
					std::string(strerror(errno)) +
					")");
		}
	}

	void Directory::remove(void) const
	{
		// XXX FIXME: Consider using the gnulib replacement
		if (::rmdir(name_.c_str()) != 0) {
			throw Exception("Cannot remove "
					"'" + name_ + "' "
					"directory "
					"(" +
					std::string(strerror(errno)) +
					")");
		}
	}

	bool Directory::exists(void) const
	{
		// XXX FIXME: Consider using the gnulib replacement
		DIR * tmp = opendir(name_.c_str());
		if (tmp == 0) {
			BUG_ON(errno == EBADF);

			throw Exception("Cannot open "
					"'" + name_ + "' "
					"directory "
					"(" +
					std::string(strerror(errno)) +
					")");
		}

		// XXX FIXME: Maybe a check on closedir() return value ...
		closedir(tmp);

		return true;
	}
};
