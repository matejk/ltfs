/* Test helper exercising syscalls that shell utilities do not reach
 * directly: renameat2() flags, ftruncate() on an open descriptor, and
 * mmap().
 *
 * Usage:
 *   fsops_helper noreplace <old> <new>   renameat2 with RENAME_NOREPLACE
 *   fsops_helper exchange <a> <b>        renameat2 with RENAME_EXCHANGE
 *   fsops_helper ftruncate <file> <len>  ftruncate an open fd, print new size
 *   fsops_helper mmap <file> <len>       mmap PROT_READ and read one byte
 *
 * Exit codes: 0 = success, 2 = syscall failed (errno printed), 3 = usage.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc != 4) {
		fprintf(stderr, "usage: %s noreplace|exchange|ftruncate <arg> <arg>\n",
			argv[0]);
		return 3;
	}

	if (!strcmp(argv[1], "noreplace") || !strcmp(argv[1], "exchange")) {
		unsigned int flags = !strcmp(argv[1], "noreplace") ?
			RENAME_NOREPLACE : RENAME_EXCHANGE;

		if (renameat2(AT_FDCWD, argv[2], AT_FDCWD, argv[3], flags) < 0) {
			printf("%s\n", strerror(errno));
			return 2;
		}
		return 0;
	}

	if (!strcmp(argv[1], "ftruncate")) {
		struct stat st;
		off_t len = strtoll(argv[3], NULL, 10);
		int fd = open(argv[2], O_RDWR);

		if (fd < 0 || ftruncate(fd, len) < 0 || fstat(fd, &st) < 0) {
			printf("%s\n", strerror(errno));
			return 2;
		}
		printf("%lld\n", (long long)st.st_size);
		close(fd);
		return 0;
	}

	if (!strcmp(argv[1], "mmap")) {
		size_t len = strtoull(argv[3], NULL, 10);
		int fd = open(argv[2], O_RDONLY);
		volatile char first;
		char *p;

		if (fd < 0) {
			printf("%s\n", strerror(errno));
			return 2;
		}
		p = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
		if (p == MAP_FAILED) {
			printf("%s\n", strerror(errno));
			close(fd);
			return 2;
		}
		first = p[0];
		(void)first;
		munmap(p, len);
		close(fd);
		return 0;
	}

	fprintf(stderr, "unknown command: %s\n", argv[1]);
	return 3;
}
