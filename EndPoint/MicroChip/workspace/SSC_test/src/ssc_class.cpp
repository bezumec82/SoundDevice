#include "ssc_class.h"

#define NEG_MASK(size,off)          negMask<size, off>::neg_mask
#define POS_MASK(size,off)          posMask<size, off>::pos_mask
#define SSC_BITS(__size, __off)     .size = __size,  .off = __off, .bits_val = 0, .neg_mask = NEG_MASK(__size,__off), .pos_mask = POS_MASK(__size,__off),



unordered_map  < string, SSC_reg_file >  SSC::ssc_regs = {
        {"SSC_CR",      {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                                 { "RXEN",      { SSC_BITS(1 ,0 ) .msg = "Receive Enable"     },},
                                                 { "RXDIS",     { SSC_BITS(1 ,1 ) .msg = "Receive Disable"    },},
                                                 { "TXEN",      { SSC_BITS(1 ,8 ) .msg = "Transmit Enable"    },},
                                                 { "TXDIS",     { SSC_BITS(1 ,9 ) .msg = "Transmit Disable"   },},
                                                 { "SWRST",     { SSC_BITS(1 ,15) .msg = "Software Reset"     },},},},},
        {"SSC_CMR",     {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "DIV",       { SSC_BITS(12,0 ) .msg = "Clock Divider",     },},},},},
        {"SSC_RCMR",    {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "CKS",       { SSC_BITS(2 ,0 ) .msg = "Receive Clock Selection"               },},
                                                 { "CKO",       { SSC_BITS(3 ,2 ) .msg = "Receive Clock Output Mode Selection"   },},
                                                 { "CKI",       { SSC_BITS(1 ,5 ) .msg = "Receive Clock Inversion"               },},
                                                 { "CKG",       { SSC_BITS(2 ,6 ) .msg = "Receive Clock Gating Selection"        },},
                                                 { "START",     { SSC_BITS(4 ,8 ) .msg = "Receive Start Selection"               },},
                                                 { "STOP",      { SSC_BITS(1 ,12) .msg = "Receive Stop Selection"                },},
                                                 { "STTDLY",    { SSC_BITS(8 ,16) .msg = "Receive Start Delay"                   },},
                                                 { "PERIOD",    { SSC_BITS(8 ,24) .msg = "Receive Period Divider Selection"      },},},},},
        {"SSC_TCMR",    {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "CKS",       { SSC_BITS(2 ,0 ) .msg = "Receive Clock Selection"               },},
                                                 { "CKO",       { SSC_BITS(3 ,2 ) .msg = "Receive Clock Output Mode Selection"   },},
                                                 { "CKI",       { SSC_BITS(1 ,5 ) .msg = "Receive Clock Inversion"               },},
                                                 { "CKG",       { SSC_BITS(2 ,6 ) .msg = "Receive Clock Gating Selection"        },},
                                                 { "START",     { SSC_BITS(4 ,8 ) .msg = "Receive Start Selection"               },},
                                                 { "STTDLY",    { SSC_BITS(8 ,16) .msg = "Receive Start Delay"                   },},
                                                 { "PERIOD",    { SSC_BITS(8 ,24) .msg = "Receive Period Divider Selection"      },},},},},
        {"SSC_RFMR",    {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "DATLEN",    { SSC_BITS(5 ,0 ) .msg = "Data Length"                           },},
                                                 { "LOOP",      { SSC_BITS(1 ,5 ) .msg = "Loop Mode"                             },},
                                                 { "MSBF",      { SSC_BITS(1 ,7 ) .msg = "Most Significant Bit First"            },},
                                                 { "DATNB",     { SSC_BITS(4 ,8 ) .msg = "Data Number per Frame"                 },},
                                                 { "FSLEN",     { SSC_BITS(4 ,16) .msg = "Receive Frame Sync Length"             },},
                                                 { "FSOS",      { SSC_BITS(3 ,20) .msg = "Transmit Frame Sync Output Selection"  },},
                                                 { "FSEDGE",    { SSC_BITS(1 ,24) .msg = "Frame Sync Edge Detection"             },},
                                                 { "FSLEN_EXT", { SSC_BITS(4 ,28) .msg = "FSLEN Field Extension"                 },},},},},
        {"SSC_TFMR",    {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "DATLEN",    { SSC_BITS(5 ,0 ) .msg = "Data Length"                           },},
                                                 { "DATDEF",    { SSC_BITS(1 ,5 ) .msg = "Data Default Value"                    },},
                                                 { "MSBF",      { SSC_BITS(1 ,7 ) .msg = "Most Significant Bit First"            },},
                                                 { "DATNB",     { SSC_BITS(4 ,8 ) .msg = "Data Number per Frame"                 },},
                                                 { "FSLEN",     { SSC_BITS(4 ,16) .msg = "Transmit Frame Sync Length"            },},
                                                 { "FSOS",      { SSC_BITS(3 ,20) .msg = "Transmit Frame Sync Output Selection"  },},
                                                 { "FSDEN",     { SSC_BITS(1 ,23) .msg = "Frame Sync Data Enable"                },},
                                                 { "FSEDGE",    { SSC_BITS(1 ,24) .msg = "Frame Sync Edge Detection"             },},
                                                 { "FSLEN_EXT", { SSC_BITS(4 ,28) .msg = "Field Extension"                       },},},},},
        {"SSC_RHR" ,    {   .file = NULL,
                            .permission = "r",
                            .reg_val = 0,
                            .bits = {
                                                 { "RDAT",      { SSC_BITS(32,0 ) .msg = "Receive Data"                          },},},},},
        {"SSC_THR" ,    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                                 { "TDAT",      { SSC_BITS(32,0 ) .msg = "Transmit Data"                         },},},},},
        {"SSC_RSHR" ,   {   .file = NULL,
                            .permission = "r",
                            .reg_val = 0,
                            .bits = {
                                                 { "RSDAT",     { SSC_BITS(16,0 ) .msg = "Receive Synchronization Data"          },},},},},
        {"SSC_TSHR" ,   {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "TSDAT",     { SSC_BITS(16,0 ) .msg = "Transmit Synchronization Data"         },},},},},
        {"SSC_RC0R" ,   {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "CP0",       { SSC_BITS(16,0 ) .msg = "Receive Compare Data 0"                },},},},},
        {"SSC_RC1R" ,   {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "CP1",       { SSC_BITS(16,0 ) .msg = "Receive Compare Data 1"                },},},},},
        {"SSC_SR" ,     {   .file = NULL,
                            .permission = "r",
                            .reg_val = 0,
                            .bits = {
                                                 { "TXRDY",     { SSC_BITS(1 ,0 ) .msg = "Transmit Ready"   },},
                                                 { "TXEMPTY",   { SSC_BITS(1 ,1 ) .msg = "Transmit Empty"   },},
                                                 { "RXDY",      { SSC_BITS(1 ,4 ) .msg = "Receive Ready"    },},
                                                 { "OVRUN",     { SSC_BITS(1 ,5 ) .msg = "Receive Overrun"  },},
                                                 { "CP0",       { SSC_BITS(1 ,8 ) .msg = "Compare 0"        },},
                                                 { "CP1",       { SSC_BITS(1 ,9 ) .msg = "Compare 1"        },},
                                                 { "TXSYN",     { SSC_BITS(1 ,10) .msg = "Transmit Sync"    },},
                                                 { "RXSYN",     { SSC_BITS(1 ,11) .msg = "Receive Sync"     },},
                                                 { "TXEN",      { SSC_BITS(1 ,16) .msg = "Transmit Enable"  },},
                                                 { "RXEN",      { SSC_BITS(1 ,17) .msg = "Receive Enable"   },},},},},
        {"SSC_IER" ,    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                                 { "TXRDY",     { SSC_BITS(1 ,0 ) .msg = "Transmit Ready Interrupt Enable"  },},
                                                 { "TXEMPTY",   { SSC_BITS(1 ,1 ) .msg = "Transmit Empty Interrupt Enable"  },},
                                                 { "RXRDY",     { SSC_BITS(1 ,4 ) .msg = "Receive Ready Interrupt Enable"   },},
                                                 { "OVRUN",     { SSC_BITS(1 ,5 ) .msg = "Receive Overrun Interrupt Enable" },},
                                                 { "CP0",       { SSC_BITS(1 ,8 ) .msg = "Compare 0 Interrupt Enable"       },},
                                                 { "CP1",       { SSC_BITS(1 ,9 ) .msg = "Compare 1 Interrupt Enable"       },},
                                                 { "TXSYN",     { SSC_BITS(1 ,10) .msg = "TX Sync Interrupt Enable"         },},
                                                 { "RXSYN",     { SSC_BITS(1 ,11) .msg = "RX Sync Interrupt Enable"         },},},},},
        {"SSC_IDR" ,    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                                 { "TXRDY",     { SSC_BITS(1 ,0 ) .msg = "Transmit Ready Interrupt Disable" },},
                                                 { "TXEMPTY",   { SSC_BITS(1 ,1 ) .msg = "Transmit Empty Interrupt Disable" },},
                                                 { "RXRDY",     { SSC_BITS(1 ,4 ) .msg = "Receive Ready Interrupt Disable"  },},
                                                 { "OVRUN",     { SSC_BITS(1 ,5 ) .msg = "Receive Overrun Interrupt Disable"},},
                                                 { "CP0",       { SSC_BITS(1 ,8 ) .msg = "Compare 0 Interrupt Disable"      },},
                                                 { "CP1",       { SSC_BITS(1 ,9 ) .msg = "Compare 1 Interrupt Disable"      },},
                                                 { "TXSYN",     { SSC_BITS(1 ,10) .msg = "TX Sync Interrupt Disable"        },},
                                                 { "RXSYN",     { SSC_BITS(1 ,11) .msg = "RX Sync Interrupt Disable"        },},},},},
        {"SSC_IMR" ,    {   .file = NULL,
                            .permission = "r",
                            .reg_val = 0,
                            .bits = {
                                                 { "TXRDY",     { SSC_BITS(1 ,0 ) .msg = "Transmit Ready Interrupt Mask"    },},
                                                 { "TXEMPTY",   { SSC_BITS(1 ,1 ) .msg = "Transmit Empty Interrupt Mask"    },},
                                                 { "RXRDY",     { SSC_BITS(1 ,4 ) .msg = "Receive Ready Interrupt Mask"     },},
                                                 { "OVRUN",     { SSC_BITS(1 ,5 ) .msg = "Receive Overrun Interrupt Mask"   },},
                                                 { "CP0",       { SSC_BITS(1 ,8 ) .msg = "Compare 0 Interrupt Mask"         },},
                                                 { "CP1",       { SSC_BITS(1 ,9 ) .msg = "Compare 1 Interrupt Mask"         },},
                                                 { "TXSYN",     { SSC_BITS(1 ,10) .msg = "TX Sync Interrupt Mask"           },},
                                                 { "RXSYN",     { SSC_BITS(1 ,11) .msg = "RX Sync Interrupt Mask"           },},},},},

        {"SSC_WPMR",    {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                                 { "WPEN",  { .size = 1,  .off =  0,  .bits_val = 0,        .neg_mask = NEG_MASK(1 ,0 ), .pos_mask = POS_MASK(1 ,0 ), .msg = "Write Protection Enable"    },},
                                                 { "WPKEY", { .size = 24, .off =  8,  .bits_val = 0x535343, .neg_mask = NEG_MASK(24,8 ), .pos_mask = POS_MASK(24,8 ), .msg = "Write Protection Key"       },},},},},
        {"SSC_WPSR",    {   .file = NULL,
                            .permission = "r",
                            .reg_val = 0,
                            .bits = {
                                                 { "WPVS",      { SSC_BITS(1 ,0 ) .msg = "Write Protection Violation Status"   },},
                                                 { "WPKEY",     { SSC_BITS(24,8 ) .msg = "Write Protect Violation Source"      },},},},},
};
/*--- End of member ------------------------------------------------------------------------------*/

