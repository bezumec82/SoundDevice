/*
 * $Id: dsp.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include <byteswap.h>
#include <linux/ioctl.h>
#include "vman.h"
#include "hal-ioctl.h"
#include <time.h>

#define DSP_FIRST_DM 0x4000
#define DSP_MAX_ADDR 0x7FFF

/************************************* DspInfo *************************************/

struct dsp_info
{
	struct param_dsp_owner info;
	struct dsp_info* next;
};

void dsp_info_list_free( struct dsp_info* di )
{
	struct dsp_info* p = di;
	while( p!=0 )
	{
		struct dsp_info* temp = p->next;
		free( p );
		p = temp;
	}
}

char* dsp_info_to_str( char* buf, struct dsp_info* di )
{
	int count = 0;
	struct param_dsp_owner* pd = &di->info;

	if( pd->owner_name[0]=='\0' )
		count = sprintf( buf, "\tdsp%02d: %s\tbus=%d\n", pd->number, "-----", pd->bus );
	else
		count = sprintf( buf, "\tdsp%02d: %s\tbus=%d\tts=[%d:%d]\n", pd->number, pd->owner_name, pd->bus, pd->ts_base, pd->ts_base + pd->ts_count - 1);

	return buf+count;
}

/************************************* BusInfo *************************************/

struct bus_info
{
	int number;
	int channels;
	int freeChannels;
	struct bus_info* next;
};

char* bus_info_to_str( char* buf, struct bus_info* bi )
{
	return buf+sprintf( buf, "\tbus%02d: channels: total: %d, free: %d\n", bi->number, bi->channels, bi->freeChannels );
}

void fill_bus_info( struct bus_info* dst, const struct param_tdm_bus_info* src )
{
	dst->number = src->bus_number;
	dst->channels = src->channels;
	dst->freeChannels = src->free_channels;
}

void bus_info_list_free( struct bus_info* bi )
{
	struct bus_info* p = bi;
	while( p!=0 )
	{
		struct bus_info* temp = p->next;
		free( p );
		p = temp;
	}
}

/***********************************************************************************/

int list_dsp(void)
{
	struct param_dsp_owner owner;
	int i, n_dsp;
	char buf[2048];
	char* ptr = buf;
	struct dsp_info *dfirst=0, *dlast=0, *pd;
	struct bus_info *bfirst=0, *blast=0, *pb;

	open_hal();

	do_ioctl(hal_ctl, HAL_DSP_GET_MAX, &n_dsp, "hal_dsp_get_max");

	for (i = 0; i < n_dsp; i++)
	{
		owner.number = i;
		if (ioctl(hal_ctl, HAL_DSP_OWNER, &owner) >= 0)
		{
			struct dsp_info* temp = malloc( sizeof(struct dsp_info) );
			if( !temp )
			{
				printf( "can't create dsp_info instance!\n" );
				goto list_dsp_exit;
			}

			temp->info = owner;

			if( !dfirst )
				dfirst = dlast = temp;
			
			temp->next = 0;
			dlast->next = temp;
			dlast = temp;
		}
	}

	printf("dsp map:\n");
	fflush(stdout);

	for( pd=dfirst; pd!=0; pd=pd->next )
		ptr = dsp_info_to_str( ptr, pd );

	printf( "%s", buf );
	fflush(stdout);

	struct param_tdm_bus_number firstBus, lastBus;
	if( ioctl(hal_ctl, HAL_TDMFIRSTBUS, &firstBus)<0 || ioctl(hal_ctl, HAL_TDMLASTBUS, &lastBus)<0 )
	{
		printf("can't get first and last bus numbers!\n");
		goto list_dsp_exit;
	}

	for( i=firstBus.value; i<=lastBus.value; ++i )
	{
		struct param_tdm_bus_info bi;
		bi.bus_number = i;
		if( ioctl(hal_ctl, HAL_TDMBUSINFO, &bi)>=0 )
		{
			struct bus_info* temp = malloc( sizeof(struct bus_info) );
			if( !temp )
			{
				printf( "can't create bus_info instance!\n" );
				goto list_dsp_exit;
			}

			fill_bus_info( temp, &bi );

			if( !bfirst )
				bfirst = blast = temp;
			
			blast->next = temp;
			temp->next = 0;
			blast = temp;
		}
	}

	printf("\ndsp bus info:\n");
	fflush(stdout);

	ptr = buf;
	for( pb=bfirst; pb!=0; pb=pb->next )
		ptr = bus_info_to_str( ptr, pb );

	printf( "%s", buf );
	fflush(stdout);

list_dsp_exit:
	dsp_info_list_free( dfirst );
	bus_info_list_free( bfirst );
	close_hal();
	return 0;
}

