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

#ifndef LIBS_FS_DIRECTORY_H
#define LIBS_FS_DIRECTORY_H

#include "config.h"

#include <string>

namespace FS {
        class Directory {
        public:
                Directory(const std::string & name);
                ~Directory(void);

                const std::string & name(void)                     const;
                void                create(void)                   const;
                void                remove(bool recursive = false) const;
                bool                exists(void)                   const;

        protected:
                Directory(void);

        private:
                std::string name_;
        };
}

#endif // LIBS_FS_DIRECTORY_H