/*! Constructor.
 * @param controller_num : The number of SSC controller : 0 or 1.
 */
SSC::SSC ( uint8_t controller_num ) : reg_path("/dev/null")
{
    if          (controller_num == 0)  { reg_path = sysfs_pth + ssc0_dir +  string("/regs/"); }
    else if     (controller_num == 1)  { reg_path = sysfs_pth + ssc1_dir +  string("/regs/"); }
    else return;

    for (auto file_iter : this->ssc_regs)
    {
        string reg_file = reg_path + file_iter.first;
#if DEBUG_SSC
        PRINTF(MAG, "Opening file : %s.\n", reg_file.c_str());
#endif
        this->ssc_regs[file_iter.first].file = fopen(reg_file.c_str(), this->ssc_regs[file_iter.first].permission.c_str() );
        if (!this->ssc_regs[file_iter.first].file)
        {
            FPRINTF_EXT("Can't open file : %s.\n", reg_file.c_str());
        }
    } //end for (auto file_iter
    this->print_files();

    try
    {   //Save this registers for quick access.
        SSC_THR_file    = ssc_regs.at("SSC_THR").file;
        SSC_RHR_file    = ssc_regs.at("SSC_RHR").file;
        SSC_SR_file     = ssc_regs.at("SSC_SR").file;
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("Can't save THR/RHR files.\n");
    }
}
/*--- End of member ------------------------------------------------------------------------------*/


