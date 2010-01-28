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

#ifndef FCP_TRACE_H
#define FCP_TRACE_H

#include <cstdio>

//
// NOTE:
//     This tracing interface is really awful. It will be replaced ASAP.
//

//
// XXX FIXME: This implementation is awful, please rewrite it ASAP
//

// Trace levels
#define TR_LVL_CRITICAL 0x01
#define TR_LVL_ERROR    0x02
#define TR_LVL_WARNING  0x04
#define TR_LVL_NOTICE   0x08
#define TR_LVL_VERBOSE  0x10
#define TR_LVL_DEBUG    0x20

extern int trace_mask;

#define _TRACE(LVL,FMT,ARGS...) {                                       \
        if (trace_mask & LVL) {                                         \
                FILE * s;                                               \
                                                                        \
                if (trace_mask & (TR_LVL_ERROR | TR_LVL_CRITICAL)) {    \
                        s = stderr;                                     \
                } else {                                                \
                        s = stdout;                                     \
                }                                                       \
                                                                        \
                fprintf(s, "%s: " FMT, PACKAGE_TARNAME, ##ARGS);        \
        }                                                               \
}

// Shortcuts for configuration
#define TR_CONFIG_LVL(LVL, VALUE) {                     \
	trace_mask |= ((VALUE == true) ? LVL : 0);      \
}

// Shortcuts for traces
#define TR_DBG(FMT,ARGS...) _TRACE(TR_LVL_DEBUG,    FMT, ##ARGS)
#define TR_VRB(FMT,ARGS...) _TRACE(TR_LVL_VERBOSE,  FMT, ##ARGS)
#define TR_WRN(FMT,ARGS...) _TRACE(TR_LVL_WARNING,  FMT, ##ARGS)
#define TR_ERR(FMT,ARGS...) _TRACE(TR_LVL_ERROR,    FMT, ##ARGS)
#define TR_CRT(FMT,ARGS...) _TRACE(TR_LVL_CRITICAL, FMT, ##ARGS)

#endif
