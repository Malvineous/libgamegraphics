INCLUDES = $(PYTHON_INCLUDES)

pyexec_LTLIBRARIES = _gamegraphics.la

_gamegraphics_la_SOURCES = gamegraphics_wrap.cxx
_gamegraphics_la_LDFLAGS = -avoid-version -module
_gamegraphics_la_LIBADD = $(BOOST_SYSTEM_LIBS) $(libgamecommon_LIBS) $(top_builddir)/src/libgamegraphics.la
_gamegraphics_la_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include $(libgamecommon_CFLAGS)

# Include the SWIG input file in the distribution
EXTRA_gamegraphics_la_DIST = gamegraphics.i

# This is one of SWIG's output files, but we include it in the dist so that
# end users don't need SWIG installed.  It's a pyexec file because it must be
# installed alongside the .so library in the pyexec dir to work.
dist_pyexec_DATA = gamegraphics.py

# Delete the SWIG output files on a 'make maintainer-clean' so SWIG can
# regenerate them.
MAINTAINERCLEANFILES = gamegraphics.py gamegraphics_wrap.cxx

# In the event that the SWIG output files are missing or the SWIG input file
# has changed, use SWIG to rebuild them.  We can't include gamegraphics.py as
# a target here or SWIG will be run twice.
%.py %_wrap.cxx: %.i
	if test -z "$(SWIG)" ; then \
		echo "SWIG is required to perform this action.  See http://www.swig.org"; \
		exit 1; \
	else \
		$(SWIG) -c++ -python -classic -Wextra -I$(top_srcdir)/include $(libgamecommon_CFLAGS) $<; \
	fi