/*!
 *
 * @param reg_name  : Name of the register to write.
 * @param reg_val   : Value to write.
 * @return          -1, written bytes or 'errno'.
 */
int SSC::write_reg ( string reg_name, uint32_t reg_val )
{
    char buf[32] = {0};
    sprintf (buf, "%d", reg_val);
    try
    {
        if (ssc_regs.at(reg_name).permission == "r")
        {
            PRINTF(MAG, "'%s' read only.\n", reg_name.c_str());
            return ( EXIT_SUCCESS );
        }
        if (ssc_regs.at(reg_name).file)
        {
#if DEBUG_SSC_SEVERE
        PRINTF(MAG, "Writing %s to the register : %s.\n", buf, reg_name.c_str() );
#endif
            int ret_val = fputs ( buf, ssc_regs.at(reg_name).file );
            if (ret_val < 0) FPRINTF;
            rewind( ssc_regs.at(reg_name).file );
            return ( ret_val );
        } else { PRINT_ERR("File not opened.\n"); return (-1); }
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register : %s.\n", reg_name.c_str());
        return ( -EXIT_FAILURE );
    }
}
/*--- End of member ------------------------------------------------------------------------------*/

int SSC::write_reg_bits ( string reg_name, string bits_name, uint32_t bits_val )
{
#if SAFE_VECTOR_TRAVERSING
    try
    {
        if (ssc_regs.at(reg_name).permission == "r")
        {
            PRINTF(MAG, "'%s' read only.\n", reg_name.c_str());
            return ( EXIT_SUCCESS );
        }
        uint32_t    reg_val     = 0;
        uint8_t     off         = ssc_regs.at(reg_name).bits.at(bits_name).off;
        uint32_t    pos_mask    = ssc_regs.at(reg_name).bits.at(bits_name).pos_mask;
        uint32_t    neg_mask    = ssc_regs.at(reg_name).bits.at(bits_name).neg_mask;

        ssc_regs.at(reg_name).bits.at(bits_name).bits_val = bits_val;   ///<Store new bits value.
        if (ssc_regs.at(reg_name).permission != "w")                    ///<Read present register state.
        {
            this->read_reg (reg_name, &reg_val);
            reg_val &=  pos_mask;                                       ///<Clear all target bits.
        }
        //All write only registers bits respond only at 1.
//        else
//        {
//            reg_val = ssc_regs.at(reg_name).reg_val;
//        }
        reg_val |=  neg_mask & ( bits_val << off );                     ///<Set new bits.
        ssc_regs.at(reg_name).reg_val = reg_val;                        ///<Save new register state locally.
        write_reg ( reg_name, reg_val );                                ///<Write new register state.
#if DEBUG_SSC_SEVERE
            PRINTF(NORM, "%-48s set to : 0x%x.\n",
                    ssc_regs.at(reg_name).bits.at(bits_name).msg,
                    ssc_regs.at(reg_name).bits.at(bits_name).val);
            std::cout << "Neg. mask : " << std::bitset<32>(neg_mask) << endl;
            std::cout << "Pos. mask : " << std::bitset<32>(pos_mask) << endl;
#endif
            return ( EXIT_SUCCESS );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
        return ( -EXIT_FAILURE );
    }
#else

#endif
}

