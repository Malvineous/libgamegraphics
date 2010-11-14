lib_LTLIBRARIES = libgamegraphics.la

libgamegraphics_la_SOURCES = main.cpp
libgamegraphics_la_SOURCES += tileset.cpp
libgamegraphics_la_SOURCES += image.cpp
libgamegraphics_la_SOURCES += palettetable.cpp
libgamegraphics_la_SOURCES += img-ega-byteplanar.cpp
libgamegraphics_la_SOURCES += img-vga.cpp
libgamegraphics_la_SOURCES += img-vga-raw.cpp
libgamegraphics_la_SOURCES += img-palette.cpp
libgamegraphics_la_SOURCES += pal-vga-raw.cpp
libgamegraphics_la_SOURCES += tileset-fat.cpp
libgamegraphics_la_SOURCES += tls-ccaves.cpp

EXTRA_libgamegraphics_la_SOURCES = img-ega-byteplanar.hpp
EXTRA_libgamegraphics_la_SOURCES += img-vga.hpp
EXTRA_libgamegraphics_la_SOURCES += img-vga-raw.hpp
EXTRA_libgamegraphics_la_SOURCES += img-palette.hpp
EXTRA_libgamegraphics_la_SOURCES += pal-vga-raw.hpp
EXTRA_libgamegraphics_la_SOURCES += tileset-fat.hpp
EXTRA_libgamegraphics_la_SOURCES += tls-ccaves.hpp

AM_CPPFLAGS = $(DEBUG_CPPFLAGS) $(BOOST_CPPFLAGS) -I $(top_srcdir)/include $(libgamecommon_CFLAGS)
AM_CXXFLAGS = $(DEBUG_CXXFLAGS)

libgamegraphics_la_LDFLAGS = $(AM_LDFLAGS) -release @VERSION@ -version-info 0
libgamegraphics_la_LIBADD = $(BOOST_SYSTEM_LIBS) $(libgamecommon_LIBS)
