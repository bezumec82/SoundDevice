#include "mcasp_class.h"

#define NEG_MASK(size,off)				negMask<size, off>::neg_mask
#define POS_MASK(size,off)				posMask<size, off>::pos_mask
#define McASP_BITS(__size, __off)	.size = __size, .off = __off, .bits_val = 0, .neg_mask = NEG_MASK(__size,__off), .pos_mask = POS_MASK(__size,__off),

std::unordered_map < std::string, McASP_reg_file > McASP::McASP_regs = {
		{"REV", 		{ .file = NULL, .permission = "r", .reg_val = 0, .bits =
									{
										{	"REV",		{ McASP_BITS(32 ,0 ) .msg = "Identifies revision of peripheral" ,},},
									},
						},
		},
		{"PWRIDLESYSCONFIG", 
						{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"IDLEMODE",	{ McASP_BITS(2 ,0 ) .msg = "Power management Configuration"	,},},
									},
						},
		},
		{"PFUNC", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"AXR",		{ McASP_BITS(4 ,0 ) .msg = "AXRn   function (McASP(0)/GPIO(1))"	,},},
										{	"AMUTE",	{ McASP_BITS(1 ,25) .msg = "AMUTE  function (McASP(0)/GPIO(1))"	,},},
										{	"ACLKX",	{ McASP_BITS(1 ,26) .msg = "ACLKX  function (McASP(0)/GPIO(1))"	,},},
										{	"AHCLKX",	{ McASP_BITS(1 ,27) .msg = "AHCLKX function (McASP(0)/GPIO(1))"	,},},
										{	"AFSX",		{ McASP_BITS(1 ,28) .msg = "AFSX   function (McASP(0)/GPIO(1))"	,},},
										{	"ACLKR",	{ McASP_BITS(1 ,29) .msg = "ACLKR  function (McASP(0)/GPIO(1))"	,},},
										{	"AHCLKR",	{ McASP_BITS(1 ,30) .msg = "AHCLKR function (McASP(0)/GPIO(1))"	,},},
										{	"AFSR",		{ McASP_BITS(1 ,31) .msg = "AFSR   function (McASP(0)/GPIO(1))"	,},},
									},
						},
		},
		/* The pin direction register (PDIR) specifies the direction of AXRn, ACLKX, AHCLKX, AFSX, ACLKR, AHCLKR, and AFSR pins as either an input or an output pin. 
		 * Regardless of the pin function register (PFUNC) setting, 
		 * each PDIR bit must be set to 1 for the specified pin to be enabled as an output 
		 * and each PDIR bit must be cleared to 0 for the specified pin to be an input.*/
		{"PDIR", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"AXR",		{ McASP_BITS(4 ,0 ) .msg = "AXRn   direction (input(0)/output(1))"	,},},
										{	"AMUTE",	{ McASP_BITS(1 ,25) .msg = "AMUTE  direction (input(0)/output(1))"	,},},
										{	"ACLKX",	{ McASP_BITS(1 ,26) .msg = "ACLKX  direction (input(0)/output(1))"	,},},
										{	"AHCLKX",	{ McASP_BITS(1 ,27) .msg = "AHCLKX direction (input(0)/output(1))"	,},},
										{	"AFSX",		{ McASP_BITS(1 ,28) .msg = "AFSX   direction (input(0)/output(1))"	,},},
										{	"ACLKR",	{ McASP_BITS(1 ,29) .msg = "ACLKR  direction (input(0)/output(1))"	,},},
										{	"AHCLKR",	{ McASP_BITS(1 ,30) .msg = "AHCLKR direction (input(0)/output(1))"	,},},
										{	"AFSR",		{ McASP_BITS(1 ,31) .msg = "AFSR   direction (input(0)/output(1))"	,},},
									},
						},
		},
		/* The pin data output register (PDOUT) holds a value for data out at all times, and may be read back at all times. */
		{"PDOUT", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"AXR",		{ McASP_BITS(4 ,0 ) .msg = "AXRn   is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
										{	"AMUTE",	{ McASP_BITS(1 ,25) .msg = "AMUTE  is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
										{	"ACLKX",	{ McASP_BITS(1 ,26) .msg = "ACLKX  is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
										{	"AHCLKX",	{ McASP_BITS(1 ,27) .msg = "AHCLKX is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
										{	"AFSX",		{ McASP_BITS(1 ,28) .msg = "AFSX   is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
										{	"ACLKR",	{ McASP_BITS(1 ,29) .msg = "ACLKR  is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
										{	"AHCLKR",	{ McASP_BITS(1 ,30) .msg = "AHCLKR is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
										{	"AFSR",		{ McASP_BITS(1 ,31) .msg = "AFSR   is GPIO, DIR is OUT, state (low(0)/high(1))"	,},},
									},
						},
		},
		/* PDIN allows the actual value of the pin to be read, regardless of the state of PFUNC and PDIR. */
		{"PDIN", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"AXR",		{ McASP_BITS(4 ,0 ) .msg = "AXRn   state (low(0)/high(1))"	,},},
										{	"AMUTE",	{ McASP_BITS(1 ,25) .msg = "AMUTE  state (low(0)/high(1))"	,},},
										{	"ACLKX",	{ McASP_BITS(1 ,26) .msg = "ACLKX  state (low(0)/high(1))"	,},},
										{	"AHCLKX",	{ McASP_BITS(1 ,27) .msg = "AHCLKX state (low(0)/high(1))"	,},},
										{	"AFSX",		{ McASP_BITS(1 ,28) .msg = "AFSX   state (low(0)/high(1))"	,},},
										{	"ACLKR",	{ McASP_BITS(1 ,29) .msg = "ACLKR  state (low(0)/high(1))"	,},},
										{	"AHCLKR",	{ McASP_BITS(1 ,30) .msg = "AHCLKR state (low(0)/high(1))"	,},},
										{	"AFSR",		{ McASP_BITS(1 ,31) .msg = "AFSR   state (low(0)/high(1))"	,},},
									},
						},
		},
		/* The pin data clear register (PDCLR) is an alias of the pin data output register (PDOUT) for writes only. */
		{"PDCLR", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"AXR",		{ McASP_BITS(4 ,0 ) .msg = "AXRn   PDOUT bit clear(1)"	,},},
										{	"AMUTE",	{ McASP_BITS(1 ,25) .msg = "AMUTE  PDOUT bit clear(1)"	,},},
										{	"ACLKX",	{ McASP_BITS(1 ,26) .msg = "ACLKX  PDOUT bit clear(1)"	,},},
										{	"AHCLKX",	{ McASP_BITS(1 ,27) .msg = "AHCLKX PDOUT bit clear(1)"	,},},
										{	"AFSX",		{ McASP_BITS(1 ,28) .msg = "AFSX   PDOUT bit clear(1)"	,},},
										{	"ACLKR",	{ McASP_BITS(1 ,29) .msg = "ACLKR  PDOUT bit clear(1)"	,},},
										{	"AHCLKR",	{ McASP_BITS(1 ,30) .msg = "AHCLKR PDOUT bit clear(1)"	,},},
										{	"AFSR",		{ McASP_BITS(1 ,31) .msg = "AFSR   PDOUT bit clear(1)"	,},},
									},
						},
		},
		
		/* The global control register (GBLCTL) provides initialization of the transmit and receive sections. */
		{"GBLCTL", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RCLKRST",	{ McASP_BITS(1 ,0 ) .msg = "Receive clock divider reset (reset(0))"					,},},
										{	"RHCLKRST",	{ McASP_BITS(1 ,1 ) .msg = "Receive high-frequency clock divider reset (reset(0))"	,},},
										{	"RSRCLR",	{ McASP_BITS(1 ,2 ) .msg = "Receive serializer clear (reset(0))"					,},},
										{	"RSMRST",	{ McASP_BITS(1 ,3 ) .msg = "Receive state machine reset (reset(0))"					,},},
										{	"RFRST",	{ McASP_BITS(1 ,4 ) .msg = "Receive frame sync generator reset (reset(0))"			,},},
										{	"XCLKRST",	{ McASP_BITS(1 ,8 ) .msg = "Transmit clock divider reset (reset(0))"				,},},
										{	"XHCLKRST",	{ McASP_BITS(1 ,9 ) .msg = "Transmit high-frequency clock divider reset (reset(0))"	,},},
										{	"XSRCLR",	{ McASP_BITS(1 ,10) .msg = "Transmit serializer clear (reset(0))"					,},},
										{	"XSMRST",	{ McASP_BITS(1 ,11) .msg = "Transmit state machine reset (reset(0))"				,},},
										{	"XFRST",	{ McASP_BITS(1 ,12) .msg = "Transmit frame sync generator reset (reset(0))"			,},},
									},
						},
		},
		/* Alias of the global control register (GBLCTL). 
		* RGBLCTL allows the receiver to be reset independently from the transmitter. */
		{"RGBLCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RCLKRST",	{ McASP_BITS(1 ,0 ) .msg = "Receive clock divider reset (reset(0))"	                ,},},
										{	"RHCLKRST",	{ McASP_BITS(1 ,1 ) .msg = "Receive high-frequency clock divider reset (reset(0))"	,},},
										{	"RSRCLR",	{ McASP_BITS(1 ,2 ) .msg = "Receive serializer clear (reset(0))"					,},},
										{	"RSMRST",	{ McASP_BITS(1 ,3 ) .msg = "Receive state machine reset (reset(0))"					,},},
										{	"RFRST",	{ McASP_BITS(1 ,4 ) .msg = "Receive frame sync generator reset (reset(0))"			,},},
										{	"XCLKRST",	{ McASP_BITS(1 ,8 ) .msg = "Transmit clock divider reset (reset(0))"				,},},
										{	"XHCLKRST",	{ McASP_BITS(1 ,9 ) .msg = "Transmit high-frequency clock divider reset (reset(0))"	,},},
										{	"XSRCLR",	{ McASP_BITS(1 ,10) .msg = "Transmit serializer clear (reset(0))"					,},},
										{	"XSMRST",	{ McASP_BITS(1 ,11) .msg = "Transmit state machine reset (reset(0))"				,},},
										{	"XFRST",	{ McASP_BITS(1 ,12) .msg = "Transmit frame sync generator reset (reset(0))"			,},},
									},
						},
		},
		/* Alias of the global control register (GBLCTL). 
		* XGBLCTL allows the transmitter to be reset independently from the receiver. */
		{"XGBLCTL", 	{ .file = NULL, .permission = "r", .reg_val = 0, .bits =
									{
										{	"RCLKRST",	{ McASP_BITS(1 ,0 ) .msg = "Receive clock divider reset (reset(0))"					,},},
										{	"RHCLKRST",	{ McASP_BITS(1 ,1 ) .msg = "Receive high-frequency clock divider reset (reset(0))"	,},},
										{	"RSRCLR",	{ McASP_BITS(1 ,2 ) .msg = "Receive serializer clear (reset(0))"					,},},
										{	"RSMRST",	{ McASP_BITS(1 ,3 ) .msg = "Receive state machine reset (reset(0))"					,},},
										{	"RFRST",	{ McASP_BITS(1 ,4 ) .msg = "Receive frame sync generator reset (reset(0))"			,},},
										{	"XCLKRST",	{ McASP_BITS(1 ,8 ) .msg = "Transmit clock divider reset (reset(0))"				,},},
										{	"XHCLKRST",	{ McASP_BITS(1 ,9 ) .msg = "Transmit high-frequency clock divider reset (reset(0))"	,},},
										{	"XSRCLR",	{ McASP_BITS(1 ,10) .msg = "Transmit serializer clear (reset(0))"					,},},
										{	"XSMRST",	{ McASP_BITS(1 ,11) .msg = "Transmit state machine reset (reset(0))"				,},},
										{	"XFRST",	{ McASP_BITS(1 ,12) .msg = "Transmit frame sync generator reset (reset(0))"			,},},
									},
						},
		},
		
		
		/* The digital loopback control register. */
		{"DLBCTL", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"DLBEN",	{ McASP_BITS(1 ,0 ) .msg = "Loopback mode (en(1)/dis(1))"	,},},
										{	"ORD",		{ McASP_BITS(1 ,1 ) .msg = "Loopback order (odd(0)/even(1))",},},
										{	"MODE",		{ McASP_BITS(1 ,2 ) .msg = "Loopback generator mode bits."	,},},
									},
						},
		},
		
		
		/* The receive format unit bit mask register. */
		{"RMASK", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RMASK",	{ McASP_BITS(32 ,0 ) .msg = "Receive data mask"	,},},
									},
						},
		},
		/* The transmit format unit bit mask register. */
		{"XMASK", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XMASK",	{ McASP_BITS(32 ,0 ) .msg = "Transmit data mask"	,},},
									},
						},
		},
		
		
		/* The receive bit stream format register. */
		{"RFMT", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RROT",		{ McASP_BITS(3 ,0 ) .msg = "Right-rotation value (RX)"				,},},
										{	"RBUSEL",	{ McASP_BITS(1 ,3 ) .msg = "Serializer read origin (CFG(1)/DAT(0))" ,},},
										{	"RSSZ",		{ McASP_BITS(4 ,4 ) .msg = "Receive slot size"						,},},
										{	"RPBIT",	{ McASP_BITS(5 ,8 ) .msg = "Pad value (RX)"							,},},
										{	"RPAD",		{ McASP_BITS(2 ,13) .msg = "Pad value for extra bits in slot not belonging to the word"	,},},
										{	"RRVRS",	{ McASP_BITS(1 ,15) .msg = "Receive serial bitstream order (LSB(0)/MSB(1))"				,},},
										{	"RDATDLY",	{ McASP_BITS(2 ,16) .msg = "Receive bit delay"											,},},
									},
						},
		},
		/* The transmit bit stream format register. */
		{"XFMT", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XROT",		{ McASP_BITS(3 ,0 ) .msg = "Right-rotation value (TX)"				,},},
										{	"XBUSEL",	{ McASP_BITS(1 ,3 ) .msg = "Serializer write origin (CFG(1)/DAT(0))",},},
										{	"XSSZ",		{ McASP_BITS(4 ,4 ) .msg = "Transmit slot size"						,},},
										{	"XPBIT",	{ McASP_BITS(5 ,8 ) .msg = "Pad value (TX)"							,},},
										{	"XPAD",		{ McASP_BITS(2 ,13) .msg = "Pad value for extra bits in slot not belonging to word defined by XMASK",},},
										{	"XRVRS",	{ McASP_BITS(1 ,15) .msg = "Transmit serial bitstream order (LSB(0)/MSB(1))"						,},},
										{	"XDATDLY",	{ McASP_BITS(2 ,16) .msg = "Transmit sync bit delay"												,},},
									},
						},
		},
		
		
		/* The receive frame sync control register. */
		{"AFSRCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"FSRP",		{ McASP_BITS(1 ,0 ) .msg = "Receive frame sync polarity"	,},},
										{	"FSRM",		{ McASP_BITS(1 ,1 ) .msg = "Receive frame sync generation"	,},},
										{	"FRWID",	{ McASP_BITS(1 ,4 ) .msg = "Receive frame sync width"		,},},
										{	"RMOD",		{ McASP_BITS(9 ,7 ) .msg = "Receive frame sync mode"		,},},
									},
						},
		},
		/* The transmit frame sync control register. */
		{"AFSXCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"FSXP",		{ McASP_BITS(1 ,0 ) .msg = "Transmit frame sync polarity"	,},},
										{	"FSXM",		{ McASP_BITS(1 ,1 ) .msg = "Transmit frame sync generation"	,},},
										{	"FXWID",	{ McASP_BITS(1 ,4 ) .msg = "Transmit frame sync width"		,},},
										{	"XMOD",		{ McASP_BITS(9 ,7 ) .msg = "Transmit frame sync mode"		,},},
									},
						},
		},
		
		
		/* The receive clock control register. */
		{"ACLKRCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"CLKRDIV",	{ McASP_BITS(5 ,0 ) .msg = "Receive bit clock divide ratio"			,},},
										{	"CLKRM",	{ McASP_BITS(1 ,5 ) .msg = "Receive bit clock source"				,},},
										{	"CLKRP",	{ McASP_BITS(1 ,7 ) .msg = "Receive bitstream clock polarity select",},},
									},
						},
		},
		/* The transmit clock control register. */
		{"ACLKXCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"CLKXDIV",	{ McASP_BITS(5 ,0 ) .msg = "Transmit bit clock divide ratio"					,},},
										{	"CLKXM",	{ McASP_BITS(1 ,5 ) .msg = "Transmit bit clock source"							,},},
										{	"ASYNC",	{ McASP_BITS(1 ,6 ) .msg = "Transmit/receive async. operation (en(1)/dis(0))"	,},},
										{	"CLKXP",	{ McASP_BITS(1 ,7 ) .msg = "Transmit bitstream clock polarity"					,},},
									},
						},
		},
		
		
		/* The receive high-frequency clock control register. */
		{"AHCLKRCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"HCLKRDIV",	{ McASP_BITS(11,0 ) .msg = "Receive high-frequency clock divide ratio"			,},},
										{	"HCLKRP",	{ McASP_BITS(1 ,14) .msg = "Receive bitstream high-frequency clock polarity"	,},},
										{	"HCLKRM",	{ McASP_BITS(1 ,15) .msg = "Receive high-frequency clock source"				,},},
									},
						},
		},
		/* The receive high-frequency clock control register. */
		{"AHCLKXCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"HCLKXDIV",	{ McASP_BITS(11,0 ) .msg = "Transmit high-frequency clock divide ratio"			,},},
										{	"HCLKXP",	{ McASP_BITS(1 ,14) .msg = "Transmit bitstream high-frequency clock polarity"	,},},
										{	"HCLKXM",	{ McASP_BITS(1 ,15) .msg = "Transmit high-frequency clock source"				,},},
									},
						},
		},
		/* The receive TDM time slot register (RTDM) specifies which TDM time slot the receiver is active. */
		{"RTDM", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RTDMS",	{ McASP_BITS(32,0 ) .msg = "Receiver mode during TDM time slot 'n'"		,},},
									},
						},
		},
		/* The transmit TDM time slot register (XTDM) specifies in which TDM time slot the transmitter is active. */
		{"XTDM", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XTDMS",	{ McASP_BITS(32,0 ) .msg = "Transmitter mode during TDM time slot 'n'"	,},},
									},
						},
		},
		/* The receiver interrupt control register. */
		{"RINTCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"ROVRN",	{ McASP_BITS(1, 0 ) .msg = "Receiver overrun interrupt (en(1)/dis(0))"				,},},
										{	"RSYNCERR",	{ McASP_BITS(1 ,1 ) .msg = "Unexpected receive frame sync interrupt (en(1)/dis(0))"	,},},
										{	"RCKFAIL",	{ McASP_BITS(1 ,2 ) .msg = "Receive clock failure interrupt (en(1)/dis(0))"			,},},
										{	"RDMAERR",	{ McASP_BITS(1 ,3 ) .msg = "Receive DMA error interrupt (en(1)/dis(0))"				,},},
										{	"RLAST",	{ McASP_BITS(1 ,4 ) .msg = "Receive last slot interrupt (en(1)/dis(0))"				,},},
										{	"RDATA",	{ McASP_BITS(1 ,5 ) .msg = "Receive data ready interrupt (en(1)/dis(0))"			,},},
										{	"RSTAFRM",	{ McASP_BITS(1 ,7 ) .msg = "Receive start of frame interrupt (en(1)/dis(0))"		,},},
									},
						},
		},
		/* The transmitter interrupt control register. */
		{"XINTCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XUNDRN",	{ McASP_BITS(1 ,0 ) .msg = "Transmitter underrun interrupt (en(1)/dis(0))"			,},},
										{	"XSYNCERR",	{ McASP_BITS(1 ,1 ) .msg = "Unexpected transmit frame sync interrupt (en(1)/dis(0))",},},
										{	"XCKFAIL",	{ McASP_BITS(1 ,2 ) .msg = "Transmit clock failure interrupt (en(1)/dis(0))"		,},},
										{	"XDMAERR",	{ McASP_BITS(1 ,3 ) .msg = "Transmit DMA error interrupt (en(1)/dis(0))"			,},},
										{	"XLAST",	{ McASP_BITS(1 ,4 ) .msg = "Transmit last slot interrupt (en(1)/dis(0))"			,},},
										{	"XDATA",	{ McASP_BITS(1 ,5 ) .msg = "Transmit data ready interrupt (en(1)/dis(0))"			,},},
										{	"XSTAFRM",	{ McASP_BITS(1 ,7 ) .msg = "Transmit start of frame interrupt (en(1)/dis(0))"		,},},
									},
						},
		},
		
		/* The receiver status register. */
		{"RSTAT", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"ROVRN",	{ McASP_BITS(1 ,0 ) .msg = "Receiver overrun flag"				,},},
										{	"RSYNCERR",	{ McASP_BITS(1 ,1 ) .msg = "Unexpected receive frame sync flag"	,},},
										{	"RCKFAIL",	{ McASP_BITS(1 ,2 ) .msg = "Receive clock failure flag"			,},},
										{	"RTDMSLOT",	{ McASP_BITS(1 ,3 ) .msg = "Returns the LSB of RSLOT"			,},},
										{	"RLAST",	{ McASP_BITS(1 ,4 ) .msg = "Receive last slot flag"				,},},
										{	"RDATA",	{ McASP_BITS(1 ,5 ) .msg = "Receive data ready flag"			,},},
										{	"RSTAFRM",	{ McASP_BITS(1 ,6 ) .msg = "Receive start of frame flag"		,},},
										{	"RDMAERR",	{ McASP_BITS(1 ,7 ) .msg = "Receive DMA error flag"				,},},
										{	"RERR",		{ McASP_BITS(1 ,8 ) .msg = "Logic-OR of: ROVRN | RSYNCERR | RCKFAIL | RDMAERR."	,},},
									},
						},
		},
		/* The transmitter status register. */
		{"XSTAT", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XUNDRN",	{ McASP_BITS(1 ,0 ) .msg = "Transmitter underrun flag"				,},},
										{	"XSYNCERR",	{ McASP_BITS(1 ,1 ) .msg = "Unexpected transmit frame sync flag"	,},},
										{	"XCKFAIL",	{ McASP_BITS(1 ,2 ) .msg = "Transmit clock failure flag"			,},},
										{	"XTDMSLOT",	{ McASP_BITS(1 ,3 ) .msg = "Returns the LSB of XSLOT"				,},},
										{	"XLAST",	{ McASP_BITS(1 ,4 ) .msg = "Transmit last slot flag"				,},},
										{	"XDATA",	{ McASP_BITS(1 ,5 ) .msg = "Transmit data ready flag"				,},},
										{	"XSTAFRM",	{ McASP_BITS(1 ,6 ) .msg = "Transmit start of frame flag"			,},},
										{	"XDMAERR",	{ McASP_BITS(1 ,7 ) .msg = "Transmit DMA error flag"				,},},
										{	"XERR",		{ McASP_BITS(1 ,8 ) .msg = "Logic-OR of: XUNDRN | XSYNCERR | XCKFAIL | XDMAERR.."	,},},
									},
						},
		},
		
		/* Indicates the current time slot for the receive data frame. */
		{"RSLOT", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RSLOTCNT",	{ McASP_BITS(9,0 ) .msg = "Current receive time slot count"		,},},
									},
						},
		},
		/* Indicates the current time slot for the transmit data frame. */
		{"XSLOT", 		{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XSLOTCNT",	{ McASP_BITS(9,0 ) .msg = "Current transmit time slot count"		,},},
									},
						},
		},
		
		/* The receive clock check control register. */
		{"RCLKCHK", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RPS",		{ McASP_BITS(4 ,0 ) .msg = "Receive clock check prescaler value",},},
										{	"RMIN",		{ McASP_BITS(8 ,8 ) .msg = "Receive clock minimum boundary"		,},},
										{	"RMAX",		{ McASP_BITS(8 ,16) .msg = "Receive clock maximum boundary"		,},},
										{	"RCNT",		{ McASP_BITS(8 ,24) .msg = "Receive clock count value"			,},},
									},
						},
		},
		/* The receive clock check control register. */
		{"XCLKCHK", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XPS",		{ McASP_BITS(4 ,0 ) .msg = "Transmit clock check prescaler value"	,},},
										{	"XMIN",		{ McASP_BITS(8 ,8 ) .msg = "Transmit clock minimum boundary"		,},},
										{	"XMAX",		{ McASP_BITS(8 ,16) .msg = "Transmit clock maximum boundary"		,},},
										{	"XCNT",		{ McASP_BITS(8 ,24) .msg = "Transmit clock count valu."				,},},
									},
						},
		},
		
		
		{"REVTCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RDATDMA",	{ McASP_BITS(1 ,0 ) .msg = "Receive data DMA request (en(1)/dis(0))"	,},},
									},
						},
		},
		{"XEVTCTL", 	{ .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XDATDMA",	{ McASP_BITS(1 ,0 ) .msg = "Transmit data DMA request (en(1)/dis(0))"	,},},
									},
						},
		},
		/* Each serializer on McASP has a serializer control register. */
		{"SRCTL_0", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"SRMOD",	{ McASP_BITS(2 ,0 ) .msg = "Serializer 0 mode"						,},},
										{	"DISMOD",	{ McASP_BITS(2 ,2 ) .msg = "Serializer 0 pin drive mode"			,},},
										{	"XRDY",		{ McASP_BITS(1 ,4 ) .msg = "Serializer 0 transmit buffer ready"		,},},
										{	"RRDY",		{ McASP_BITS(1 ,5 ) .msg = "Serializer 0 receive buffer ready"		,},},
									},
						},
		},
		{"SRCTL_1", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"SRMOD",	{ McASP_BITS(2 ,0 ) .msg = "Serializer 1 mode"					,},},
										{	"DISMOD",	{ McASP_BITS(2 ,2 ) .msg = "Serializer 1 pin drive mode"		,},},
										{	"XRDY",		{ McASP_BITS(1 ,4 ) .msg = "Serializer 1 transmit buffer ready"	,},},
										{	"RRDY",		{ McASP_BITS(1 ,5 ) .msg = "Serializer 1 receive buffer ready"	,},},
									},
						},
		},
		{"SRCTL_2", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"SRMOD",	{ McASP_BITS(2 ,0 ) .msg = "Serializer 2 mode"					,},},
										{	"DISMOD",	{ McASP_BITS(2 ,2 ) .msg = "Serializer 2 pin drive mode"		,},},
										{	"XRDY",		{ McASP_BITS(1 ,4 ) .msg = "Serializer 2 transmit buffer ready"	,},},
										{	"RRDY",		{ McASP_BITS(1 ,5 ) .msg = "Serializer 2 receive buffer ready"	,},},
									},
						},
		},
		{"SRCTL_3", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"SRMOD",	{ McASP_BITS(2 ,0 ) .msg = "Serializer 3 mode"					,},},
										{	"DISMOD",	{ McASP_BITS(2 ,2 ) .msg = "Serializer 3 pin drive mode"		,},},
										{	"XRDY",		{ McASP_BITS(1 ,4 ) .msg = "Serializer 3 Transmit buffer ready"	,},},
										{	"RRDY",		{ McASP_BITS(1 ,5 ) .msg = "Serializer 3 Receive buffer ready"	,},},
									},
						},
		},
		{"SRCTL_4", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"SRMOD",	{ McASP_BITS(2 ,0 ) .msg = "Serializer 4 mode"					,},},
										{	"DISMOD",	{ McASP_BITS(2 ,2 ) .msg = "Serializer 4 pin drive mode"		,},},
										{	"XRDY",		{ McASP_BITS(1 ,4 ) .msg = "Serializer 4 Transmit buffer ready"	,},},
										{	"RRDY",		{ McASP_BITS(1 ,5 ) .msg = "Serializer 4 Receive buffer ready"	,},},
									},
						},
		},
		{"SRCTL_5", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"SRMOD",	{ McASP_BITS(2 ,0 ) .msg = "Serializer 5 mode"					,},},
										{	"DISMOD",	{ McASP_BITS(2 ,2 ) .msg = "Serializer 5 pin drive mode"		,},},
										{	"XRDY",		{ McASP_BITS(1 ,4 ) .msg = "Serializer 5 transmit buffer ready"	,},},
										{	"RRDY",		{ McASP_BITS(1 ,5 ) .msg = "Serializer 5 receive buffer ready"	,},},
									},
						},
		},
		/* The transmit buffers for the serializers. */