/*--- End of member ------------------------------------------------------------------------------*/

/*!
 * @param reg_name  : Name of the register to read.
 * @param reg_val   : Return parameter - read value.
 * @return          0, -1 or 'errno'.
 */
int SSC::read_reg (const string & reg_name, uint32_t * reg_val)
{
    char buf[32] = {0};
    try
    {
        if (ssc_regs.at(reg_name).permission == "w")
        {
            PRINTF(MAG, "'%s' write only.\n", reg_name.c_str());
            *reg_val = ssc_regs.at(reg_name).reg_val;
            return ( EXIT_SUCCESS );
        }
        if (!ssc_regs.at(reg_name).file) { PRINT_ERR("File not opened.\n"); *reg_val = 0; return (-1); }

        char * ret_val = fgets(buf, sizeof(buf), ssc_regs.at(reg_name).file);   ///<Read from register file.
        if (!ret_val)
        {
            FPRINTF;
            *reg_val = 0;
            return ( -EXIT_FAILURE );
        };
#if DEBUG_SSC_SEVERE
        PRINTF(MAG, "Value of the register %10s : %s.\n", reg_name.c_str(), buf );
#endif
        *reg_val = strtol(buf, NULL, 10);                                       ///<Convert string to number.
        ssc_regs.at(reg_name).reg_val = *reg_val;                               ///<Save new register state locally.
        rewind(ssc_regs.at(reg_name).file);                                     ///<Rewind file.
        return ( EXIT_SUCCESS );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register : %s.\n", reg_name.c_str());
        return ( -EXIT_FAILURE );
    }
}
/*--- End of member ------------------------------------------------------------------------------*/

