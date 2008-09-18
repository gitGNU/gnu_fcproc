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

#ifndef TRACES_H
#define TRACES_H

#include <cstdio>

#define MODULE "test"

extern int trace_level;

// Trace levels
#define TR_LVL_CRITICAL 4
#define TR_LVL_ERROR    3
#define TR_LVL_WARNING  2
#define TR_LVL_NOTICE   1
#define TR_LVL_DEBUG    0

#define TR_LVL_DEFAULT TR_LVL_NOTICE

#define TRACE(LVL,FMT,ARGS...) {				\
	if ((LVL) >= trace_level) {				\
		fprintf(stdout, MODULE ": " FMT, ##ARGS);	\
	}							\
}

// Shortcuts for traces
#define TR_DBG(FMT,ARGS...) TRACE(TR_LVL_DEBUG,    FMT, ##ARGS);
#define TR_WRN(FMT,ARGS...) TRACE(TR_LVL_WARNING,  FMT, ##ARGS);
#define TR_ERR(FMT,ARGS...) TRACE(TR_LVL_ERROR,    FMT, ##ARGS);
#define TR_CRT(FMT,ARGS...) TRACE(TR_LVL_CRITICAL, FMT, ##ARGS);

#endif // TRACES_H
