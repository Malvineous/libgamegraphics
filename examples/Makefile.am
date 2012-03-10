bin_PROGRAMS = gamegfx gameimg

gamegfx_SOURCES = gamegfx.cpp
EXTRA_gamegfx_SOURCES = common.hpp

gameimg_SOURCES = gameimg.cpp
EXTRA_gameimg_SOURCES = common.hpp

WARNINGS = -Wall -Wextra -Wno-unused-parameter

AM_CPPFLAGS  = $(BOOST_CPPFLAGS)
AM_CPPFLAGS += -I $(top_srcdir)/include
AM_CPPFLAGS += $(libpng_CPPFLAGS)
AM_CPPFLAGS += $(libgamecommon_CPPFLAGS)
AM_CPPFLAGS += $(WARNINGS)

AM_CXXFLAGS  = $(DEBUG_CXXFLAGS)
AM_CXXFLAGS += $(libpng_CFLAGS)
AM_CXXFLAGS += $(libgamecommon_CFLAGS)

AM_LDFLAGS  = $(BOOST_SYSTEM_LIBS)
AM_LDFLAGS += $(BOOST_PROGRAM_OPTIONS_LIBS)
AM_LDFLAGS += $(libpng_LIBS)
AM_LDFLAGS += $(libgamecommon_LIBS)
AM_LDFLAGS += $(top_builddir)/src/libgamegraphics.la
