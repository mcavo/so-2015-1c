#ifndef __UTILS_H__
#define __UTILS_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void check(int condition, char *fmt, ...);

void decdump(void *memory, size_t length);

char *fpath(char *path);

char *fname(char *path);

#endif