#if (CFG_BUS)
		{"XBUF_0", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XBUF",	{ McASP_BITS(32 ,0 ) .msg = "Transmit buffers for serializer 0"	,},},
									},
						},
		},
		{"XBUF_1", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XBUF",	{ McASP_BITS(32 ,0 ) .msg = "Transmit buffers for serializer 1"	,},},
									},
						},
		},
		{"XBUF_2", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XBUF",	{ McASP_BITS(32 ,0 ) .msg = "Transmit buffers for serializer 2"	,},},
									},
						},
		},
		{"XBUF_3", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XBUF",	{ McASP_BITS(32 ,0 ) .msg = "Transmit buffers for serializer 3"	,},},
									},
						},
		},
		{"XBUF_4", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XBUF",	{ McASP_BITS(32 ,0 ) .msg = "Transmit buffers for serializer 4"	,},},
									},
						},
		},
		{"XBUF_5", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"XBUF",	{ McASP_BITS(32 ,0 ) .msg = "Transmit buffers for serializer 5"	,},},
									},
						},
		},
		/* The receive buffers for the serializers. */
		{"RBUF_0", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RBUF",	{ McASP_BITS(32 ,0 ) .msg = "Receive buffers for serializer 0"	,},},
									},
						},
		},
		{"RBUF_1", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RBUF",	{ McASP_BITS(32 ,0 ) .msg = "Receive buffers for serializer 1"	,},},
									},
						},
		},
		{"RBUF_2", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RBUF",	{ McASP_BITS(32 ,0 ) .msg = "Receive buffers for serializer 2"	,},},
									},
						},
		},
		{"RBUF_3", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RBUF",	{ McASP_BITS(32 ,0 ) .msg = "Receive buffers for serializer 3"	,},},
									},
						},
		},
		{"RBUF_4", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RBUF",	{ McASP_BITS(32 ,0 ) .msg = "Receive buffers for serializer 4"	,},},
									},
						},
		},
		{"RBUF_5", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RBUF",	{ McASP_BITS(32 ,0 ) .msg = "Receive buffers for serializer 5"	,},},
									},
						},
		},
