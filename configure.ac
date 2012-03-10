#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libgamegraphics], [0.1], [camoto@lists.shikadi.net])
AM_INIT_AUTOMAKE([libgamegraphics], [0.1])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG(C++)

# Overarching version of all libraries (used for installation dir)
AC_SUBST(camoto_release, camoto-1.0)

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([libgamecommon], [libgamecommon])
PKG_CHECK_MODULES([libpng], [libpng])

SAVE_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $libpng_CFLAGS"

# Checks for header files.
AC_CACHE_CHECK([for png++],
	[ac_cv_pngpp],
	[AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM([#include <png++/png.hpp>], [return 0;])],
		[ac_cv_pngpp=yes],
		[AC_MSG_ERROR([<png++/png.hpp> not found or unusable.

Please download and install png++.  See config.log for failure messages if required.])
		]
	)]
)

AC_CACHE_CHECK([if png++ is new enough],
	[ac_cv_pngpp_ver],
	[AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM([#include <png++/png.hpp>], [std::string fn; png::image<png::index_pixel> png(fn); return 0;])],
		[ac_cv_pngpp_ver=yes],
		[AC_MSG_ERROR([png++ is too old.

Please download and install png++ newer than 0.2.5.])
		]
	)]
)

CPPFLAGS=$SAVE_CPPFLAGS

BOOST_REQUIRE([1.46])
BOOST_PROGRAM_OPTIONS
BOOST_TEST

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug],[enable extra debugging output]))

dnl Check for --enable-debug and add appropriate flags for gcc
if test "x$enable_debug" = "xyes";
then
	# Remove any optimisation flags
	CPPFLAGS=`echo "$CPPFLAGS" | sed -e 's/-O[0-9]*//g'`
	CXXFLAGS=`echo "$CXXFLAGS" | sed -e 's/-O[0-9]*//g'`
	CFLAGS=`echo "$CFLAGS" | sed -e 's/-O[0-9]*//g'`
	# Add gdb info (-g), disable any default optimisation
	DEBUG_CXXFLAGS="-O0 -g"
	#AM_CXXFLAGS="-O0 -g"
	# Add DEBUG define
	AC_DEFINE([DEBUG], [1], [Define to include extra debugging output])
fi

case "$host_os" in
*mingw*)
	# Must specify this parameter when compiling for Windows
	LDFLAGS="$LDFLAGS -no-undefined"
	;;
esac

AM_SILENT_RULES([yes])

AC_OUTPUT(Makefile src/Makefile include/Makefile include/camoto/Makefile examples/Makefile tests/Makefile doc/Makefile utils/Makefile $PACKAGE.pc)
