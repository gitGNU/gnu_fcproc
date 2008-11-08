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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "libs/fs/file.h"

namespace FS {
	File::File(const std::string & name) :
		name_(name)
	{
		TR_DBG("File '%s':\n", name_.c_str());

		if (name_ == "") {
			throw Exception("Missing file name");
		}
		// name cannot be empty
		BUG_ON(name_ == "");
		TR_DBG("  name      = '%s'\n", name_.c_str());

		std::string::size_type p;

		p = name_.rfind("/");
		dirname_ = ((p != std::string::npos) ?
			    name_.substr(0, p + 1) : "");
		// dirname_ could be empty
		TR_DBG("  dirname   = '%s'\n", dirname_.c_str());

		p         = name_.rfind("/");
		basename_ = ((p != std::string::npos) ?
			     name_.substr(p + 1) : name_);
		if (basename_ == "") {
			throw Exception("Malformed filename "
					"'" + name_ + "'");
		}
		// basename_ cannot be empty
		BUG_ON(basename_ == "");
		TR_DBG("  basename  = '%s'\n", basename_.c_str());

		p = basename_.rfind(".");
		if (p == 0) {
			// No extension, file is ".something"
			extension_ = "";
		} else if (p == std::string::npos) {
			// No extension
			extension_ = "";
		} else {
			// We got it
			extension_ = basename_.substr(p + 1);
		}
		TR_DBG("  extension = '%s'\n", extension_.c_str());
	}

	File::~File(void)
	{
	}

	const std::string & File::name(void) const
	{
		return name_;
	}

	const std::string & File::dirname(void) const
	{
		return dirname_;
	}

	const std::string & File::basename(void) const
	{
		return basename_;
	}

	const std::string & File::extension(void) const
	{
		return extension_;
	}

	time_t File::mtime(void) const
	{
		struct stat t;

		if (stat(name_.c_str(), &t) != 0) {
			throw Exception("Cannot stat() file "
					"'" + name_ + "' "
					"(" +
					std::string(strerror(errno)) +
					")");
		}

		return t.st_mtime;
	}

	// XXX FIXME: Ugly
	bool File::exists(void) const
	{
		struct stat t;

		if (stat(name_.c_str(), &t) == 0) {
			return true;
		} else {
			return false;
		}
	}
};
