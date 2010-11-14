bin_PROGRAMS = gamegfx gameimg

gamegfx_SOURCES = gamegfx.cpp
EXTRA_gamegfx_SOURCES = common.hpp

gameimg_SOURCES = gameimg.cpp
EXTRA_gameimg_SOURCES = common.hpp

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include $(libgamecommon_CFLAGS)
AM_LDFLAGS = $(BOOST_SYSTEM_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS)
AM_LDFLAGS += $(libgamecommon_LIBS)
AM_LDFLAGS += $(libpng_LIBS)
AM_LDFLAGS += $(top_builddir)/src/libgamegraphics.la
