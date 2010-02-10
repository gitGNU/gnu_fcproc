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

SUBDIRS    =
EXTRA_DIST =

## Programs

bin_PROGRAMS =					\
	fcp
bin_SCRIPTS  =

COMMON_CPPFLAGS =				\
	$(BOOST_CPPFLAGS)

fcp_SOURCES =					\
	fcp.cxx					\
	namespace.h				\
	tag.h					\
	tag.cxx					\
	transformation.h			\
	transformation.cxx			\
	chain.h					\
	chain.cxx				\
	rules.h					\
	rules.cxx				\
	filter.h				\
	filter.cxx				\
	command.h				\
	command.cxx				\
	file.h					\
	file.cxx				\
	exception.h				\
	exception.cxx				\
	debug.h					\
	debug.cxx				\
	trace.h					\
	trace.cxx				\
	utility.h				\
	utility.cxx
fcp_CPPFLAGS =					\
	$(COMMON_CPPFLAGS)
fcp_LDADD =					\
        $(BOOST_LDFLAGS)			\
        $(BOOST_SYSTEM_LIB)			\
        $(BOOST_THREAD_LIB)			\
        $(BOOST_FILESYSTEM_LIB)			\
        $(BOOST_REGEX_LIB)			\
        $(BOOST_PROGRAM_OPTIONS_LIB)

##
## Maintainer related targets
##
update-local: Makefile

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

all_SRCS =					\
	$(fcp_SOURCES)

maintainer-check-whitespaces: Makefile $(all_SRCS)
	$(WHITESPACE_DETECT) $(all_SRCS)

maintainer-check-wordwraps: Makefile $(all_SRCS)
	$(WORDWRAP_DETECT) --tab-size 8 --columns 80 $(all_SRCS)

maintainer-check-guards:  Makefile $(all_SRCS)
	$(GUARDS_CHECK) $(all_SRCS)

maintainer-check-local: Makefile		\
	maintainer-check-guards			\
	maintainer-check-whitespaces		\
	maintainer-check-wordwraps		\
	maintainer-check-parameters
