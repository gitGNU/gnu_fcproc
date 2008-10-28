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
#include "libs/file/utils.h"
#include "libs/misc/exception.h"
#include "file.h"

namespace FCP {
	File::File(const std::string & name) :
		name_(name)
	{
		if (name_ == "") {
			throw Exception("Missing file name");
		}

		dirname_   = ::File::dirname(name_);
		basename_  = ::File::basename(name_);

		if (basename_ == "") {
			throw Exception("Malformed filename");
		}

		extension_ = ::File::extension(name_);

		if (extension_ == "") {
			throw Exception("Missing extension in "
					"filename '" + name_ + "'");
		}
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
		return ::File::mtime(name_);
	}

	bool File::operator ==(const FCP::File & rhs) const
	{
		return ((*this).name_ == rhs.name_);
	}
};
