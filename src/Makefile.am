##
## Input file for automake
##
## Copyright (C) 2007, 2008 Francesco Salvestrini
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##

include $(top_srcdir)/Makefile.sub

AUTOMAKE_OPTIONS = subdir-objects

## Programs

bin_PROGRAMS =					\
	fcp

fcp_SOURCES =					\
	main.cxx				\
	log.cxx					\
	log.h					\
	graph.cxx				\
	graph.h					\
	job.cxx					\
	job.h					\
	parser.yxx				\
	scanner.lxx
fcp_CPPFLAGS =					\
	$(EXTRA_CPPFLAGS)
fcp_LDADD =					\
	libmisc.a

AM_LFLAGS = $(LEX_FLAGS)
AM_YFLAGS = $(YACC_FLAGS) -d

BUILT_SOURCES =					\
	parser.h

## Libraries

noinst_LIBRARIES =				\
	libmisc.a

libmisc_a_SOURCES =                             \
        misc/debug.h                            \
        misc/debug.cxx                          \
        misc/trace.h                            \
        misc/trace.cxx                          \
        misc/constant.h                         \
        misc/utility.h                          \
        misc/thread.h                           \
        misc/thread.cxx
libmisc_a_CPPFLAGS =                            \
        $(EXTRA_CPPFLAGS)

##
## Maintainer related targets
##
update-local: Makefile

maintainer-check-local: Makefile
