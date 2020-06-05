/*
 * $Id: vman.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"
#include "vman-help.h"
#include VERSION_INCLUDE

#include "libparam.h"
#include "params.h"

const char *voice_dev_name = "/dev/voice";
const char *rtp_dev_name = "/dev/rtp";
const char *hal_dev_name = "/dev/hal";


int h_ctl = -1,
    h_conf = -1,
    h_dtmf = -1,
    h_rtp = -1,
    hal_ctl = -1;

typedef struct
{
    int id;
    char* name;
    void* dev;
} codec_types;

codec_types codecs[] =
{
    {.id = CODEC_ID_G711  , .name = "g711"  , .dev = NULL },
    {.id = CODEC_ID_G723  , .name = "g723"  , .dev = NULL },
    {.id = CODEC_ID_G729  , .name = "g729"  , .dev = NULL },
    {.id = CODEC_ID_MIXER , .name = "mixer" , .dev = NULL },
    {.id = CODEC_ID_CMIXER, .name = "cmixer", .dev = NULL },
    {.id = CODEC_ID_LIN16 , .name = "lin16" , .dev = NULL },
};

//static const char *codecs[] = {/*"g711"*/ NULL, "g711", "g729", "g723", NULL};
//static const char *dev_names[] = {"/dev/g711", NULL, NULL, "/dev/dtmf", NULL, NULL, NULL, "/dev/mixer"};

const char *clk_srcs[] = {"f0", "f1", "f2", "f3", "freerun", "x3"};
int clk_srcs_count = sizeof(clk_srcs) / sizeof(const char *);

void vman_version(void)
{
    printf( "vman: %s\n"
            "vman: %s\n", version_string(), version_copyright());
}


void vman_usage(void)
{
 int i;

    fprintf(stdout, vman_help_string);

    fprintf(stdout, "\nAvailable codecs:\n\n");
    for (i = 0; i < sizeof(codecs)/sizeof(codecs[0]); i++)
        printf("\t%s\n", codecs[i].name);

    fprintf(stdout, "\nAvailable clock sources:\n\n");
    for (i = 0; i < sizeof(clk_srcs) / sizeof(char *); i++) {
     fprintf(stdout, "\t%s\n", clk_srcs[i]);
    }
    fprintf(stdout, "\n");

    exit(0);
}


int main(int argc, char **argv)
{
    return parse_params(argc, argv, app_params, sizeof(app_params) / sizeof(*app_params));
}


int get_codec_id(char *s)
{
    int i;
    for (i = 0; i < sizeof(codecs)/sizeof(codecs[0]); i++)
    {
        if (strcmp(s, codecs[i].name) == 0) return codecs[i].id;
    }
 return -1;
}

/*
const char *get_codec_devname(int codec_id)
{
    if (codec_id < 1 || codec_id >= sizeof(dev_names) / sizeof(*dev_names) + 1) {
        return NULL;
    }
    return dev_names[codec_id - 1];
}
*/

const char *get_codec_name(int id)
{
    if (id < 1 || id >= (sizeof(codecs)/sizeof(codecs[0]))) {
        return NULL;
    }
    return codecs[id].name;
}

static void open_dev(const char *name, int mode, int *h)
{
 if (*h < 0) {
  *h = open(name, mode);
  if (*h < 0) {
    fprintf(stderr, "failed to open %s: %s\n", name, strerror(errno));
    exit(1);
  }
 }
}

static void close_dev(int *h)
{
    if (*h >= 0) {
        close(*h);
        *h = -1;
    }
}

void open_ctl(void)
{
    open_dev(voice_dev_name, O_RDWR, &h_ctl);
}


void open_hal(void)
{
    open_dev(hal_dev_name, O_RDWR, &hal_ctl);
}
void close_hal(void)
{
    close_dev(&hal_ctl);
}

void close_ctl(void)
{
    close_dev(&h_ctl);
}

void open_conf(void)
{
    open_dev("/dev/mixer", O_RDWR, &h_conf);
}

void close_conf(void)
{
    close_dev(&h_conf);
}

void open_dtmf(unsigned int mode)
{
    open_dev("/dev/dtmf", mode, &h_dtmf);
}


void close_dtmf(void)
{
    close_dev(&h_dtmf);
}

void open_rtp(void)
{
    open_dev(rtp_dev_name, O_RDWR, &h_rtp);
}

void close_rtp(void)
{
    close_dev(&h_rtp);
}

int do_ioctl(int h, unsigned ctl, void *arg, char *perror_msg)
{
    int err;

    if ((err = ioctl(h, ctl, arg)) < 0) {
    	//printf("vman: do_ioctl FAIL: (0x%02X, 0x%02X, 0x%02X, %s) = %d\n", h, ctl, arg, perror_msg, err);
		if (perror_msg) {
			perror(perror_msg);
		}
		exit(1);
    }

    //printf("vman: do_ioctl OK: (0x%02X, 0x%02X, 0x%02X, %s) = %d\n", h, ctl, arg, perror_msg, err);
    return err;
}
