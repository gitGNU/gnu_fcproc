//
// Copyright (C) 2008 Francesco Salvestrini
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

#ifndef CONF_WRITER_H
#define CONF_WRITER_H

#include "config.h"

#include <string>
#include <fstream>

#include "conf/base.h"

namespace Configuration {
	template <typename K, typename V> class Writer : public Base<K, V> {
	public:
		Writer(const std::string & filename) {
			stream_.open(filename.c_str());
			if (stream_.is_open()) {
				// XXX FIXME: Throw an exception here ...
			}
		};
		~Writer(void) {
			if (stream_.is_open()) {
				stream_.close();
			}
		};

		virtual std::istream & operator <<(std::istream & is) = 0;

	protected:

	private:
		std::ofstream stream_;

	};
};

#endif // CONF_WRITER_H