#endif
		{"WFIFOCTL", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"WNUMDMA",	{ McASP_BITS(8 ,0 ) .msg = "Write word count per transfer"	,},},
										{	"WNUMEVT",	{ McASP_BITS(8 ,8 ) .msg = "Write word count per DMA event"	,},},
										{	"WENA",		{ McASP_BITS(1 ,16) .msg = "Write FIFO (en(1)/dis(0))"		,},},
									},
						},
		},
		{"RFIFOCTL", { .file = NULL, .permission = "w+", .reg_val = 0, .bits =
									{
										{	"RNUMDMA",	{ McASP_BITS(8 ,0 ) .msg = "Read word count per transfer"	,},},
										{	"RNUMEVT",	{ McASP_BITS(8 ,8 ) .msg = "Read word count per DMA event"	,},},
										{	"RENA",		{ McASP_BITS(1 ,16) .msg = "Read FIFO (en(1)/dis(0))"		,},},
									},
						},
		},
		{"WFIFOSTS", { .file = NULL, .permission = "r", .reg_val = 0, .bits =
									{
										{	"WLVL",	{ McASP_BITS(8 ,0 ) .msg = "Write level"	,},},
									},
						},
		},
		{"RFIFOSTS", { .file = NULL, .permission = "r", .reg_val = 0, .bits =
									{
										{	"RLVL",	{ McASP_BITS(8 ,0 ) .msg = "Read level"		,},},
									},
						},
		},
};
/*--- End of member ------------------------------------------------------------------------------*/

