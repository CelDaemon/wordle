licensedir = ${datarootdir}/licenses/${PACKAGE}

bin_PROGRAMS = wordle
noinst_PROGRAMS = mkwords

EXTRA_wordle_SOURCES = words.txt

wordle_SOURCES = \
	wordle.c

mkwords_SOURCES = mkwords.c

pkgdata_DATA = \
	words.db.gz

dist_license_DATA = LICENSE

wordle_CPPFLAGS = \
	-DPKGDATADIR='"$(pkgdatadir)"'

wordle_CFLAGS = \
	$(z_CFLAGS) \
	-Wall -Wextra -Wpedantic

wordle_LDADD = \
	$(z_LIBS)

mkwords_CPPFLAGS = \
	-DPKGDATADIR='"$(pkgdatadir)"'

mkwords_CFLAGS = \
	-Wall -Wextra -Wpedantic

MOSTLYCLEANFILES = \
	words.db \
	words.db.gz

words.db: words.txt mkwords
	sort < $< | tr -d '\n' > $@

words.db.gz: words.db
	gzip -c $< > $@
