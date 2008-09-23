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

#ifndef CONF_BASE_H
#define CONF_BASE_H

#include "config.h"

#include <string>
#include <map>
#include <fstream>

#include "misc/debug.h"

namespace Configuration {
	template <typename K, typename V> class Entry {
	public:
		Entry(void);
		virtual ~Entry(void);

		std::string key(void) {
			return key_;
		}
		void        key(const K & k) {
			key_ = k;
		}
		std::string value(void) {
			return value_;
		}
		void        value(const V & v) {
			value_ = v;
		}

		virtual std::istream & operator <<(std::istream & is) = 0;
		virtual std::ostream & operator >>(std::ostream & os) = 0;

	protected:

	private:
		K key_;
		V value_;
	};

	template <typename K, typename V> class Base {
	public:
		Base(void)  {
			entries_.clear();
		};
		~Base(void) {
			typename std::map<K, Entry<K, V > * >::iterator iter;

			for (iter  = entries_.begin();
			     iter != entries_.end();
			     iter++) {
				BUG_ON((*iter).second == 0);
				delete (*iter).second;
			}
		};

	private:

	protected:
		std::map<K, Entry<K, V> * > entries_;
	};
};

#endif // CONF_BASE_H
