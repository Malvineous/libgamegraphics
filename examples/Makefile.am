bin_PROGRAMS = gametls gameimg
noinst_PROGRAMS = hello

gametls_SOURCES = gametls.cpp
EXTRA_gametls_SOURCES  = common.hpp
EXTRA_gametls_SOURCES += pngutil.hpp

gameimg_SOURCES = gameimg.cpp
EXTRA_gameimg_SOURCES  = common.hpp
EXTRA_gameimg_SOURCES += pngutil.hpp

hello_SOURCES = hello.cpp

WARNINGS = -Wall -Wextra -Wno-unused-parameter

AM_CPPFLAGS  = -I $(top_srcdir)/include
AM_CPPFLAGS += $(BOOST_CPPFLAGS)
AM_CPPFLAGS += $(libpng_CPPFLAGS)
AM_CPPFLAGS += $(libgamecommon_CPPFLAGS)
AM_CPPFLAGS += $(libgamearchive_CPPFLAGS)
AM_CPPFLAGS += $(WARNINGS)

AM_CXXFLAGS  = $(DEBUG_CXXFLAGS)
AM_CXXFLAGS += $(libpng_CFLAGS)
AM_CXXFLAGS += $(libgamecommon_CFLAGS)
AM_CXXFLAGS += $(libgamearchive_CFLAGS)

AM_LDFLAGS  = $(top_builddir)/src/libgamegraphics.la
AM_LDFLAGS += $(BOOST_LDFLAGS)
AM_LDFLAGS += $(BOOST_SYSTEM_LIB)
AM_LDFLAGS += $(BOOST_PROGRAM_OPTIONS_LIB)
AM_LDFLAGS += $(libpng_LIBS)
AM_LDFLAGS += $(libgamecommon_LIBS)
AM_LDFLAGS += $(libgamearchive_LIBS)
