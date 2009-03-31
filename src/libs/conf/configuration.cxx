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

#include "configuration.h"

#include "libs/misc/string.h"
#include "libs/conf/configuration.h"

namespace Configuration {
	File::File(std::string delimiter,
		   std::string comment,
		   std::string whites) :
		delimiter_(delimiter),
		comment_(comment),
		whites_(whites)
	{
	}

	void File::remove(const std::string & key)
	{
		tuples_.erase(tuples_.find(key));
	}

	bool File::exists(const std::string & key) const
	{
		return (tuples_.find(key) != tuples_.end());
	}

	std::ostream & operator <<(std::ostream & os, const File & cf)
	{
		std::map<std::string, std::string>::const_iterator p;

		for (p = cf.tuples_.begin(); p != cf.tuples_.end(); p++) {
			os << p->first
			   << " " << cf.delimiter_ << " "
			   << p->second
			   << std::endl;
		}

		return os;
	}

	std::istream & operator >>(std::istream & is, File & cf)
	{
		std::string::size_type skip = cf.delimiter_.length();

		while (is) {
			std::string line;

			std::getline(is, line);

			// Remove comments
			line = line.substr(0, line.find(cf.comment_));
			line = String::trim_both(line, cf.whites_);
			if (line == "") {
				// Empty line
				continue;
			}

			// Look for delimiter
			std::string::size_type delimiter_pos;
			delimiter_pos = line.find(cf.delimiter_);
			if (delimiter_pos >= std::string::npos) {
				// XXX FIXME: Error, no delimiter found
				continue;
			}

			// Grab the key
			std::string key;
			key = line.substr(0, delimiter_pos);
			line.replace(0, delimiter_pos + skip, "");

			// Grab its value
			std::string value;
			value = line;

			key   = String::trim_right(key, cf.whites_);
			if (key == "") {
				// XXX FIXME: Error, empty key
				continue;
			}

			value = String::trim_left(value, cf.whites_);
			if (value == "") {
				// XXX FIXME: Error, empty value
				continue;
			}

			cf.tuples_[key] = value;
		}

		return is;
	}
}
