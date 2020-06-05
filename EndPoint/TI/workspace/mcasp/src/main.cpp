#include "main.h"

#define REG_W_ERR           PRINT_ERR("Can't change McASP reg. bits.\n");
#define OSC_EN_GPIO_NUM     32+27

int main(/* int argc, char * argv[] */) {
    std::cout << "Starting McASP test." << std::endl;
    Set_GPIO_val(OSC_EN_GPIO_NUM, 1);
    McASP * McASP = new ::McASP(0);
    /** (1) Reset McASP to default values by setting GBLCTL = 0. */
    if (McASP->write_reg("GBLCTL", 0x0 ) < 0) { REG_W_ERR; };
    /** (2) Configure all McASP registers except GBLCTL in the following order: */
    /** (a) Power Idle SYSCONFIG: PWRIDLESYSCONFIG. */
    if (McASP->write_reg_bits("PWRIDLESYSCONFIG", "IDLEMODE", 0x1 ) < 0) { REG_W_ERR; }; /* No-idle mode. */
    /** (b) TO DO: Receive registers: RMASK, RFMT, AFSRCTL, ACLKRCTL, AHCLKRCTL, RTDM, RINTCTL, RCLKCHK. */
    /** (c) Transmit registers: XMASK, XFMT, AFSXCTL, ACLKXCTL, AHCLKXCTL, XTDM, XINTCTL, XCLKCHK. */
    /** (c.1) */
    if (McASP->write_reg("XMASK", 0x0000ffff ) < 0) { REG_W_ERR; };
    /** (c.2) */
    if (McASP->write_reg_bits("XFMT",       "XROT",     0x0     ) < 0) { REG_W_ERR; }; /* Rotate right by 0 (no rotation). */
    if (McASP->write_reg_bits("XFMT",       "XBUSEL",   0x0     ) < 0) { REG_W_ERR; }; /* Transfer through data port. */
    if (McASP->write_reg_bits("XFMT",       "XSSZ",     0xf     ) < 0) { REG_W_ERR; }; /* Slot size is 32 bits. */
    if (McASP->write_reg_bits("XFMT",       "XRVRS",    0x1     ) < 0) { REG_W_ERR; }; /* Bitstream is MSB first. */
    /** (c.3) The transmit frame sync control register. */
    if (McASP->write_reg_bits("AFSXCTL",    "FSXP",     0x1     ) < 0) { REG_W_ERR; }; /* A falling edge on transmit frame sync (AFSX) indicates the beginning of a frame. */
    if (McASP->write_reg_bits("AFSXCTL",    "FSXM",     0x1     ) < 0) { REG_W_ERR; }; /* Internally-generated transmit frame sync. */
    if (McASP->write_reg_bits("AFSXCTL",    "FXWID",    0x1     ) < 0) { REG_W_ERR; }; /* Word wide. */
    if (McASP->write_reg_bits("AFSXCTL",    "XMOD",     0x2     ) < 0) { REG_W_ERR; }; /* 2-slot TDM (I2S mode). */
    /** (c.4) The transmit clock control register. */
    if (McASP->write_reg_bits("ACLKXCTL",   "CLKXDIV",  24-1    ) < 0) { REG_W_ERR; }; /* Divide. */
    if (McASP->write_reg_bits("ACLKXCTL",   "CLKXM",    0x1     ) < 0) { REG_W_ERR; }; /* Internal. */
    if (McASP->write_reg_bits("ACLKXCTL",   "ASYNC",    0x1     ) < 0) { REG_W_ERR; }; /* */
    if (McASP->write_reg_bits("ACLKXCTL",   "CLKXP",    0x0     ) < 0) { REG_W_ERR; }; /* Rising edge. */
    /** (c.5)  The transmit high-frequency clock control register. */
    if (McASP->write_reg_bits("AHCLKXCTL",  "HCLKXDIV", 0       ) < 0) { REG_W_ERR; }; /* Divide. */
    if (McASP->write_reg_bits("AHCLKXCTL",  "HCLKXP",   0x0     ) < 0) { REG_W_ERR; }; /* AHCLKX is not inverted. */
    if (McASP->write_reg_bits("AHCLKXCTL",  "HCLKXM",   0x0     ) < 0) { REG_W_ERR; }; /* External. */
    /** (c.6) */
    if (McASP->write_reg_bits("XTDM",       "XTDMS",    0b11    ) < 0) { REG_W_ERR; }; /* Two slots are active. */
    /** (c.7) */
    if (McASP->write_reg("XINTCTL", 0x0 ) < 0) { REG_W_ERR; }; /* All interrupts disabled. */
    /** (c.8) The transmit clock check control register. */
    if (McASP->write_reg_bits("XCLKCHK",    "XMIN",     0x0     ) < 0) { REG_W_ERR; }; /* Transmit clock minimum boundary. */
    if (McASP->write_reg_bits("XCLKCHK",    "XMAX",     0xff    ) < 0) { REG_W_ERR; }; /* Transmit clock maximum boundary. */
    /** (d) Serializer registers: SRCTL[n]. */
    /* AXR0 as output. AXR1 as input. */
    if (McASP->write_reg_bits("SRCTL_0",    "SRMOD",    0x1 ) < 0) { REG_W_ERR; }; /* Serializer is transmitter. */
    if (McASP->write_reg_bits("SRCTL_1",    "SRMOD",    0x2 ) < 0) { REG_W_ERR; }; /* Serializer is receiver. */
    if (McASP->write_reg_bits("SRCTL_0",    "DISMOD",   0x0 ) < 0) { REG_W_ERR; }; /* Drive on pin is 3-state. */
    if (McASP->write_reg_bits("SRCTL_1",    "DISMOD",   0x0 ) < 0) { REG_W_ERR; }; /* Drive on pin is 3-state. */
    /** (e) Global registers: PFUNC, PDIR, (-)DITCTL, DLBCTL, (-)AMUTE. */
    /* The pin function register (PFUNC) specifies the function of AXRn, ACLKX, AHCLKX, AFSX, ACLKR, AHCLKR, and AFSR pins
     * as either a McASP pin or a general-purpose input/output (GPIO) pin. */
    if (McASP->write_reg("PFUNC", 0x0 ) < 0) { REG_W_ERR; }; /* Set all functions as McASP. */
    /* Only AXR0 and AXR1 are accessible. */
    if (McASP->write_reg_bits("PFUNC",      "AXR",      0b1100 ) < 0) { REG_W_ERR; }; /* 0=McASP/1=GPIO*/
    /* AMUTE, ACLKR, AHCLKR, AFSR are unused. */
    if (McASP->write_reg_bits("PFUNC",      "AMUTE",    0x1 ) < 0) { REG_W_ERR; };
    if (McASP->write_reg_bits("PFUNC",      "ACLKR",    0x1 ) < 0) { REG_W_ERR; };
    if (McASP->write_reg_bits("PFUNC",      "AHCLKR",   0x1 ) < 0) { REG_W_ERR; };
    if (McASP->write_reg_bits("PFUNC",      "AFSR",     0x1 ) < 0) { REG_W_ERR; };

    if (McASP->write_reg_bits("PDIR",       "ACLKX",    0x1 ) < 0) { REG_W_ERR; }; /* ACLKX as output. */
    if (McASP->write_reg_bits("PDIR",       "AHCLKX",   0x0 ) < 0) { REG_W_ERR; }; /* AHCLKX as input. */
    if (McASP->write_reg_bits("PDIR",       "AFSX",     0x1 ) < 0) { REG_W_ERR; }; /* AFSX as output. */
    if (McASP->write_reg_bits("PDIR",       "AXR",      0b0001 ) < 0) { REG_W_ERR; }; /* 0=in/1=out */

    if (McASP->write_reg_bits("DLBCTL",     "DLBEN",    0x0 ) < 0) { REG_W_ERR; }; /* Loopback mode is disabled. */
    /** (3) Start the respective high-frequency serial clocks AHCLKX and/or AHCLKR. */
    /** (a) Take the respective internal high-frequency serial clock divider(s) out of reset
     * by setting the RHCLKRST bit for the receiver and/or the XHCLKRST bit for the transmitter in GBLCTL.*/
    if (McASP->write_reg_bits("GBLCTL",     "XHCLKRST", 0x1 ) < 0) { REG_W_ERR; }; /* Transmit high-frequency clock divider is running. */
    /** (b) Read back from GBLCTL to ensure the bit(s) to which you wrote are successfully latched in GBLCTL before you proceed. */
    McASP->read_reg ("GBLCTL", NULL);
    /** (4) Start the respective serial clocks ACLKX and/or ACLKR. This step can be skipped if external serial clocks are used and they are running. */
    /** (a) Take the respective internal serial clock divider(s) out of reset by setting the RCLKRST bit for the receiver and/or the XCLKRST bit for the transmitter in GBLCTL. */
    if (McASP->write_reg_bits("GBLCTL",     "XCLKRST",  0x1 ) < 0) { REG_W_ERR; }; /* Transmit clock divider is running. */
    /** (b) Read back from GBLCTL to ensure the bit(s) to which you wrote are successfully latched in GBLCTL before you proceed. */
    McASP->read_reg ("GBLCTL", NULL);
    /**TO DO: (5) Setup data acquisition as required. */
    /** (6) Activate serializers. */
    /** (a) Before starting, clear the respective transmitter and receiver status registers by writing XSTAT = FFFFh and RSTAT = FFFFh. */
    if (McASP->write_reg("XSTAT", 0xffffffff ) < 0) { REG_W_ERR; };
    if (McASP->write_reg("RSTAT", 0xffffffff ) < 0) { REG_W_ERR; };
    /** (b) Take the respective serializers out of reset by setting the RSRCLR bit for the receiver and/or the XSRCLR bit for the transmitter in GBLCTL.
     * All other bits in GBLCTL should be left at the previous state. */
    if (McASP->write_reg_bits("GBLCTL",     "XSRCLR",   0x1 ) < 0) { REG_W_ERR; }; /* Transmit serializers are active. */
    /** (c) Read back from GBLCTL to ensure the bit(s) to which you wrote are successfully latched in GBLCTL before you proceed. */
    McASP->read_reg ("GBLCTL", NULL);
    /** (7) Verify that all transmit buffers are serviced. */
    /** (a) Verify that the XDATA bit in the XSTAT is cleared to 0, indicating that all transmit buffers are already serviced by the DMA. */
    if (McASP->write_reg_bits("XSTAT",      "XDATA",    0x1 ) < 0) { REG_W_ERR; };
    /** (b) SKIP: CPU interrupts. */
    /** (c) SKIP: CPU polling. */
    /** (8) Release state machines from reset. */
    /** (a) Take the respective state machine(s) out of reset by setting the RSMRST bit for the receiver and/or the XSMRST bit for the transmitter in GBLCTL. */
    if (McASP->write_reg_bits("GBLCTL",     "XSMRST",   0x1 ) < 0) { REG_W_ERR; }; /* Transmit state machine is released from reset. */
    /** (b) Read back from GBLCTL to ensure the bit(s) to which you wrote are successfully latched in GBLCTL before you proceed. */
    McASP->read_reg ("GBLCTL", NULL);
    /** (9) Release frame sync generators from reset. */
    /** (a) Take the respective frame sync generator(s) out of reset by setting the RFRST bit for the receiver, and/or the XFRST bit for the transmitter in GBLCTL. */
    if (McASP->write_reg_bits("GBLCTL",     "XFRST",    0x1 ) < 0) { REG_W_ERR; }; /* Transmit frame sync generator is active.  */
    /** (b) Read back from GBLCTL to ensure the bit(s) to which you wrote are successfully latched in GBLCTL before you proceed. */
    McASP->read_reg ("GBLCTL", NULL);

    /** (10) Upon the first frame sync signal, McASP transfers begin. */
    if (McASP->write_reg_bits("XEVTCTL",    "XDATDMA",  0x0 ) < 0) { REG_W_ERR; };
    if (McASP->write_reg("XSTAT", 0xffffffff ) < 0) { REG_W_ERR; };
    McASP->read_reg ("XSTAT", NULL);
    McASP->read_reg ("XCLKCHK", NULL);

    delete McASP;
    return 0;
}
