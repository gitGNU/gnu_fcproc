//
// Copyright (C) 2008, 2009 Francesco Salvestrini
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

#include <string.h>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "libs/fs/directory.h"
#include "libs/fs/file.h"

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

		TR_DBG("Directory '%s' created successfully\n", name_.c_str());
	}

	void Directory::remove(bool recursive) const
	{
		TR_DBG("Removing directory '%s'\n", name_.c_str());
		if (recursive) {
			DIR * dir;

			dir = opendir(name_.c_str());
			if (!dir) {
				throw Exception("Cannot open "
						"'" + name_ + "' "
						"directory "
						"(" +
						std::string(strerror(errno)) +
						")");
			}

			struct dirent * entry;
			while ((entry = readdir(dir)) != 0) {
				std::string t(entry->d_name);

				switch (entry->d_type) {
					case DT_REG: {
						File f(name_ + "/" + t);
						f.remove();
						break;
					}
					case DT_DIR: {
						// Skip '.' and '..'
						if ((t == ".") || (t == "..")) {
							break;
						}

						Directory d(name_ + "/" + t);
						d.remove(recursive);
						break;
					}
					default:
						BUG();
						break;
				}
			}

			closedir(dir);
		}

		// XXX FIXME: Consider using the gnulib replacement
		if (::rmdir(name_.c_str()) != 0) {
			throw Exception("Cannot remove "
					"'" + name_ + "' "
					"directory "
					"(" +
					std::string(strerror(errno)) +
					")");
		}

		TR_DBG("Directory '%s' removed successfully\n", name_.c_str());
	}

	bool Directory::exists(void) const
	{
		// XXX FIXME: Consider using the gnulib replacement
		DIR * tmp = opendir(name_.c_str());
		if (tmp == 0) {
			BUG_ON(errno == EBADF);

			if (errno == ENOENT) {
				return false;
			}

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
