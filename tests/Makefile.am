check_PROGRAMS = tests

tests_SOURCES = tests.cpp
tests_SOURCES += test-img-ega-planar.cpp
tests_SOURCES += test-img-ega-byteplanar.cpp
tests_SOURCES += test-img-ega-rowplanar.cpp
tests_SOURCES += test-tls-ccaves-sub.cpp
tests_SOURCES += test-img-zone66_tile.cpp
tests_SOURCES += test-pal-vga-raw.cpp
tests_SOURCES += test-pal-defaults.cpp
tests_SOURCES += test-tls-zone66.cpp

EXTRA_tests_SOURCES = tests.hpp
EXTRA_tests_SOURCES += test-img.hpp
EXTRA_tests_SOURCES += test-tls.hpp

TESTS = tests

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include $(libgamecommon_CFLAGS)
AM_LDFLAGS = $(BOOST_SYSTEM_LIBS) $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) $(libgamecommon_LIBS) $(top_builddir)/src/libgamegraphics.la