int read_dsp_mem(struct param_dsp_mem *m)
{

	open_hal();
	if (do_ioctl(hal_ctl, HAL_READDSP, m, "hal_read_dsp") < 0)
	{
		close_hal();
		perror("dsp read mem");
		return -1;
	}
	close_hal();

	if (m->address >= DSP_FIRST_DM)
	{
		/* DM */
		printf("vman: dsp%d(0x%x) = 0x%04x\n", m->number, m->address, m->data);
	}
	else
	{
		/* PM */
		printf("vman: dsp%d(0x%x) = 0x%06x\n", m->number, m->address, m->data);
	}
	return 0;
}


static inline void do_dump_dsp(int dsp, int overlay, unsigned addr, int count)
{

	struct param_dsp_mem ios;
	int i;

	ios.number = dsp;
	ios.address = addr;
	ios.overlay_page = overlay;

	for (i = 0; i < count; i++, ios.address++)
	{
		if (((i + 1) % 8) == 1)
		{
			printf("0x%04x(%u): ", ios.address, overlay);
		}
		do_ioctl(hal_ctl, HAL_READDSP, &ios, "hal_read_dsp");
		printf(addr >= DSP_FIRST_DM ? "%04x " : "%06x ", ios.data);
		if (((i + 1) % 8) == 0)
		{
			printf("\n");
		}
	}
	if (((i + 1) % 8) != 0 && count)
	{
		printf("\n");
	}
}


int dump_dsp_mem(struct dump_dsp *m)
{
	int count, first;

//      fprintf(stderr, "vman: overlay %d\n", m->overlay);
	count = m->count <= 0 ? 8 : m->count;
	if ((m->address + count) > DSP_MAX_ADDR)
	{
		fprintf(stderr, "vman: dump: invalid address and/or size specified\n");
		return -1;
	}

	if (m->address < DSP_FIRST_DM && (m->address + count) > DSP_FIRST_DM)
	{
		first = DSP_FIRST_DM - m->address;
	}
	else if (m->address < DSP_FIRST_DM)
	{
		first = count;
	}
	else
	{
		first = 0;
	}
	open_hal();
	printf("\n");
	do_dump_dsp(m->dsp, m->overlay, m->address, first);
	do_dump_dsp(m->dsp, m->overlay, m->address + first, count - first);
	close_hal();
	return 0;
}

int write_dsp_mem(struct param_dsp_mem *m)
{
	open_hal();
	do_ioctl(hal_ctl, HAL_WRITEDSP, m, "dsp write mem");
	close_hal();
	return 0;
}

#define ST_START  1
#define ST_END    2
#define ST_EOF    3
#define ST_PA     4
#define ST_DA     5
#define ST_SEGEND 6
#define ST_NUMBER 7
#define ST_ERR    0

/*
static int exe_line_number;
static int image_get_number(FILE * img, unsigned long *number)
{
	char tstr[256];
	tstr[255] = 0;
	if (fscanf(img, "%255s", tstr) != 1)
		return ST_EOF;
	exe_line_number++;

	if (!*tstr)
		return ST_ERR;
	if (!strcmp(tstr, "i"))
		return ST_START;
	if (!strcmp(tstr, "o"))
		return ST_END;
	if (!strcmp(tstr, "@PA"))
		return ST_PA;
	if (!strcmp(tstr, "@DA"))
		return ST_DA;
	if (!strncmp(tstr, "#123", 4))
		return ST_SEGEND;

	if (sscanf(tstr, "%lx", number) != 1)
	{
		fprintf(stderr, "\nInvalid record: %s\n", tstr);
		return ST_ERR;
	}
	return ST_NUMBER;
}
*/

static __inline__ void do_microsleep(unsigned msec)
{
	struct timespec req, rem;
	int err;

	req.tv_sec = msec / 1000000000;
	req.tv_nsec = (msec % 1000000000) / 1000;
	for (;;)
	{
		err = nanosleep(&req, &rem);
		if (err == -1)
		{
			if (errno == EINTR) req = rem;
			else break;
		}
		else return;
	}
}


