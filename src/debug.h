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

#ifndef FCP_DEBUG_H
#define FCP_DEBUG_H

#include "config.h"

#include <cassert>
#include <cstdlib>

#include "trace.h"

void backtrace_dump(void);

#define BACKTRACE() {                                           \
        backtrace_dump();                                       \
        TR_CRT("\n");                                           \
        TR_CRT("Please report to <%s>\n", PACKAGE_BUGREPORT);   \
        abort();                                                \
}

#define BUG() {                                                 \
        TR_CRT("Bug hit in '%s' (%s:%d)\n",                     \
               __PRETTY_FUNCTION__, __FILE__, __LINE__);        \
        TR_CRT("\n");                                           \
        BACKTRACE();                                            \
}

#define BUG_ON(X) {                                                     \
        if (X) {                                                        \
                TR_CRT("Unsatisfied condition '%s' in '%s' (%s:%d)\n",  \
                       #X, __PRETTY_FUNCTION__, __FILE__, __LINE__);    \
                TR_CRT("\n");                                           \
                BACKTRACE();                                            \
        }                                                               \
}

#define MISSING_CODE() {                                        \
        TR_DBG("Missing code in '%s' (%s:%d)\n",                \
               __PRETTY_FUNCTION__, __FILE__, __LINE__);        \
}

#endif
