SUBDIRS = src
EXTRA_DIST = autogen.sh TODO

dist: clean
	@rm -f history/$(DIST_ARCHIVES)
	@$(MKDIR_P) history
	@tmpdir=`/bin/mktemp -q -d -t $(PACKAGE_NAME)-dist.XXXXXXXX`;\
	[ $$? -eq 0 ] || \
	 { echo "cannot create a temporary directory"; exit 1; };\
	$(MKDIR_P) $$tmpdir/$(PACKAGE_NAME)-$(VERSION_NAME);\
	find . -mindepth 1 \( -name .git -o -name history \) \
	   -prune -o \( \! -name *~ -print0 \) | \
	   cpio --quiet -pmd0 $$tmpdir/$(PACKAGE_NAME)-$(VERSION_NAME)/;\
	currdir="`pwd`";\
	pushd $$tmpdir/$(PACKAGE_NAME)-$(VERSION_NAME)/ >/dev/null;\
	tar cf - -C .. $(PACKAGE_NAME)-$(VERSION_NAME) |\
	   bzip2 -9 -c > "$$currdir"/history/$(DIST_ARCHIVES);\
	[ -f "$$currdir/history/$(DIST_ARCHIVES)" ] && \
	   echo "Wrote: $$currdir/history/$(DIST_ARCHIVES)";\
	popd >/dev/null;\
	rm -fr $$tmpdir
