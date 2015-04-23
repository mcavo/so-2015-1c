#include "../inc/utils.h"
#include <libgen.h>
#include <stdarg.h>

void decdump(void *memory, size_t length) {
	for (size_t i = 0; i < length; i++)
		printf(" %03d", ((char*) memory)[i]);
	printf("\n");
	fflush(stdout);
}

void check(int condition, char *fmt, ...) {
	if (condition) 
		return;
	int err = errno;
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	if (err) 
		printf(" errno %d (%s)\n", err, strerror(err));
	va_end(args);
	exit(1);
}

char *fpath(char *path) {
	char *temp = strdup(path);
	char *root = strdup(dirname(temp));
	free(temp);
	return root;
}

char *fname(char *path) {
	char *temp = strdup(path);
	char *name = strdup(basename(temp));
	free(temp);
	return name;
}