static __inline__ int dsp_do_reset(int dsp)
{
	struct param_reset_dsp ios;

	fprintf(stderr, "vman: resetting dsp...");
	fflush(stderr);

	ios.number=dsp;
	ios.action=RESETDSP_ACTION_RESET;

	do_ioctl(hal_ctl, HAL_RESETDSP, &ios, "\ndsp reset");
	do_microsleep(100000);
	ios.action=RESETDSP_ACTION_UNRESET;
	do_ioctl(hal_ctl, HAL_RESETDSP, &ios, "\ndsp unreset");
	do_microsleep(100000);
	fprintf(stderr, "done\n");
	return 0;
}


int reset_dsp(int dsp)
{
	int stat;

	open_hal();
	stat = dsp_do_reset(dsp);
	close_hal();
	return stat;
}

int dsp_show_bus(int number)
{
#if 0
	int bus_number;
	open_hal();
	bus_number = do_ioctl(hal_ctl, HW_DSP_BUS, &number, "HW_DSP_BUS");
	fprintf(stdout, "vman: dsp(%d) has bus %u\n", number, bus_number);
	close_hal();
#endif
	return 0;
}


#define MAX_OVERLAYS 51

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define is_code(x) ((x) && 0x4000 == 0)

struct module_packet {
	unsigned short size;
	unsigned short addr;
	unsigned short ovr_page;
	unsigned short *data;
};

int parse_image(char *filename, struct module_packet **mp)
{
	int np = 0;
	FILE *src;
	struct module_packet *p = NULL;

	src = fopen(filename, "rt");
	if (!src)
	{
		fprintf(stderr, "error: could not open %s: %s\n", filename, strerror(errno));
		exit(-1);
	}

	while (!feof(src))
	{
		unsigned short pp, aa, ss;
		unsigned long n;

		fscanf(src, "%hx\n", &ss);

		if (ss == 0xFFFF) break;
		fscanf(src, "%hx\n", &aa);
		fscanf(src, "%hx\n", &pp);

		if (aa & 0x4000) pp >>= 4;
		pp &= 0xF;
		printf("got block: overlay=%d, address=%04X, size=%d\n", pp, aa, ss);

		if (!(p = realloc(p, sizeof(struct module_packet) * (np + 1))))
		{
			fprintf(stderr, "error: malloc failed: %s\n", strerror(errno));
			exit(-1);
		}
		p[np].ovr_page = pp;
		p[np].addr = aa;
		p[np].size = ss;

		if (!(p[np].data = malloc(ss * (is_code(aa) ? 2 : 4))))
		{
			fprintf(stderr, "error: malloc failed: %s\n", strerror(errno));
			exit(-1);
		}
		n = 0;

		while (!feof(src) && ss--)
		{
			fscanf(src, "%hx\n", &p[np].data[n++]);
			if (is_code(aa))
				fscanf(src, "%hx\n", &p[np].data[n++]);
		}
		np++;
		if (ss != 0xFFFF)
		{
			fprintf(stderr, "warning: unexpected end of stream (%u)\n", np);
//                      exit(-1);
		}
	}
	fclose(src);
	*mp = p;
	return np;
}

int pack_image_compare(struct module_packet *mp1, struct module_packet *mp2)
{
	return (mp1->data && mp2->data) || (!mp1->data && !mp2->data) ? 0 : mp1->data ? -1 : 1;
}

int pack_image(char *filename, struct module_packet **mp, int np)
{
	int i, j, nnp = np;
	struct module_packet *p = *mp;

	for (i = 0; i < np; i++)
	{
		if (p[i].data)
		{
			for (j = 0; j < np; j++)
			{

				if (i != j && p[j].data && p[i].addr + p[i].size == p[j].addr
				    && p[i].ovr_page == p[j].ovr_page)
				{

					int mem_item_size = (is_code(p[i].addr)) ? 4 : 2;
					if (!(p[i].data = realloc(p[i].data, (p[i].size + p[j].size) * mem_item_size)))
					{
						fprintf(stderr, "error: malloc failed: %s\n", strerror(errno));
						exit(-1);
					}
					memcpy(((unsigned char *) p[i].data) + mem_item_size * p[i].size, p[j].data,
					       p[j].size * mem_item_size);
					p[i].size += p[j].size;
					free(p[j].data);
					p[j].data = NULL;
					nnp--;
				}
			}
		}
	}
	qsort(p, np, sizeof(struct module_packet), (int (*)(const void *, const void *)) pack_image_compare);


	if (!(*mp = realloc(*mp, nnp * sizeof(struct module_packet))))
	{
		fprintf(stderr, "error: malloc failed: %s\n", strerror(errno));
		exit(-1);
	}

	return nnp;
}

