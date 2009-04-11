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

#include <cstring>
#include <string>
#include <cerrno>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "libs/fs/file.h"

namespace FS {
	File::File(const std::string & name) :
		name_(name)
	{
		// TR_DBG("File '%s':\n", name_.c_str());

		if (name_ == "") {
			throw Exception("Missing file name");
		}
		// name cannot be empty
		BUG_ON(name_ == "");
		// TR_DBG("  name      = '%s'\n", name_.c_str());

		std::string::size_type p;

		p = name_.rfind("/");

		dirname_ = ((p != std::string::npos) ?
			    name_.substr(0, p - 1) : "");
		// dirname_ could be empty
		// TR_DBG("  dirname   = '%s'\n", dirname_.c_str());

		std::string tmp;
		tmp = ((p != std::string::npos) ?
		       name_.substr(p + 1) : name_);
		if (tmp == "") {
			throw Exception("Malformed filename "
					"'" + name_ + "'");
		}
		// basename cannot be empty
		BUG_ON(tmp == "");
		// TR_DBG("  basename  = '%s'\n", basename_.c_str());

		std::string::size_type q;
		q = tmp.rfind(".");

		basename_.prefix = tmp.substr(0, q);
		if (q == 0) {
			// No extension, file is ".something"
			basename_.suffix = "";
		} else if (q == std::string::npos) {
			// No extension
			basename_.suffix = "";
		} else {
			// We got it
			basename_.suffix = tmp.substr(q + 1);
		}
		// TR_DBG("  extension = '%s'\n", extension_.c_str());
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

	std::string File::basename(bool strip_suffix) const
	{
		std::string ret;

		ret = basename_.prefix;
		if (strip_suffix) {
			return ret;
		}

		if (basename_.suffix.size() != 0) {
			ret = ret + "." + basename_.suffix;
		}

		return ret;
	}

	const std::string & File::extension(void) const
	{
		return basename_.suffix;
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

	void File::remove(void) const
	{
		TR_DBG("Removing file '%s'\n", name_.c_str());

		if (::unlink(name_.c_str()) != 0) {
			throw Exception("Cannot remove "
					"'" + name_ + "' "
					"file "
					"(" +
					std::string(strerror(errno)) +
					")");
		}
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