/*! Constructor.
 * @param controller_num : The number of McASP controller : 0 or 1.
 */
McASP::McASP ( uint8_t controller_num ) : reg_path("/dev/null")
{
	if		(controller_num == 0)	{	reg_path = sysfs_pth + McASP0_dir + std::string("/regs/"); }
	else if	(controller_num == 1)	{	reg_path = sysfs_pth + McASP1_dir + std::string("/regs/"); }
	else return;

	for (auto file_iter : this->McASP_regs)
	{
		std::string reg_file = reg_path + file_iter.first;
#if DEBUG_McASP
		PRINTF(MAG, "Opening file : %s.\n", reg_file.c_str());
#endif
		this->McASP_regs[file_iter.first].file = fopen(reg_file.c_str(), this->McASP_regs[file_iter.first].permission.c_str() );
		if (!this->McASP_regs[file_iter.first].file)
		{
			FPRINTF_EXT("Can't open file : %s.\n", reg_file.c_str());
		}
	} //end for (auto file_iter
}
/*--- End of member ------------------------------------------------------------------------------*/


/*!
 *
 * @param reg_name	: Name of the register to write.
 * @param reg_val	: Value to write.
 * @return			-1, written bytes or 'errno'.
 */
int McASP::write_reg ( std::string reg_name, uint32_t reg_val )
{
	char buf[32] = {0};

	try
	{
		if (McASP_regs.at(reg_name).permission == "r")
		{
			PRINTF(MAG, "'%s' read only.\n", reg_name.c_str());
			return ( EXIT_SUCCESS );
		}
		if (McASP_regs.at(reg_name).file)
		{
		    sprintf (buf, "%d", reg_val);
#if DEBUG_McASP_SEVERE
		PRINTF(MAG, "Writing %s to the register : %s.\n", buf, reg_name.c_str() );
#endif
			int ret_val = fputs ( buf, McASP_regs.at(reg_name).file );
			if (ret_val < 0) FPRINTF;
			rewind( McASP_regs.at(reg_name).file );
			return ( ret_val );
		} else { PRINT_ERR("File not opened.\n"); return (-1); }
	}
	catch (std::out_of_range&)
	{
		PRINT_ERR("No register : %s.\n", reg_name.c_str());
		return ( -EXIT_FAILURE );
	}
}
/*--- End of member ------------------------------------------------------------------------------*/

