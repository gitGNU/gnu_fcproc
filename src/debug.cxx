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

#include "config.h"

//
// XXX FIXME:
//     This implementation is ugly (at least). We must move this functionality
//     to a proper place while reimplementing in another way
//

#if defined(HAVE_EXECINFO_H) && \
        defined(HAVE_BACKTRACE) && \
        defined(HAVE_BACKTRACE_SYMBOLS)
#define BACKTRACE_ENABLED
#else
#undef BACKTRACE_ENABLED
#endif

#include "trace.h"

#ifdef  BACKTRACE_ENABLED
#include <cstdlib>
#include <execinfo.h>

#define BACKTRACE_SIZE 1024

static void * buffer[BACKTRACE_SIZE];

void backtrace_dump(void)
{
        int     nptrs;
        char ** strings;

        nptrs   = backtrace(buffer, BACKTRACE_SIZE);
        strings = backtrace_symbols(buffer, nptrs);
        if (!strings) {
                TR_CRT("No backtrace symbols available\n");
                return;
        }

        int j;

        TR_CRT("Backtrace:\n");
        TR_CRT("\n");
        for (j = 0; j < nptrs; j++) {
                TR_ERR("  %s\n", strings[j]);
        }

        free(strings);
}
#else
void backtrace_dump(void)
{ TR_CRT("No backtrace dump available\n"); }
#endif
