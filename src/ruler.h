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

#ifndef FCP_RULER_H
#define FCP_RULER_H

#include "config.h"

#include <cstdlib>
#include <ostream>

namespace fcp {

        class ruler {
        public:
                ruler(size_t length); // length is in chars
                ~ruler();

                void set(size_t current = 0); // current is 0 .. 100

                void show(std::ostream & stream);
                void update(std::ostream & stream);

        protected:

        private:
                size_t status_;
                size_t length_;
        };

}

#endif
