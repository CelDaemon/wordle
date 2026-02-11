licensedir = ${datarootdir}/licenses/${PACKAGE}

bin_PROGRAMS = wordle

EXTRA_wordle_SOURCES = words.txt

wordle_SOURCES = \
	wordle.c

pkgdata_DATA = \
	words.db.gz

dist_license_DATA = LICENSE

if BUILD_STATIC
  wordle_LDFLAGS = \
	  -static
endif

wordle_CPPFLAGS = \
	-DPKGDATADIR='"$(pkgdatadir)"'

wordle_CFLAGS = \
	$(z_CFLAGS) \
	-Wall -Wextra -Wpedantic

wordle_LDADD = \
	$(z_LIBS)

MOSTLYCLEANFILES = \
	words.db \
	words.db.gz

words.db: words.txt
	sort < $< | tr -d '\n' > $@

words.db.gz: words.db
	gzip -n -c $< > $@
