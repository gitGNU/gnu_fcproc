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

#include <string>
#include <cstdlib>

#include "libs/misc/debug.h"
#include "libs/misc/environment.h"

namespace Environment {
        std::string get(const std::string & key)
        {
                BUG_ON(key.size() == 0);

                std::string tmp;

                tmp = getenv(key.c_str());

                return tmp;
        }

        std::string get(const char * key)
        {
                BUG_ON(key == 0);

                return get(std::string(key));
        }

        bool set(const std::string & key,
                 const std::string & value)
        {
                BUG_ON(key.size() == 0);

                if (setenv(key.c_str(), value.c_str(), 1) != 0) {
                        return false;
                }

                return true;
        }
}
