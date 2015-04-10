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
tests_SOURCES += test-img-ddave.cpp
tests_SOURCES += test-img-ega-linear.cpp
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
tests_SOURCES += test-pal-vga-raw8.cpp
tests_SOURCES += test-pal-defaults.cpp
tests_SOURCES += test-subimage.cpp
tests_SOURCES += test-tileset.cpp
tests_SOURCES += test-tls-bash-sprite.cpp
tests_SOURCES += test-tls-ccaves-sub.cpp
tests_SOURCES += test-tls-ccomic.cpp
tests_SOURCES += test-tls-ccomic2.cpp
tests_SOURCES += test-tls-cosmo.cpp
tests_SOURCES += test-tls-ddave.cpp
tests_SOURCES += test-tls-harry-hsb.cpp
tests_SOURCES += test-tls-harry-ico.cpp
tests_SOURCES += test-tls-stryker.cpp
tests_SOURCES += test-tls-vinyl.cpp
tests_SOURCES += test-tls-zone66.cpp

EXTRA_tests_SOURCES = tests.hpp
EXTRA_tests_SOURCES += test-filter.hpp
EXTRA_tests_SOURCES += test-image.hpp
EXTRA_tests_SOURCES += test-tileset.hpp

TESTS = tests

AM_CPPFLAGS  = $(BOOST_CPPFLAGS)
AM_CPPFLAGS += -I $(top_srcdir)/include
AM_CPPFLAGS += $(libgamecommon_CFLAGS)
AM_CPPFLAGS += $(libgamearchive_CFLAGS)

AM_LDFLAGS  = $(BOOST_SYSTEM_LIBS)
AM_LDFLAGS += $(BOOST_UNIT_TEST_FRAMEWORK_LIBS)
AM_LDFLAGS += $(libgamecommon_LIBS)
AM_LDFLAGS += $(libgamearchive_LIBS)
AM_LDFLAGS += $(top_builddir)/src/libgamegraphics.la
