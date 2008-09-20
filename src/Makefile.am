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
	log.cxx					\
	log.h					\
	job.cxx					\
	job.h
fcp_CPPFLAGS =					\
	$(EXTRA_CPPFLAGS)
fcp_LDADD =					\
	libmisc.a				\
	libgraph.a

## Libraries

noinst_LIBRARIES =				\
	libmisc.a				\
	libgraph.a

libmisc_a_SOURCES =				\
	misc/debug.h				\
	misc/debug.cxx				\
	misc/trace.h				\
	misc/trace.cxx				\
	misc/environment.h			\
	misc/environment.cxx			\
	misc/constant.h				\
	misc/utility.h				\
	misc/thread.h				\
	misc/thread.cxx
libmisc_a_CPPFLAGS =				\
	$(EXTRA_CPPFLAGS)

libgraph_a_SOURCES =				\
	graph/rule.h				\
	graph/rule.cxx				\
	graph/node.h				\
	graph/node.cxx				\
	graph/tree.h				\
	graph/tree.cxx				\
	graph/dag.h				\
	graph/dag.cxx
libgraph_a_CPPFLAGS =				\
	$(EXTRA_CPPFLAGS)

##
## Maintainer related targets
##
update-local: Makefile

maintainer-check-local: Makefile
