//
// Copyright (C) 2007, 2008 Francesco Salvestrini
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

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include "log.h"

static FILE* vrb_stream = 0;

int vrb_printf(const char* format, ...)
{
	va_list args;
	int     i;

	i = 0;

	if (vrb_stream) {
		va_start(args, format);
		i = vfprintf(vrb_stream, format, args);
		va_end(args);
	}

	return i;
}

void vrb_config(FILE* stream)
{
	vrb_stream = stream;
}

static FILE* dbg_stream = 0;

int dbg_printf(const char* format, ...)
{
	va_list args;
	int     i;

	i = 0;

	if (dbg_stream) {
		va_start(args, format);
		i = vfprintf(dbg_stream, format, args);
		va_end(args);
	}

	return i;
}

void dbg_config(FILE* stream)
{
	dbg_stream = stream;
}
