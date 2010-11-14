check_PROGRAMS = tests

tests_SOURCES = tests.cpp
tests_SOURCES += test-img-ega-byteplanar.cpp
tests_SOURCES += test-tls-ccaves.cpp
tests_SOURCES += test-pal-vga-raw.cpp

EXTRA_tests_SOURCES = tests.hpp
EXTRA_tests_SOURCES += test-img.hpp
EXTRA_tests_SOURCES += test-tls.hpp

TESTS = tests

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include $(libgamecommon_CFLAGS)
AM_LDFLAGS = $(BOOST_SYSTEM_LIBS) $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) $(libgamecommon_LIBS) $(top_builddir)/src/libgamegraphics.la