int SSC::read_reg_bits (string reg_name, string bits_name, uint32_t * bits_val)
{
    try
    {
        if (ssc_regs.at(reg_name).permission == "w")
        {
            PRINTF(MAG, "'%s' write only.\n", reg_name.c_str());
            *bits_val = ssc_regs.at(reg_name).bits.at(bits_name).bits_val;
            return ( EXIT_SUCCESS );
        }
        uint32_t    reg_val     = 0;
        uint8_t     off         = ssc_regs.at(reg_name).bits.at(bits_name).off;
        uint32_t    neg_mask    = ssc_regs.at(reg_name).bits.at(bits_name).neg_mask;
        uint32_t    bits_tmp    = 0;

        this->read_reg (reg_name, &reg_val);
        bits_tmp = (reg_val&neg_mask)>>off;
        if (bits_val) { *bits_val =  bits_tmp; }
        ssc_regs.at(reg_name).bits.at(bits_name).bits_val = bits_tmp;
        return ( EXIT_SUCCESS );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
        return ( -EXIT_FAILURE );
    }
}
/*--- End of member ------------------------------------------------------------------------------*/


uint32_t SSC::show_reg (string reg_name)
{
    try
    {
        return ( ssc_regs.at(reg_name).reg_val );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register '%s'.\n", reg_name.c_str());
        return ( 0xffffffff );
    }
}

uint32_t SSC::show_reg_bits (string reg_name, string bits_name)
{
    try
    {
        return ( ssc_regs.at(reg_name).bits.at(bits_name).bits_val );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
        return ( 0xffffffff );
    }
}
/*--- End of member ------------------------------------------------------------------------------*/

void SSC::read_all_regs ( void )
{
    for (auto iter : ssc_regs)
    {
        uint32_t reg_val = 0;
        int ret_val = this->read_reg(iter.first, &reg_val);
        PRINTF(CYN, "Value of the register %-10s : 0x%-8x.\n", iter.first.c_str(), reg_val);
        if (ret_val!=EXIT_SUCCESS) { FPRINTF_EXT("Can't read register : %s.\n", iter.first.c_str()); }
    }
}
/*--- End of member ------------------------------------------------------------------------------*/

SSC::~SSC ( void )
{
    for (auto iter : ssc_regs)
    {
        string reg_file = reg_path + iter.first;
        PRINTF(MAG, "Closing file : %s.\n", reg_file.c_str());
        fclose(ssc_regs[iter.first].file);
    } //end for
}/* end ~SSC */
/*--- End of member ------------------------------------------------------------------------------*/

void SSC::print_files ( void )
{
//    for(auto iter : ssc_regs)
//    {
//        PRINTF(MAG, "File name : %10s. \tFile descriptor : %3i.\n", iter.first.c_str(), iter.second.fd);
//    }
}
/*--- End of member ------------------------------------------------------------------------------*/

int SSC::write_data (uint32_t SSC_THR_val)
{
    uint32_t SSC_SR_val     = 0;
    uint32_t TXRDY_val      = 0;
    char SSC_SR_buf[32]     = {0};
    char SSC_THR_buf[32]    = {0};

    if (!SSC_THR_file||!SSC_SR_file)
    {
        PRINT_ERR("Necessary files not opened.\n");
        return ( -1 );
    }
    //Wait for TXRDY bit.
    do {
        char * ret_val = fgets(SSC_SR_buf, sizeof(SSC_SR_buf), SSC_SR_file);   ///<Read from status register file.
        if (!ret_val)
        {
            FPRINTF;
            return ( -EXIT_FAILURE );
        };
        SSC_SR_val = strtol(SSC_SR_buf, NULL, 10);  ///<Convert string to number.
        rewind(SSC_SR_file);  ///<Rewind file.
        TXRDY_val = SSC_SR_val&POS_MASK(1,0);
    } while(!TXRDY_val);

    //Write to the THR.
    sprintf (SSC_THR_buf, "%d", SSC_THR_val);
    int ret_val = fputs (SSC_THR_buf, SSC_THR_file);
    rewind(SSC_THR_file);
    return ( ret_val );
}

