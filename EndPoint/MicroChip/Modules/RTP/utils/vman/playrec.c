/*
 * $Id: playrec.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"


int record_file(struct record *r)
{
#if 0
 int h, ptr, ho, codec_id, tdm;
 const char *codec_devname;

 if (r->filename == NULL) {
		ho = fileno(stdout);
 } else {
 	ho = open(r->filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
 	if (ho < 0) {
 		fprintf(stderr, "error: failed to create %s: %s\n", r->filename, strerror(errno));
 	}
 }

 codec_id = get_codec_id(r->codec_id);

	if (codec_id <= 0) {
		close(ho);
		if (r->filename) {
			unlink(r->filename);
		}
		fprintf(stderr, "error: invalid codec specified (%s)\n", r->codec_id);
 	return -1;
	}

	r->max_size *= 1024;

	codec_devname = get_codec_devname(codec_id);
	if (!codec_devname) {
		close(ho);
		if (r->filename) {
			unlink(r->filename);
		}
		fprintf(stderr, "error: codec %s has no device\n", r->codec_id);
		return -1;
	}

	h = open(codec_devname, O_RDONLY);
	if (h < 0) {
		close(ho);
		if (r->filename) {
			unlink(r->filename);
		}
		perror("open codec");
		return -1;
	}
	open_ctl();

	tdm = ioctl(h, CODEC_CHANNEL);
	if (tdm < 0) {
		close(h);
		close(ho);
		if (r->filename) {
			unlink(r->filename);
		}
	 perror("CODEC_CHANNEL");
	 return -1;
	}
	if (do_mux_connect(tdm, r->channel) < 0) {
		return -1;
	}

	fprintf(stderr, "vman: recording using %s codec (pid = %u)\n", r->codec_id, getpid());
	fflush(stderr);

	ptr = 0;
	while (ptr < r->max_size || !r->max_size) {
 	char block[512];
		int nread;

		if (ptr + sizeof(block) < r->max_size || !r->max_size)
 		nread = read(h, block, sizeof(block));
 	else
 	 nread = read(h, block, r->max_size - ptr);
		if (nread < 0) {
 		close(h);
		 perror("read");
		 return -1;
		}
		if (!nread)
		 continue;
	 if (write(ho, block, nread) < 0) {
	 	if (errno != ENOSPC) {
 			close(h);
  		close(ho);
	 		unlink(r->filename);
 	 	perror("write");
 	 }
	 	return -1;
	 }
	 if (r->max_size) {
 		ptr += nread;
 	}
	}


	if (do_mux_disconnect(tdm) < 0) {
 	close(h);
		return -1;
	}

	close(h);

	if (r->filename) {
		close(ho);
	}

	close_ctl();
#endif
	return 0;
}

int play_file(struct play *p)
{
#if 0
 int h, ptr, hi, codec_id, tdm;
 const char *codec_devname;
 struct pollfd ufd;

 if (p->filename == NULL) {
		hi = fileno(stdout);
 } else {
 	hi = open(p->filename, O_RDONLY);
 	if (hi < 0) {
 		fprintf(stderr, "error: failed to open %s: %s\n", p->filename, strerror(errno));
 	}
 }

 codec_id = get_codec_id(p->codec_id);

	if (codec_id <= 0) {
		fprintf(stderr, "error: invalid codec specified (%s)\n", p->codec_id);
 	return -1;
	}

	codec_devname = get_codec_devname(codec_id);
	if (!codec_devname) {
		fprintf(stderr, "error: codec %s has no device\n", p->codec_id);
	}

	h = open(codec_devname, O_WRONLY);
	if (h < 0) {
		perror("open codec");
		return -1;
	}

	tdm = ioctl(h, CODEC_CHANNEL);
	if (tdm < 0) {
		close(h);
		close(hi);
		if (p->filename) {
			unlink(p->filename);
		}
	 perror("CODEC_CHANNEL");
	 return -1;
	}
	open_ctl();
	if (do_mux_connect(p->channel, tdm) < 0) {
		return -1;
	}

	fprintf(stderr, "vman: playing using %s codec (pid = %u)\n", p->codec_id, getpid());
	fflush(stderr);

	ptr = 0;
	while (1) {
 	char block[511];
		int nread, nwrite;

		nread = read(hi, block, sizeof(block));
		if (nread < 0) {
			close(h);
		 perror("read");
		 return -1;
		}
		if (!nread)
		 break;
	 nwrite = write(h, block, nread);
	 if (nwrite < 0) {
	 	close(h);
	  perror("write");
	  return -1;
	 }
		ptr += nwrite;
	}

	ufd.fd = h;
	ufd.events = POLLOUT;
restart_poll:
	hi = poll(&ufd, 1, -1);
	if (hi < 0) {
		fprintf(stderr, "vman: poll failed\n");
	} else {
		if (!(ufd.revents & POLLOUT)) {
			fprintf(stderr, "vman: waiting for end of play\n");
		 goto restart_poll;
		}
	}
	fflush(stderr);

	if (do_mux_disconnect(p->channel) < 0) {
 	close(h);
 	close(hi);
		return -1;
	}


	close(hi);
	close(h);
#endif
	return 0;
}


