# Process this file with autoconf to produce a configure script.

#
# configure.ac for fcp
#
# Copyright (C) 2008, 2009 Francesco Salvestrini
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

AC_INIT([Filter Chains Processor],
        m4_esyscmd([tools/maint/git-version-gen .tarball-version]),
        [fcproc-generic@nongnu.org],
        [fcp],
        [http://savannah.nongnu.org/projects/fcproc])

# Check for a recent version of autoconf
AC_PREREQ([2.65])

# Place/find all autotools related files in the following directories
AC_CONFIG_AUX_DIR([tools/autotools])
AC_CONFIG_MACRO_DIR([tools/autotools/m4])
AC_CONFIG_SRCDIR([src/fcp.cxx])

#
# For automake options (AM_INIT_AUTOMAKE options) see Makefile.am
# file, AUTOMAKE_OPTIONS variable
#
AM_INIT_AUTOMAKE([1.11.1 foreign std-options -Wall color-tests])

#
# Autoheader support
#
AC_CONFIG_HEADERS([src/config.h])

#
# Build related programs
#
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_GREP
AC_PROG_SED

AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXX_C_O
AC_PROG_RANLIB

#
# gnulib related
#
#GNULIB_EARLY
#GNULIB_INIT

#
# BOOST related checks
#
AX_BOOST_BASE([1.41.0],[],[
  AC_MSG_ERROR([Missing Boost library >= 1.41.0])
])
AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_BOOST_FILESYSTEM
AX_BOOST_REGEX
AX_BOOST_PROGRAM_OPTIONS

# Checks for preprocessor flags
TEMP_FLAGS=""
AX_CXXCPP_CHECK_FLAG([-Wall],  [],[],[ TEMP_FLAGS="$TEMP_FLAGS -Wall"  ])
AX_CXXCPP_CHECK_FLAG([-Werror],[],[],[ TEMP_FLAGS="$TEMP_FLAGS -Werror"])
AC_SUBST([EXTRA_CPPFLAGS],$TEMP_FLAGS)


AX_PATH_MISSING([PERL],     [perl])
AX_PATH_MISSING([SITETOOL], [sitetool])
AX_PATH_MISSING([HELP2MAN], [help2man])
AX_PATH_MISSING([WGET],     [wget])
AX_PATH_MISSING([DNT],      [dnt])
AX_PATH_MISSING([SORT],     [sort])

# The amount we should wait after modifying files depends on the platform.
# On Windows '95, '98 and ME, files modifications have 2-seconds
# granularity and can be up to 3 seconds in the future w.r.t. the
# system clock.  When it is important to ensure one file is older
# than another we wait at least 5 seconds between creations.
case $build in
  *-pc-msdosdjgpp) MODIFICATION_DELAY=5;;
  *)               MODIFICATION_DELAY=2;;
esac
AC_SUBST([MODIFICATION_DELAY])

#
# Check for execinfo.h, backtrace() and backtrace_symbols(). They may be useful
#
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_FUNCS([backtrace backtrace_symbols])

AX_SPLIT_VERSION($PACKAGE_VERSION, AX_MAJOR_VERSION, AX_MINOR_VERSION, AX_MICRO_VERSION, AX_EXTRA_VERSION)
AX_PACKAGE_API([${AX_MAJOR_VERSION}"."${AX_MINOR_VERSION}])

AC_CONFIG_FILES([
	Makefile

	src/Makefile

	tools/Makefile
	tools/maint/Makefile
	tools/autotools/Makefile
	tools/autotools/m4/Makefile

	site/Makefile
	site/in/Makefile

	docs/Makefile
	docs/info/Makefile
	docs/man/Makefile

	tests/Makefile
])

AC_CONFIG_FILES([tools/maint/fetch],               [chmod +x tools/maint/fetch])
AC_CONFIG_FILES([tools/maint/wordwrap-detect],     [chmod +x tools/maint/wordwrap-detect])
AC_CONFIG_FILES([tools/maint/whitespace-detect],   [chmod +x tools/maint/whitespace-detect])
AC_CONFIG_FILES([tools/maint/gitlog-to-committers],[chmod +x tools/maint/gitlog-to-committers])
AC_CONFIG_FILES([tools/maint/gittag-to-date],      [chmod +x tools/maint/gittag-to-date])
AC_CONFIG_FILES([tools/maint/releases-to-news],    [chmod +x tools/maint/releases-to-news])
AC_CONFIG_FILES([tools/maint/guards-check],        [chmod +x tools/maint/guards-check])

## Look for programs and inject their rules
#FCP_RULE_SETUP([./rules])
#
#FCP_RULE([asciidoc],[ASCIIDOC],[
#txt:xhtml
#	ASCIIDOC -b=xhtml11 -o=$O $I
#
#txt:html
#	ASCIIDOC -b=html4 -o=$O $I
#
#])

AC_OUTPUT
