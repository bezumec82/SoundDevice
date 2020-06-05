/*
 * $Id: vman-help.h,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#ifndef __VMAN_HELP_H
#define __VMAN_HELP_H

static const char *vman_help_string = "\
VMAN General Help\n\
\n\
Commands are:\n\
\n\
--list                      display all found DSPs in system\n\
-----------------------------------------------------------------------------\n\
--install                   install codec on DSP\n\
 <dsp> <codec>\n\
-----------------------------------------------------------------------------\n\
--uninstall                 uninstall codec\n\
 <dsp>\n\
-----------------------------------------------------------------------------\n\
--record                    record a sound file\n\
 <codec> <chn>              note: if no filename given, writes data to stdout\n\
 [file] [size]\n\
-----------------------------------------------------------------------------\n\
--play <codec> <chn> [file] play a sound file\n\
                            note: if no filename give, reads data from stdin\n\
-----------------------------------------------------------------------------\n\
--upload                    upload DSP executable image into DSP\n\
 <dsp> <file>\n\
-----------------------------------------------------------------------------\n\
--clock                     set framers synchronization source\n\
 <src>                      note: see below for available sources\n\
-----------------------------------------------------------------------------\n\
--connect                   connect channel <dest> with <src>\n\
 <dest> <src>\n\
-----------------------------------------------------------------------------\n\
--disconnect\n\
 <chn>                      disconnect channel <chn>\n\
-----------------------------------------------------------------------------\n\
--clear\n\
 {local, backplane, all}    clear mux field (disconnect all channels)\n\
-----------------------------------------------------------------------------\n\
--mrc                       read mux command memory\n\
 <chn>\n\
-----------------------------------------------------------------------------\n\
--mrd                       read mux data memory\n\
 <chn>\n\
-----------------------------------------------------------------------------\n\
--mrx                       read tdm data from mux on channel <chn>\n\
 <chn>\n\
-----------------------------------------------------------------------------\n\
--mtx                       write <data> in mux in msg mode on channel <chn>\n\
 <chn> <data>\n\
-----------------------------------------------------------------------------\n\
--write                     write DSP memory\n\
 <dsp> <addr> <data>\n\
-----------------------------------------------------------------------------\n\
--read                      read DSP memory\n\
 <dsp> <addr>\n\
-----------------------------------------------------------------------------\n\
--dump                      display dump of DSP memory\n\
 <dsp> <addr> [size] [ovl]\n\
-----------------------------------------------------------------------------\n\
--dtmf                      displays DTMF events on channel <chn>\n\
 <chn>\n\
-----------------------------------------------------------------------------\n\
--rtp                       start rtp session\n\
 <payload> <dest>           dest - <target addr>/<target port>/<source port>\n\
 <channel> <display name>   if target port is not specified,\n\
                            source port number will be used\n\
-----------------------------------------------------------------------------\n\
--muxlist                   list mux devices\n\
-----------------------------------------------------------------------------\n\
--muxmode                   add node to conference\n\
 <dsp> <conf>               where fl1, fl2, fl3 can be one of the following:\n\
 [fl1] [fl2] [fl3]          \"echo\", \"lms\", \"suppress\"\n\
                            echo      - turn on echo canceller\n\
                            lms       - turn on lms\n\
                            suppress  - turn on echo suppress\n\
-----------------------------------------------------------------------------\n\
--muxadd                    add node to conference\n\
 <dsp> <conf> <node> <gain>\n\
-----------------------------------------------------------------------------\n\
--muxremove                 remove node from conference\n\
 <dsp> <conf> <node>\n\
-----------------------------------------------------------------------------\n\
--muxclear                  clear conference\n\
 <dsp> <conf>\n\
-----------------------------------------------------------------------------\n\
--sysclk                    show sysclk status\n\
-----------------------------------------------------------------------------\n\
--dumpcm                    dump connection memory. by default it dumps only\n\
   [-m0] [-m1] [-a]	        active channels, inactive ones can be inspected\n\
                            using -a key. -m1/-m0 keys control display of\n\
                            channels in message mode.\n\
-----------------------------------------------------------------------------\n\
--enable-panic              kernel panic on fatal driver errors (default)\n\
--disable-panic             \n\
-----------------------------------------------------------------------------\n\
--help                        show this information\n\
\n\
";

#endif
