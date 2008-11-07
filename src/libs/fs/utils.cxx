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

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "libs/fs/utils.h"
#include "libs/misc/exception.h"

namespace File {
	std::string dirname(const std::string & s)
	{
		return s.substr(0, s.rfind("/"));
	}

	std::string basename(const std::string & s)
	{
		std::string::size_type p;

		p = s.rfind("/");
		return (p != std::string::npos) ? s.substr(p + 1) : s;
	}

	std::string extension(const std::string & s)
	{
		std::string t;

		t = basename(s);
		if (t != "") {
			std::string::size_type p;

			p = t.rfind(".");
			if (p == 0) {
				// No extension, file is ".something"
				return "";
			}
			if (p == std::string::npos) {
				// No extension
				return "";
			}

			return t.substr(p + 1);
		}

		return t;
	}

	// XXX FIXME: Ugly
	bool exists(const std::string & s)
	{
		struct stat t;

		if (stat(s.c_str(), &t) == 0) {
			return true;
		} else {
			return false;
		}
	}

	time_t mtime(const std::string & s)
	{
		struct stat t;

		if (stat(s.c_str(), &t) != 0) {
			throw Exception("Cannot stat() file "
					"'" + s + "' "
					"(" +
					std::string(strerror(errno)) +
					")");
		}

		return t.st_mtime;
	}
}

namespace Directory {
	// XXX FIXME: Ugly, consider revising ...
	bool exists(const std::string & s)
	{
		BUG_ON(s.size() == 0);

		// XXX FIXME: Consider using gnulib replacement
		DIR * tmp = opendir(s.c_str());
		if (tmp == 0) {
			BUG_ON(errno == EBADF);

			throw Exception("Cannot open directory "
					"'" + s + "' "
					"(" +
					std::string(strerror(errno)) +
					")");
		}

		// XXX FIXME: Maybe a check on closedir() return value ...
		closedir(tmp);

		return true;
	}

	void mkdir(const std::string & s)
	{
		BUG_ON(s.size() == 0);

		if (::mkdir(s.c_str(), 0700) != 0) {
			throw Exception("Cannot create "
					"'" + s + "' "
					"directory "
					"(" +
					std::string(strerror(errno)) +
					")");
		}
	}

	void rmdir(const std::string & s)
	{
		BUG_ON(s.size() == 0);

		if (::rmdir(s.c_str()) != 0) {
			throw Exception("Cannot remove "
					"'" + s + "' "
					"directory"
					"(" +
					std::string(strerror(errno)) +
					")");
		}
	}
};
