bin_PROGRAMS = wordle

EXTRA_wordle_SOURCES = words.txt

wordle_SOURCES = \
	wordle.c

pkgdata_DATA = \
	words.db.gz

if BUILD_STATIC
  wordle_LDFLAGS = \
	  -static
endif

wordle_CPPFLAGS = \
	-DPKGDATADIR='"$(pkgdatadir)"'

wordle_LDADD = \
	-lz

MOSTLYCLEANFILES = \
	words.db \
	words.db.gz

words.db: words.txt
	tr -d '\n' < $< > $@

words.db.gz: words.db
	gzip -c $< > $@
