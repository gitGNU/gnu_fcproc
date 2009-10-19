// -*- c++ -*-

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

#ifndef LIBS_MISC_STRING_H
#define LIBS_MISC_STRING_H

#include "config.h"

#include <string>
#include <sstream>

namespace String {
        std::string trim_left(const std::string & s,
                              const std::string & t);
        std::string trim_right(const std::string & s,
                               const std::string & t);
        std::string trim_both(const std::string & s,
                              const std::string & t);
        std::string replace(std::string &       in,
                            const std::string & from,
                            const std::string & to);
        std::string itos(int value);

        void        slice(const std::string & in,
                          char                separator,
                          std::string &       out1,
                          std::string &       out2);
};

#endif // LIBS_MISC_STRING_H
