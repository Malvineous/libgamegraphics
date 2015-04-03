check_PROGRAMS = tests

tests_SOURCES = tests.cpp
tests_SOURCES += test-archive.cpp
tests_SOURCES += test-filter.cpp
tests_SOURCES += test-filter-ccomic.cpp
tests_SOURCES += test-filter-ccomic2.cpp
tests_SOURCES += test-filter-pad.cpp
tests_SOURCES += test-image.cpp
tests_SOURCES += test-img-bash-sprite.cpp
tests_SOURCES += test-img-ccomic.cpp
tests_SOURCES += test-img-ega-planar.cpp
tests_SOURCES += test-img-ega-byteplanar.cpp
tests_SOURCES += test-img-ega-rowplanar.cpp
tests_SOURCES += test-img-nukem2.cpp
tests_SOURCES += test-img-pcx-1b4p.cpp
tests_SOURCES += test-img-pcx-8b1p.cpp
tests_SOURCES += test-img-pic-raptor.cpp
tests_SOURCES += test-img-vga.cpp
tests_SOURCES += test-img-zone66_tile.cpp
tests_SOURCES += test-pal-vga-raw.cpp
tests_SOURCES += test-pal-defaults.cpp
tests_SOURCES += test-subimage.cpp
tests_SOURCES += test-tileset.cpp
tests_SOURCES += test-tls-bash-sprite.cpp
tests_SOURCES += test-tls-ccaves-sub.cpp
tests_SOURCES += test-tls-ddave.cpp
tests_SOURCES += test-tls-harry-hsb.cpp
tests_SOURCES += test-tls-harry-ico.cpp
tests_SOURCES += test-tls-vinyl.cpp
tests_SOURCES += test-tls-zone66.cpp

EXTRA_tests_SOURCES = tests.hpp
EXTRA_tests_SOURCES += test-filter.hpp
EXTRA_tests_SOURCES += test-image.hpp
EXTRA_tests_SOURCES += test-tileset.hpp

TESTS = tests

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include $(libgamecommon_CFLAGS)
AM_LDFLAGS = $(BOOST_SYSTEM_LIBS) $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) $(libgamecommon_LIBS) $(top_builddir)/src/libgamegraphics.la