int McASP::write_reg_bits ( std::string reg_name, std::string bits_name, uint32_t bits_val )
{
#if SAFE_VECTOR_TRAVERSING
	try
	{
		if (McASP_regs.at(reg_name).permission == "r")
		{
			PRINTF(MAG, "'%s' read only.\n", reg_name.c_str());
			return ( EXIT_SUCCESS );
		}
		uint32_t		reg_val		= McASP_regs.at(reg_name).reg_val;
		uint8_t			off			= McASP_regs.at(reg_name).bits.at(bits_name).off;
		uint32_t		pos_mask	= McASP_regs.at(reg_name).bits.at(bits_name).pos_mask;
		uint32_t		neg_mask	= McASP_regs.at(reg_name).bits.at(bits_name).neg_mask;

		McASP_regs.at(reg_name).bits.at(bits_name).bits_val = bits_val;	///<Store new bits value.
		if (McASP_regs.at(reg_name).permission != "w")					///<Read present register state.
		{
			this->read_reg (reg_name, &reg_val);
		}
		reg_val &=  pos_mask;                       ///<Clear all target bits.
		reg_val |=	neg_mask & ( bits_val << off );	///<Set new bits.
		McASP_regs.at(reg_name).reg_val = reg_val;	///<Save new register state locally.
		write_reg ( reg_name, reg_val );			///<Write new register state.
#if DEBUG_McASP_SEVERE
			PRINTF(NORM, "Registet %s is set to : 0x%x.\n",
					reg_name.c_str(),
					McASP_regs.at(reg_name).reg_val);
			std::cout << reg_name.c_str() << " : " << std::bitset<32>(McASP_regs.at(reg_name).reg_val) << std::endl;
//			std::cout << "Neg. mask : " << std::bitset<32>(neg_mask) << std::endl;
//			std::cout << "Pos. mask : " << std::bitset<32>(pos_mask) << std::endl;
#endif
			return ( EXIT_SUCCESS );
	}
	catch (std::out_of_range&)
	{
		PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
		return ( -EXIT_FAILURE );
	}
#else

#endif
}

