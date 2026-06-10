#
# Integration tests for LTFS using the file tape backend.
#
# Run with: make check
# The tests need a Linux host with /dev/fuse; elsewhere they are skipped.
# On macOS use tests/run-in-docker.sh to run them in a Linux container.
#

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)

AM_TESTS_ENVIRONMENT = \
	top_builddir='$(abs_top_builddir)' top_srcdir='$(abs_top_srcdir)'; \
	export top_builddir top_srcdir;

TESTS = \
	t/00-mount-umount.sh \
	t/01-io-roundtrip.sh \
	t/02-truncate.sh \
	t/03-rename.sh \
	t/04-dirs-unlink.sh \
	t/05-xattr.sh \
	t/06-symlink.sh \
	t/07-large-dir.sh \
	t/08-tar-roundtrip.sh

EXTRA_DIST = \
	lib/harness.sh \
	$(TESTS) \
	docker/Dockerfile \
	docker/icu-config \
	run-in-docker.sh
