/*
 * $Id: io.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

off_t ltell(int h)
{
	return lseek(h, 0, SEEK_CUR);
}

size_t filesize(int h)
{
	off_t cur_pos;
	size_t size;

	cur_pos = ltell(h);
	if (cur_pos < 0) {
		return cur_pos;
	}
	size = lseek(h, 0, SEEK_END);
	lseek(h, cur_pos, SEEK_SET);
	return size;
}