/*--- End of member ------------------------------------------------------------------------------*/

/*!
 * @param reg_name	: Name of the register to read.
 * @param reg_val	: Return parameter - read value.
 * @return			0, -1 or 'errno'.
 */
int McASP::read_reg (const std::string & reg_name, uint32_t * reg_val)
{
	try
	{
	    char buf[32] = {0};
	    uint32_t ret_reg_val = 0;
		if (McASP_regs.at(reg_name).permission == "w")
		{
			PRINTF(MAG, "'%s' write only.\n", reg_name.c_str());
			ret_reg_val = McASP_regs.at(reg_name).reg_val;
			if (reg_val!=NULL) { *reg_val = ret_reg_val; }
			return ( EXIT_SUCCESS );
		}
		if (!McASP_regs.at(reg_name).file) { PRINT_ERR("File not opened.\n"); *reg_val = 0; return (-1); }

		char * ret_val = fgets(buf, sizeof(buf), McASP_regs.at(reg_name).file); ///<Read from register file.
		if (!ret_val)
		{
			FPRINTF;
			if (reg_val!=NULL) { *reg_val = ret_reg_val; }
			return ( -EXIT_FAILURE );
		};

		ret_reg_val = strtol(buf, NULL, 10);			///<Convert std::string to number.
		McASP_regs.at(reg_name).reg_val = ret_reg_val;	///<Save new register state locally.
		rewind(McASP_regs.at(reg_name).file);		    ///<Rewind file.

#if DEBUG_READ_REG
        PRINTF(MAG, "Value of the register %s : 0x%x.\n", reg_name.c_str(), (unsigned int)ret_reg_val );
        std::cout << reg_name.c_str() << " : " << std::bitset<32>(ret_reg_val) << std::endl;
#endif
        if (reg_val!=NULL) { *reg_val = ret_reg_val; }
		return ( EXIT_SUCCESS );
	}
	catch (std::out_of_range&)
	{
		PRINT_ERR("No register : %s.\n", reg_name.c_str());
		return ( -EXIT_FAILURE );
	}
}
/*--- End of member ------------------------------------------------------------------------------*/

