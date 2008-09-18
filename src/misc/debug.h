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

#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"

#include <cassert>
#include <cstdlib>

#include "misc/trace.h"

extern void backtrace_dump(void);

#define ABORT() {				\
	backtrace_dump();			\
	abort();				\
}

#define BUG() {							\
	TR_CRT("Bug hit in '%s' (%s:%d)\n",			\
	       __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
	TR_CRT("\n");						\
	ABORT();						\
}

#define BUG_ON(X) {							\
	if (X) {							\
		TR_CRT("Unsatisfied condition '%s' in '%s' (%s:%d)\n",	\
		       #X, __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
		TR_CRT("\n");						\
		ABORT();						\
	}								\
}

#define MISSING_CODE() {					\
	TR_DBG("Missing code in '%s' (%s:%d)\n",		\
	       __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
}

#endif // DEBUG_H
