bin_PROGRAMS = gamegfx

gamegfx_SOURCES = gamegfx.cpp

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include $(libgamecommon_CFLAGS)
AM_LDFLAGS = $(BOOST_SYSTEM_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS)
AM_LDFLAGS += $(libgamecommon_LIBS)
AM_LDFLAGS += $(libpng_LIBS)
AM_LDFLAGS += $(top_builddir)/src/libgamegraphics.la
