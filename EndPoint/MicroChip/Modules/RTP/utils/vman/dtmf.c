/*
 * $Id: dtmf.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"

extern struct playtone ptone;

int read_dtmf(void)
{
	int i;
	dtmf_event_t event;
	int nread;
	open_dtmf(O_RDONLY);
	for (i=256; i<512; i++)
		ioctl(h_dtmf, DTMF_ENABLE_DETECTOR, i);

	while (1)
	{
		nread = read(h_dtmf, &event, sizeof(event));
		if (nread < 0)	{
			perror("tonec read");
			return -1;
		}
		if (event.code >= ' ')
			printf("vman(%d): dtmf (ts=%d) digit = %c\n", event.timestamp, event.timeslot, event.code);
		else
			printf("vman(%d): dtmf (ts=%d) special code = %02x\n", event.timestamp, event.timeslot, event.code);
	}
	for (i=256; i<512; i++)
		ioctl(h_dtmf, DTMF_DISABLE_DETECTOR, i);
	close_dtmf();
}

int write_dtmf(void)
{
	dtmf_event_t event;
	int i;
	open_dtmf(O_WRONLY);
	printf("playing: %s\n", ptone.string);
	for(i=0; i < strlen(ptone.string); ++i)
	{
		event.timeslot=ptone.timeslot;
		event.code=(int)ptone.string[i];
		write(h_dtmf, &event, sizeof(event));
	}
	close_dtmf();
	return 0;
}

int clear_dtmf(unsigned int timeslot)
{
	open_dtmf(O_WRONLY);
	do_ioctl(h_dtmf, DTMF_CLEAR_QUEUE, (void *)timeslot, "DTMF_CLEAR_QUEUE");
	close_dtmf();
	return 0;
}
