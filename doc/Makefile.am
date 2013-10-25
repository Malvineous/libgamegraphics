man_MANS = gametls.1
man_MANS += gameimg.1

EXTRA_DIST = gametls.xml
EXTRA_DIST += gameimg.xml
EXTRA_DIST += camoto.xsl

# Also distribute the converted man pages so users don't need DocBook installed
EXTRA_DIST += $(man_MANS)

HTML_MAN = gametls.html
HTML_MAN += gameimg.html

.PHONY: html

html: $(HTML_MAN)

MAINTAINERCLEANFILES = $(man_MANS) $(HTML_MAN)

%.1: %.xml camoto.xsl
	xmlto --skip-validation man -m $(<D)/camoto.xsl $<

%.html: %.xml camoto.xsl
	xmlto --skip-validation html-nochunks -m $(<D)/camoto.xsl $<
