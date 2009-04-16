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

#ifndef LIBS_MISC_TRACES_H
#define LIBS_MISC_TRACES_H

#include <cstdio>

//
// NOTE:
//     This tracing interface is really awful. It will be replaced ASAP.
//

extern int          trace_level;
extern const char * trace_prefix;

// Trace levels
#define TR_LVL_CRITICAL 5
#define TR_LVL_ERROR    4
#define TR_LVL_WARNING  3
#define TR_LVL_NOTICE   2
#define TR_LVL_VERBOSE  1
#define TR_LVL_DEBUG    0

#define TR_LVL_DEFAULT TR_LVL_NOTICE

#define _TRACE(LVL,FMT,ARGS...) {                                       \
	if ((LVL) >= trace_level) {                                     \
		FILE * s;	                                        \
                                                                        \
		if (trace_level >= TR_LVL_ERROR) {                      \
			s = stderr;                                     \
		} else {                                                \
			s = stdout;                                     \
		}                                                       \
                                                                        \
		if (trace_prefix) {                                     \
			fprintf(s, "%s: " FMT, trace_prefix, ##ARGS);   \
		} else {                                                \
			fprintf(s, FMT, ##ARGS);                        \
		}                                                       \
	}                                                               \
}

// Shortcuts for configuration
#define TR_CONFIG_LVL(LVL)  { trace_level  = LVL; }
#define TR_CONFIG_PFX(PFX)  { trace_prefix = PFX; }

// Shortcuts for traces
#define TR_DBG(FMT,ARGS...) _TRACE(TR_LVL_DEBUG,    FMT, ##ARGS);
#define TR_VRB(FMT,ARGS...) _TRACE(TR_LVL_VERBOSE,  FMT, ##ARGS);
#define TR_WRN(FMT,ARGS...) _TRACE(TR_LVL_WARNING,  FMT, ##ARGS);
#define TR_ERR(FMT,ARGS...) _TRACE(TR_LVL_ERROR,    FMT, ##ARGS);
#define TR_CRT(FMT,ARGS...) _TRACE(TR_LVL_CRITICAL, FMT, ##ARGS);

#endif // LIBS_MISC_TRACES_H
