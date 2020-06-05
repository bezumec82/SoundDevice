/*
 * $Id: libcommon.h,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#ifndef __LIBCOMMON_H
#define __LIBCOMMON_H

extern void *xmalloc(int size);
extern void *xrealloc(void *ptr, int newsize);
extern void xfree(void *ptr);
extern char *xstrdup(char *old);
extern char *xstrcat(char *str1, char *str2);
extern int xstrtoul(char *str, unsigned long *val);
extern int xstrtol(char *str, long *val);

extern off_t ltell(int h);
extern size_t filesize(int h);

#endif
