/*
 * $Id: mem.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>

void *xmalloc(int size)
{
	void *res;

	res = malloc(size);
	if (!res) {
		fprintf(stderr, "failed to allocate %d bytes: %s\n", size, strerror(errno));
		exit(1);
	}
	memset(res, 0, size);

	return res;
}

void *xrealloc(void *ptr, int newsize)
{
	void *res;

	res = realloc(ptr, newsize);
	if (!res) {
		fprintf(stderr, "failed to reallocate %d bytes: %s\n", newsize, strerror(errno));
		exit(1);
	}

	return res;
}

void xfree(void *ptr)
{
	free(ptr);
}

char *xstrdup(char *old)
{
	char *newstr;
	int len;

	len = strlen(old) + 1;
	newstr = malloc(len);
	if (!newstr) {
		fprintf(stderr, "failed to duplicate string %d bytes long: %s\n", len - 1, strerror(errno));
		exit(1);
	}

	strcpy(newstr, old);

	return newstr;
}

char *xstrcat(char *str1, char *str2)
{
	char *newstr;
	int len;

	len = strlen(str1) + strlen(str2) + 1;
	newstr = malloc(len);
	if (!newstr) {
		fprintf(stderr, "failed to concate two strings (%d bytes): %s\n", len - 1, strerror(errno));
		exit(1);
	}

	strcpy(newstr, str1);
	strcat(newstr, str2);

	return newstr;
}


int xstrtoul(char *str, unsigned long *val)
{
	char *endptr = NULL;
	*val = strtoul(str, &endptr, 0);
	if (endptr && *endptr == '\0') return 0;
	return -1;
}

int xstrtol(char *str, long *val)
{
	char *endptr = NULL;
	*val = strtol(str, &endptr, 0);
	if (endptr && *endptr == '\0') return 0;
	return -1;
}