void free_image(struct module_packet *mp, int np)
{
	int i;

	for (i = 0; i < np; i++)
		if (mp[i].data)
			free(mp[i].data);
	free(mp);
}

int upload_dsp_image(struct upload_dsp_image *ud)
{
	struct param_dsp_upload	p_upload;
	struct param_reset_dsp	p_reset;
	int np, pnp, i;
	struct module_packet *mp;
	char *out_buf;
	unsigned short *in_buf, *in_buf_ptr;
	unsigned img_size;
	int compress = 1;

	np = parse_image(ud->image_name, &mp);
	printf("%d records in image %s\n", np, ud->image_name);
	if (!np)
	{
		fprintf(stderr, "error: image \"%s\" contains no records\n", ud->image_name);
		return -1;
	}

	pnp = pack_image(ud->image_name, &mp, np);
	if (pnp != np)
	{
		fprintf(stderr, "image \"%s\" originally contains %u records,\n"
				"after packing it contains %u records.\n", ud->image_name, np, pnp);
	}
	for (i = 0, img_size = 0; i < pnp; i++)
	{
		if (mp[i].size)
		{
			if (is_code(mp[i].addr))
				img_size += mp[i].size * 2;
			else
				img_size += mp[i].size;
			img_size += 3;
		}
	}

	in_buf = malloc(img_size * sizeof(unsigned short));
	if (!in_buf)
	{
		perror("malloc");
		return 1;
	}
	in_buf_ptr = in_buf;
	if (compress)
	{
		out_buf = malloc(img_size * sizeof(unsigned short) * 2);
	}
	else
	{
		out_buf = NULL;
	}

	for (i = 0; i < pnp; i++)
	{
		unsigned short addr, count;

		addr = mp[i].addr;
		count = mp[i].size;
		if (is_code(mp[i].addr)) count *= 2;
		if (count)
		{
			*in_buf_ptr++ = addr;
			*in_buf_ptr++ = count;
			*in_buf_ptr++ = mp[i].ovr_page;
			memcpy(in_buf_ptr, mp[i].data, count * sizeof(unsigned short));
			in_buf_ptr += count;
		}
	}

#ifdef POWERPC
	for(i=0; i < img_size; ++i) in_buf[i]=__bswap_16(in_buf[i]);
#endif

	img_size *= sizeof(unsigned short);

	printf("img_size=%d\n", img_size);
	p_reset.number=ud->dsp;
	p_reset.action=RESETDSP_ACTION_RESET;
	open_hal();
	do_ioctl(hal_ctl, HAL_RESETDSP, &p_reset, "\ndsp reset");
	do_microsleep(100000);
	p_reset.action=RESETDSP_ACTION_UNRESET;
	do_ioctl(hal_ctl, HAL_RESETDSP, &p_reset, "\ndsp unreset");
	do_microsleep(100000);
	p_upload.number=ud->dsp;
	p_upload.code=in_buf;
	p_upload.size=img_size;
	do_ioctl(hal_ctl, HAL_DSP_UPLOAD, &p_upload, "\nupload image");
	close_hal();
	free(in_buf);
	return 0;
}

static inline unsigned short read_dsp_mem_lp(struct param_dsp_mem *m)
{
	open_hal();
	if (do_ioctl(hal_ctl, HAL_READDSP, m, "hal_read_dsp") < 0)
	{
		close_hal();
		perror("dsp read mem");
		return 0;
	}
	close_hal();
	return 1;
}

static inline void read_dsp_block(unsigned short* buf, int count, int dsp, int page, unsigned addr)
{
	struct param_dsp_mem ios;
	int i;

	ios.number = dsp;
	ios.overlay_page = page;
	ios.address = addr;

	open_hal();
	for (i = 0; i < count; i++, ios.address++)
	{
		do_ioctl(hal_ctl, HAL_READDSP, &ios, "hal_read_dsp");
		*buf++ = ios.data;
	}
	close_hal();
}

