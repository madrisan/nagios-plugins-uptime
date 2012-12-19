PACKAGE = nagios-plugins-linux-uptime
VERSION = 2
DISTDIR = $(PACKAGE)-$(VERSION)
DIST_ARCHIVE = $(DISTDIR).tar.bz2

DESTDIR =
LIBDIR = /usr/lib
NAGIOSPLUGINDIR = $(LIBDIR)/nagios/plugins

INSTALL = /usr/bin/install -c
INSTALL_DIR = ${INSTALL} -d -m 755
INSTALL_PROGRAM = ${INSTALL} -m 755

#CFLAGS = -O2 -Wall -W -pedantic
CFLAGS = -O2 -Wall -W

.SUFFIXES: .c .o
.c.o:; $(CC) -c $(CFLAGS) -I. -o $@ $<

targets = check_uptime
libraries = nputils.o

all: $(targets)

check_filesystem: check_filesystem.o $(libraries)
	$(CC) $< $(libraries) -o $@

check_uptime: check_uptime.o $(libraries)
	$(CC) $< $(libraries) -o $@

install: all
	$(INSTALL_DIR) $(DESTDIR)$(NAGIOSPLUGINDIR)
	for p in $(targets); do\
	   $(INSTALL_PROGRAM) $$p $(DESTDIR)$(NAGIOSPLUGINDIR)/$$p;\
	done

.PHONY : clean
clean:; $(RM) *.o $(targets) *~

dist: clean
	@rm -f history/$(DIST_ARCHIVE)
	@$(INSTALL_DIR) history
	@tmpdir=`mktemp -q -d -t $(PACKAGE)-dist.XXXXXXXX`;\
	[ $$? -eq 0 ] || \
	 { echo "cannot create a temporary directory"; exit 1; };\
	$(INSTALL_DIR) $$tmpdir/$(PACKAGE)-$(VERSION);\
	find . -mindepth 1 \( -name .git -o -name history \) \
	   -prune -o \( \! -name *~ -print0 \) | \
	   cpio --quiet -pmd0 $$tmpdir/$(PACKAGE)-$(VERSION)/;\
	currdir="`pwd`";\
	pushd $$tmpdir/$(PACKAGE)-$(VERSION)/ >/dev/null;\
	tar cf - -C .. $(PACKAGE)-$(VERSION) |\
	   bzip2 -9 -c > "$$currdir"/history/$(DIST_ARCHIVE);\
	[ -f "$$currdir/history/$(DIST_ARCHIVE)" ] && \
	   echo "Wrote: $$currdir/history/$(DIST_ARCHIVE)";\
	popd >/dev/null;\
	rm -fr $$tmpdir
