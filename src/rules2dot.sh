#@SHELL@

#
# rules2dot.sh
#
# Copyright (C) 2009 Francesco Salvestrini
#                    Alessandro Massignan
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


#
# Global variables
#

PROGNAME="rules2dot"
PROGVERSION="0.0.1"
DEBUG="0"
VERBOSE="0"
WARNING="1"


#
# Trace functions
#

error ()
{
    echo "$PROGNAME: $1" >&2
}

warning ()
{
    if test $WARNING = 1; then
	echo "$PROGNAME: $1" >&2
    fi
}

debug ()
{
    if test $DEBUG = 1; then
	echo "$PROGNAME: $1" >&2
    fi
}

verbose ()
{
    if test $VERBOSE = 1; then
	echo "$PROGNAME: $1"
    fi
}


#
# Miscellaneous functions
#

version ()
{
    echo "$PROGNAME (@PACKAGE_NAME@) $PROGVERSION"
}

hint ()
{
    echo "$PROGNAME: $1"
    echo "Try \`$PROGNAME -h' for more information."
}


#
# Script funcions
#

help() {
    echo "Usage: ${PROGNAME} [OPTION]"
    echo
    echo "Options:"
    echo "  -r, --rules=FILE       specify dumped rules file"
    echo "  -o, --output=FILE      specify output file"
    echo "  -v, --verbose          enable verbose messages"
    echo "  -d, --debug            enable debugging"
    echo "  -V, --version          print this help, then exit"
    echo "  -h, --help             print version number, then exit"
    echo
    echo "Report bugs to <@PACKAGE_BUGREPORT@>"
}


# Script variables
SED="@SED@"
RULES=""
OUTPUT=""


#
# Parsing command line arguments
#

if test x"${1}" = x""; then
  hint "Missing required options!"
  exit 1
fi

while test ! x"${1}" = x""
do
  case $1 in
    -r|--rules)      shift && RULES="$1"                      ;;
    --rules=*)       RULES="`echo ${1} | $SED 's/^[^=]=//'`"  ;;
    -o|--output)     shift && OUTPUT="$1"                     ;;
    --output=*)      OUTPUT="`echo ${1} | $SED 's/^[^=]=//'`" ;;
    -v|--verbose)    VERBOSE="1"                              ;;
    -d|--debug)      DEBUG="1"                                ;;
    -h|--help)       help && exit 0                           ;;
    -V|--version)    version && exit 0                        ;;
      *)              hint "Wrong option '${1}'!" && exit 1   ;;
  esac
  shift
done


#
# Checking command line arguments
#

verbose "Checking command line arguments"

if test "x${RULES}" = "x"; then
    hint "Missing dumped rules file"
    exit 1
fi

if test ! -r "${RULES}"; then
    error "Dumped rules file \`${RULES}' is not readable"
    exit 1
fi
debug "Dumped rules file is \`${RULES}'"

if test "x${OUTPUT}" = "x"; then
    hint "Missing output file"
    exit 1
fi

: >$OUTPUT || {
    error "Output file \`${OUTPUT}' is not writable"
    exit 1
}
debug "Output file is \`${OUTPUT}'"


#
# Start the real work
#

verbose "Processing rules"

$SED -n '
      i\
digraph rules {
      b a;
      :b {
      a\
}
      q;
      }
      :a {
	s/^\ *\([^\ ]\{1,\}\)[\ \t]\{1,\}.*[\ \t]\{1,\}/\1\ \-\>\ /;
	s/^/\ \ \ \ /;
	s/[\ \t]*$/\;/;
	p;
	$ {
	  b b;
	}
	n;
	b a;
      }
     ' <$RULES >>$OUTPUT || {
    error "Failed to process rules";
    exit 1;
}

verbose "All done."

exit 0