static inline void write_dsp_block(unsigned short* buf, int count, int dsp, int page, unsigned addr)
{

	struct param_dsp_mem ios;
	int i;

	ios.number = dsp;
	ios.overlay_page = page;
	ios.address = addr;

	open_hal();
//	printf("o=%d a=%x\n", sizeof(ios.overlay_page), sizeof(ios.address));
	for (i = 0; i < count; i++, ios.address++)
	{
		ios.data = *buf++;
		do_ioctl(hal_ctl, HAL_WRITEDSP, &ios, "hal_write_dsp");
	}
	close_hal();
}


#define	VTEST_SPEED53  0x0001
#define	VTEST_HP_EN    0x0002
#define	VTEST_PF_EN    0x0004
#define	VTEST_VAD_EN   0x0008
#define	VTEST_BAD_CRC  0x0010

#define	VTEST_IBUF_RDY	0x0100
#define	VTEST_OBUF_RDY	0x0200
#define	VTEST_RESTART	0x0800

#define DSP_EXCH_BASE	0x6000
#define DSP_EXCH_MAGIC	(DSP_EXCH_BASE+0)
#define DSP_EXCH_FLAG	(DSP_EXCH_BASE+1)
#define DSP_EXCH_IPAGE	(DSP_EXCH_BASE+2)
#define DSP_EXCH_IADDR	(DSP_EXCH_BASE+3)
#define DSP_EXCH_ISIZE	(DSP_EXCH_BASE+4)
#define DSP_EXCH_OPAGE	(DSP_EXCH_BASE+5)
#define DSP_EXCH_OADDR	(DSP_EXCH_BASE+6)
#define DSP_EXCH_OSIZE	(DSP_EXCH_BASE+7)

inline void wait_dsp_data(int dsp)
{
	struct param_dsp_mem dsp_mem;

	dsp_mem.number = dsp;
	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_FLAG;
	do { if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1); }
	while((dsp_mem.data & VTEST_OBUF_RDY) == 0);
}

inline void vtest_send_event(struct test_dsp_param *tparm)
{
	struct param_dsp_mem dsp_mem;

	dsp_mem.number = tparm->dsp;
	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_FLAG;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	dsp_mem.data |= VTEST_IBUF_RDY;			// send signal to DSP
	dsp_mem.data &= ~VTEST_OBUF_RDY;		// clear flag
	write_dsp_mem(&dsp_mem);
}

inline void vtest_get_ibsize(struct test_dsp_param *tparm)
{
	struct param_dsp_mem dsp_mem;
	dsp_mem.number = tparm->dsp;
	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_ISIZE;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->isize = dsp_mem.data;
}

inline void vtest_get_obsize(struct test_dsp_param *tparm)
{
	struct param_dsp_mem dsp_mem;

	dsp_mem.number = tparm->dsp;
	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_OSIZE;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->osize = dsp_mem.data;
}

inline void set_crc(struct test_dsp_param *tparm, int value)
{
	struct param_dsp_mem dsp_mem;

	dsp_mem.number = tparm->dsp;
	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_FLAG;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	if (value)
		dsp_mem.data |= VTEST_BAD_CRC;
	else
		dsp_mem.data &= ~VTEST_BAD_CRC;
	write_dsp_mem(&dsp_mem);
//	printf("set bad CRC\n");
}

//void set_test_parm(char)
int vtest_configure(struct test_dsp_param *tparm)
{
	struct param_dsp_mem dsp_mem;

	dsp_mem.number = tparm->dsp;
	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_MAGIC;

	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	if (dsp_mem.data != 0xA5F0)
	{
		fprintf(stderr, "invalid magic word: %04X\n", dsp_mem.data);
		exit(-1);
	}
//	printf ("restarting ...\n");

	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_FLAG;
	dsp_mem.data = VTEST_RESTART;
	write_dsp_mem(&dsp_mem);

	/* waiting for DSP made a sowtfare restart */

	do {if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);}
	while(dsp_mem.data);

