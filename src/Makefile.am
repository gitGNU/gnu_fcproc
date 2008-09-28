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
	fcp.cxx					\
	transformation.h			\
	transformation.cxx
fcp_CPPFLAGS =					\
	$(EXTRA_CPPFLAGS)
fcp_LDADD =					\
	libmisc.a				\
	libconf.a				\
	libfile.a				\
	libgraph.a

## Libraries

noinst_LIBRARIES =				\
	libmisc.a				\
	libconf.a				\
	libfile.a				\
	libgraph.a

libmisc_a_SOURCES =				\
	libs/misc/debug.h			\
	libs/misc/debug.cxx			\
	libs/misc/trace.h			\
	libs/misc/trace.cxx			\
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

libfile_a_SOURCES =				\
	libs/file/utils.h			\
	libs/file/utils.cxx
libfile_a_CPPFLAGS =				\
	$(EXTRA_CPPFLAGS)

libgraph_a_SOURCES =				\
	libs/graph/rule.h			\
	libs/graph/rule.cxx			\
	libs/graph/node.h			\
	libs/graph/node.cxx			\
	libs/graph/tree.h			\
	libs/graph/tree.cxx			\
	libs/graph/dag.h			\
	libs/graph/dag.cxx
libgraph_a_CPPFLAGS =				\
	$(EXTRA_CPPFLAGS)

##
## Maintainer related targets
##
update-local: Makefile

maintainer-check-local: Makefile
