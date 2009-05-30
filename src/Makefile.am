##
## Input file for automake
##
## Copyright (C) 2008, 2009 Francesco Salvestrini
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

EXTRA_DIST =

## Programs

bin_PROGRAMS =					\
	fcp

bin_SCRIPTS =					\
	fcp2dot

CLEANFILES = $(bin_SCRIPTS)

fcp2dot: $(srcdir)/fcp2dot.in Makefile
	( rm -f $@ $@.tmp && \
	  $(do_subst) < $(srcdir)/fcp2dot.in > $@.tmp && \
	  chmod +x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
		rm -f $@ $@.tmp ; \
		exit 1 ; \
	}
EXTRA_DIST += fcp2dot.in

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

maintainer-check-parameters: Makefile $(bin_SCRIPTS) $(bin_PROGRAMS)
	for i in $(bin_SCRIPTS) $(bin_PROGRAMS) ; do \
		echo @ECHO_N@ "Checking standard parameters for $$i... " ; \
		if ( ./$$i -h 2>&1 > /dev/null ) ; then \
			echo "@ECHO_T@ok" ; \
		else \
			echo "@ECHO_T@error" ; \
			exit 1 ; \
		fi ; \
	done

maintainer-check-local: Makefile maintainer-check-parameters
	$(WHITESPACE_DETECT)  $(all_SRCS)
