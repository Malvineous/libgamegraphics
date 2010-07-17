lib_LTLIBRARIES = libgamegraphics.la

libgamegraphics_la_SOURCES = main.cpp
libgamegraphics_la_SOURCES += graphics.cpp
libgamegraphics_la_SOURCES += conv-byteplanar.cpp
libgamegraphics_la_SOURCES += img-ccaves.cpp

EXTRA_libgamegraphics_la_SOURCES = conv-byteplanar.hpp
EXTRA_libgamegraphics_la_SOURCES += img-ccaves.hpp

AM_CPPFLAGS = $(DEBUG_CPPFLAGS) $(BOOST_CPPFLAGS) $(libgamecommon_CFLAGS) -I $(top_srcdir)/include
AM_CXXFLAGS = $(DEBUG_CXXFLAGS)

libgamegraphics_la_LDFLAGS = $(AM_LDFLAGS) -release @VERSION@ -version-info 0
libgamegraphics_la_LIBADD = $(BOOST_SYSTEM_LIBS) $(libgamecommon_LIBS)