//	printf ("setting parameters ...\n");

	dsp_mem.overlay_page = 0;
	dsp_mem.address = DSP_EXCH_FLAG;
	dsp_mem.data = 0;
	dsp_mem.data |= strchr(tparm->mode_str, 'p') ? VTEST_PF_EN:0;
	dsp_mem.data |= strchr(tparm->mode_str, 'h') ? VTEST_HP_EN:0;
	dsp_mem.data |= strchr(tparm->mode_str, '5') ? VTEST_SPEED53:0;
	dsp_mem.data |= strchr(tparm->mode_str, 'v') ? VTEST_VAD_EN:0;
	write_dsp_mem(&dsp_mem);

//	printf ("read configuration ...\n");

	dsp_mem.address = DSP_EXCH_IPAGE;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->ipage = dsp_mem.data;

	dsp_mem.address = DSP_EXCH_IADDR;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->iaddr = dsp_mem.data;

	dsp_mem.address = DSP_EXCH_ISIZE;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->isize = dsp_mem.data;

	dsp_mem.address = DSP_EXCH_OPAGE;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->opage = dsp_mem.data;

	dsp_mem.address = DSP_EXCH_OADDR;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->oaddr = dsp_mem.data;

	dsp_mem.address = DSP_EXCH_OSIZE;
	if (read_dsp_mem_lp(&dsp_mem) == 0) exit(-1);
	tparm->osize = dsp_mem.data;

/*	printf ("inbuf:\n"
			"  page: %02x\n"
			"  addr: 0x%04x\n"
			"  size: %d\n"
			"outbuf:\n"
			"  page: %02x\n"
			"  addr: 0x%04x\n"
			"  size: %d\n",
		tparm->ipage, tparm->iaddr, tparm->isize,
		tparm->opage, tparm->oaddr, tparm->osize);
*/
	tparm->iaddr |= 0x4000;
	tparm->oaddr |= 0x4000;

	if (tparm->isize <=0) exit(-1);
	if (tparm->osize <=0) exit(-1);
	return 0;
}


int vtest_dsp(struct test_dsp_param *tparm)
{
//	struct dsp_mem_param dsp_mem;
	FILE *f_in;
	FILE *f_out;
	FILE *f_crc;
	int r_count, w_count;
	unsigned short dbuf[1024];
	int frames_count = 0;
	unsigned short crc;

	vtest_configure(tparm);

	f_in = fopen(tparm->input_filename, "rb");
	if (f_in == NULL)
	{
		close_hal();
		fprintf(stderr, "error: could not open %s: %s\n", tparm->input_filename, strerror(errno));
		exit(-1);
	}

	f_out = fopen(tparm->output_filename, "wb");
	if (f_out == NULL)
	{
		close_hal();
		fclose(f_in);
		fprintf(stderr, "error: could not open %s: %s\n", tparm->input_filename, strerror(errno));
		exit(-1);
	}

	f_crc = fopen(tparm->crc_filename, "rb");

	printf ("start test %s on DSP:%d\n", tparm->input_filename, tparm->dsp);
	printf ("frames processed:     ");
/*

	dsp_mem.addr = 0;
	dsp_mem.data = 0;
	write_dsp_mem(&dsp_mem);
*/
	/* main cycle */
	while (!feof(f_in))
	{
		/*	upload source data	*/
		vtest_get_ibsize(tparm);
		r_count = fread(dbuf, sizeof(short), tparm->isize, f_in);

		if (f_crc != NULL)
		{
			fread(&crc, sizeof(short), 1, f_crc);
			set_crc(tparm, crc);
		}

		if (r_count == 0) goto ball_out;
		if (r_count < 0) exit(-1);
		write_dsp_block(dbuf, r_count, tparm->dsp, tparm->ipage, tparm->iaddr);
//		printf ("read block:%d bytes\n", r_count);
		vtest_send_event(tparm);
		/*	download dest data	*/
		wait_dsp_data(tparm->dsp);
		vtest_get_obsize(tparm);
//		printf ("read block:%d bytes\n", tparm->osize);
		read_dsp_block(dbuf, tparm->osize, tparm->dsp, tparm->opage, tparm->oaddr);
		w_count = fwrite(dbuf, sizeof(short), tparm->osize, f_out);
		printf ("\b\b\b\b%4d", ++frames_count);
//		exit(0);
	}
ball_out:
	fclose(f_in); fclose(f_out);
	if (f_crc != NULL)	fclose(f_crc);
	printf ("\n");
	return 0;
}
