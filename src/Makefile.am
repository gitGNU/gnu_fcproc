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

SUBDIRS = gnulib

## Programs

bin_PROGRAMS =					\
	fcp

fcp_SOURCES =					\
	fcp.cxx					\
	transformation.h			\
	transformation.cxx			\
	chain.h					\
	chain.cxx				\
	rules.h					\
	rules.cxx				\
	filter.h				\
	filter.cxx
fcp_CPPFLAGS =					\
	$(EXTRA_CPPFLAGS)
fcp_LDADD =					\
	libmisc.a				\
	libconf.a				\
	libfs.a					\
	gnulib/libgnu.a

## Libraries

noinst_LIBRARIES =				\
	libmisc.a				\
	libconf.a				\
	libfs.a

libmisc_a_SOURCES =				\
	libs/misc/stream.h			\
	libs/misc/stream.cxx			\
	libs/misc/exception.h			\
	libs/misc/exception.cxx			\
	libs/misc/debug.h			\
	libs/misc/debug.cxx			\
	libs/misc/trace.h			\
	libs/misc/trace.cxx			\
	libs/misc/string.h			\
	libs/misc/string.cxx			\
	libs/misc/environment.h			\
	libs/misc/environment.cxx		\
	libs/misc/constant.h			\
	libs/misc/utility.h			\
	libs/misc/thread.h			\
	libs/misc/thread.cxx
libmisc_a_CPPFLAGS =				\
	$(EXTRA_CPPFLAGS)

libconf_a_SOURCES =				\
	libs/conf/configuration.h		\
	libs/conf/configuration.cxx
libconf_a_CPPFLAGS =				\
	$(EXTRA_CPPFLAGS)

libfs_a_SOURCES =				\
	libs/fs/file.h				\
	libs/fs/file.cxx			\
	libs/fs/directory.h			\
	libs/fs/directory.cxx
libfs_a_CPPFLAGS =				\
	$(EXTRA_CPPFLAGS)

##
## Maintainer related targets
##
update-local: Makefile

all_SRCS =					\
	$(fcp_SOURCES)				\
	$(libmisc_a_SOURCES)			\
	$(libconf_a_SOURCES)			\
	$(libfs_a_SOURCES)

maintainer-check-local: Makefile
	$(WHITESPACE_DETECT)  $(all_SRCS)