int McASP::read_reg_bits ( std::string reg_name, std::string bits_name, uint32_t * bits_val )
{
	try
	{
		if (McASP_regs.at(reg_name).permission == "w")
		{
			PRINTF(MAG, "'%s' write only.\n", reg_name.c_str());
			*bits_val = McASP_regs.at(reg_name).bits.at(bits_name).bits_val;
			return ( EXIT_SUCCESS );
		}
		uint32_t		reg_val		= 0;
		uint8_t			off			= McASP_regs.at(reg_name).bits.at(bits_name).off;
		uint32_t		neg_mask	= McASP_regs.at(reg_name).bits.at(bits_name).neg_mask;
		uint32_t		bits_tmp	= 0;

		this->read_reg (reg_name, &reg_val);
		bits_tmp = (reg_val&neg_mask)>>off;
		if (bits_val) { *bits_val = bits_tmp; }
		McASP_regs.at(reg_name).bits.at(bits_name).bits_val = bits_tmp;
		return ( EXIT_SUCCESS );
	}
	catch (std::out_of_range&)
	{
		PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
		return ( -EXIT_FAILURE );
	}
}
/*--- End of member ------------------------------------------------------------------------------*/


uint32_t McASP::show_reg ( std::string reg_name )
{
	try
	{
		return ( McASP_regs.at(reg_name).reg_val );
	}
	catch (std::out_of_range&)
	{
		PRINT_ERR("No register '%s'.\n", reg_name.c_str());
		return ( 0xffffffff );
	}
}

uint32_t McASP::show_reg_bits ( std::string reg_name, std::string bits_name )
{
	try
	{
		return ( McASP_regs.at(reg_name).bits.at(bits_name).bits_val );
	}
	catch (std::out_of_range&)
	{
		PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
		return ( 0xffffffff );
	}
}
/*--- End of member ------------------------------------------------------------------------------*/

void McASP::read_all_regs ( void )
{
	for (auto iter : McASP_regs)
	{
		uint32_t reg_val = 0;
		int ret_val = this->read_reg(iter.first, &reg_val);
		PRINTF(CYN, "Value of the register %-10s : 0x%-8x.\n", iter.first.c_str(), reg_val);
		if (ret_val!=EXIT_SUCCESS) {	FPRINTF_EXT("Can't read register : %s.\n", iter.first.c_str()); }
	}
}
/*--- End of member ------------------------------------------------------------------------------*/

McASP::~McASP ( void )
{
	for (auto iter : McASP_regs)
	{
		std::string reg_file = reg_path + iter.first;
		PRINTF(MAG, "Closing file : %s.\n", reg_file.c_str());
		fclose(McASP_regs[iter.first].file);
	} //end for
}/* end ~McASP */
/*--- End of member ------------------------------------------------------------------------------*/

int McASP::write_data (uint32_t McASP_THR_val)
{
	uint32_t McASP_SR_val	= 0;
	uint32_t TXRDY_val		= 0;
	char McASP_SR_buf[32]	= {0};
	char McASP_THR_buf[32]	= {0};

	if (!McASP_THR_file||!McASP_SR_file)
	{
		PRINT_ERR("Necessary files not opened.\n");
		return ( -1 );
	}
	//Wait for TXRDY bit.
	do {
		char * ret_val = fgets(McASP_SR_buf, sizeof(McASP_SR_buf), McASP_SR_file);	///<Read from status register file.
		if (!ret_val)
		{
			FPRINTF;
			return ( -EXIT_FAILURE );
		};
		McASP_SR_val = strtol(McASP_SR_buf, NULL, 10);	///<Convert std::string to number.
		rewind(McASP_SR_file);	///<Rewind file.
		TXRDY_val = McASP_SR_val&POS_MASK(1,0);
	} while(!TXRDY_val);

	//Write to the THR.
	sprintf (McASP_THR_buf, "%d", McASP_THR_val);
	int ret_val = fputs (McASP_THR_buf, McASP_THR_file);
	rewind(McASP_THR_file);
	return ( ret_val );
}

