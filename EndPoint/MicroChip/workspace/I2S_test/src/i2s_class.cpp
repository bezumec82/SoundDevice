#include "i2s_class.h"


#define NEG_MASK(size,off)          negMask<size, off>::neg_mask
#define POS_MASK(size,off)          posMask<size, off>::pos_mask
#define I2SC_BITS(__size, __off)     .size = __size,  .off = __off, .bits_val = 0, .neg_mask = NEG_MASK(__size,__off), .pos_mask = POS_MASK(__size,__off),



unordered_map  < string, I2SC_reg_file >  I2SC::i2s_regs = {
        {"I2SC_CR",     {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                    { "RXEN",      { I2SC_BITS(I2SC_CR_RXEN_SIZE   ,I2SC_CR_RXEN_OFF   ) .msg = "Receive Enable"       },},
                                    { "RXDIS",     { I2SC_BITS(I2SC_CR_RXDIS_SIZE  ,I2SC_CR_RXDIS_OFF  ) .msg = "Receive Disable"      },},
                                    { "CKEN",      { I2SC_BITS(I2SC_CR_CKEN_SIZE   ,I2SC_CR_CKEN_OFF   ) .msg = "Clocks Enable"        },},
                                    { "CKDIS",     { I2SC_BITS(I2SC_CR_CKDIS_SIZE  ,I2SC_CR_CKDIS_OFF  ) .msg = "Clocks Disable"       },},
                                    { "TXEN",      { I2SC_BITS(I2SC_CR_TXEN_SIZE   ,I2SC_CR_TXEN_OFF   ) .msg = "Transmitter Enable"   },},
                                    { "TXDIS",     { I2SC_BITS(I2SC_CR_TXDIS_SIZE  ,I2SC_CR_TXDIS_OFF  ) .msg = "Transmitter Disable"  },},
                                    { "SWRST",     { I2SC_BITS(I2SC_CR_SWRST_SIZE  ,I2SC_CR_SWRST_OFF  ) .msg = "Software Reset"       },},
                                    },},},

        {"I2SC_MR",     {   .file = NULL,
                            .permission = "w+",
                            .reg_val = 0,
                            .bits = {
                                    { "MODE",       { I2SC_BITS(I2SC_MR_MODE_SIZE       ,I2SC_MR_MODE_OFF       ) .msg = "Inter-IC Sound Controller Mode"           },},
                                    { "DATALENGTH", { I2SC_BITS(I2SC_MR_DATALENGTH_SIZE ,I2SC_MR_DATALENGTH_OFF ) .msg = "Data Word Length"                         },},
                                    { "FORMAT",     { I2SC_BITS(I2SC_MR_FORMAT_SIZE     ,I2SC_MR_FORMAT_OFF     ) .msg = "Data Format"                              },},
                                    { "RXMONO",     { I2SC_BITS(I2SC_MR_RXMONO_SIZE     ,I2SC_MR_RXMONO_OFF     ) .msg = "Receive Mono"                             },},
                                    { "RXLOOP",     { I2SC_BITS(I2SC_MR_RXLOOP_SIZE     ,I2SC_MR_RXLOOP_OFF     ) .msg = "Loopback Test Mode"                       },},
                                    { "TXMONO",     { I2SC_BITS(I2SC_MR_TXMONO_SIZE     ,I2SC_MR_TXMONO_OFF     ) .msg = "Transmit Mono"                            },},
                                    { "TXSAME",     { I2SC_BITS(I2SC_MR_TXSAME_SIZE     ,I2SC_MR_TXSAME_OFF     ) .msg = "Transmit Data when Underrun"              },},
                                    { "IMCKDIV",    { I2SC_BITS(I2SC_MR_IMCKDIV_SIZE    ,I2SC_MR_IMCKDIV_OFF    ) .msg = "Selected Clock to I2SC Master Clock Ratio"},},
                                    { "IMCKFS",     { I2SC_BITS(I2SC_MR_IMCKFS_SIZE     ,I2SC_MR_IMCKFS_OFF     ) .msg = "Master Clock to Fs Ratio"                 },},
                                    { "IMCKMODE",   { I2SC_BITS(I2SC_MR_IMCKMODE_SIZE   ,I2SC_MR_IMCKMODE_OFF   ) .msg = "Master Clock Mode"                        },},
                                    { "IWS",        { I2SC_BITS(I2SC_MR_IWS_SIZE        ,I2SC_MR_IWS_OFF        ) .msg = "I2SC_WS Slot Width"                       },},
                                    },},},

        {"I2SC_SR",    {  .file = NULL,
                            .permission = "r",
                            .reg_val = 0,
                            .bits = {
                                    { "RXEN",      { I2SC_BITS(I2SC_SR_RXEN_SIZE   ,I2SC_SR_RXEN_OFF    ) .msg = "Receiver Enabled"         },},
                                    { "RXRDY",     { I2SC_BITS(I2SC_SR_RXRDY_SIZE  ,I2SC_SR_RXRDY_OFF   ) .msg = "Receive Ready"            },},
                                    { "RXOR",      { I2SC_BITS(I2SC_SR_RXOR_SIZE   ,I2SC_SR_RXOR_OFF    ) .msg = "Receive Overrun"          },},
                                    { "TXEN",      { I2SC_BITS(I2SC_SR_TXEN_SIZE   ,I2SC_SR_TXEN_OFF    ) .msg = "Transmitter Enabled"      },},
                                    { "TXRDY",     { I2SC_BITS(I2SC_SR_TXRDY_SIZE  ,I2SC_SR_TXRDY_OFF   ) .msg = "Transmit Ready"           },},
                                    { "TXUR",      { I2SC_BITS(I2SC_SR_TXUR_SIZE   ,I2SC_SR_TXUR_OFF    ) .msg = "Transmit Underrun"        },},
                                    { "RXORCH",    { I2SC_BITS(I2SC_SR_RXORCH_SIZE ,I2SC_SR_RXORCH_OFF  ) .msg = "Receive Overrun Channel"  },},
                                    { "TXURCH",    { I2SC_BITS(I2SC_SR_TXURCH_SIZE ,I2SC_SR_TXURCH_OFF  ) .msg = "Transmit Underrun Channel"},},

        },},},


        {"I2SC_SCR",    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                    { "RXOR",      { I2SC_BITS(I2SC_SCR_RXOR_SIZE   ,I2SC_SCR_RXOR_OFF  ) .msg = "Receive Overrun Status Clear"                 },},
                                    { "TXUR",      { I2SC_BITS(I2SC_SCR_TXUR_SIZE   ,I2SC_SCR_TXUR_OFF  ) .msg = "Transmit Underrun Status Clear"               },},
                                    { "RXORCH",    { I2SC_BITS(I2SC_SCR_RXORCH_SIZE ,I2SC_SCR_RXORCH_OFF) .msg = "Receive Overrun Per Channel Status Clear"     },},
                                    { "TXURCH",    { I2SC_BITS(I2SC_SCR_TXURCH_SIZE ,I2SC_SCR_TXURCH_OFF) .msg = "Transmit Underrun Per Channel Status Clear"   },},
        },},},
        {"I2SC_SSR",    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                    { "RXOR",      { I2SC_BITS(I2SC_SSR_RXOR_SIZE   ,I2SC_SSR_RXOR_OFF  ) .msg = "Receive Overrun Status Set"               },},
                                    { "TXUR",      { I2SC_BITS(I2SC_SSR_TXUR_SIZE   ,I2SC_SSR_TXUR_OFF  ) .msg = "Transmit Underrun Status Set"             },},
                                    { "RXORCH",    { I2SC_BITS(I2SC_SSR_RXORCH_SIZE ,I2SC_SSR_RXORCH_OFF) .msg = "Receive Overrun Per Channel Status Set"   },},
                                    { "TXURCH",    { I2SC_BITS(I2SC_SSR_TXURCH_SIZE ,I2SC_SSR_TXURCH_OFF) .msg = "Transmit Underrun Per Channel Status Set" },},
        },},},


        {"I2SC_IER",    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                    { "RXRDY",     { I2SC_BITS(I2SC_IER_RXRDY_SIZE  ,I2SC_IER_RXRDY_OFF ) .msg = "Receiver Ready Interrupt Enable"      },},
                                    { "RXOR",      { I2SC_BITS(I2SC_IER_RXOR_SIZE   ,I2SC_IER_RXOR_OFF  ) .msg = "Receiver Overrun Interrupt Enable"    },},
                                    { "TXRDY",     { I2SC_BITS(I2SC_IER_TXRDY_SIZE  ,I2SC_IER_TXRDY_OFF ) .msg = "Transmit Ready Interrupt Enable"      },},
                                    { "TXUR",      { I2SC_BITS(I2SC_IER_TXUR_SIZE   ,I2SC_IER_TXUR_OFF  ) .msg = "Transmit Underflow Interrupt Enable"  },},
        },},},
        {"I2SC_IDR",    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                    { "RXRDY",     { I2SC_BITS(I2SC_IDR_RXRDY_SIZE  ,I2SC_IDR_RXRDY_OFF ) .msg = "Receiver Ready Interrupt Disable"     },},
                                    { "RXOR",      { I2SC_BITS(I2SC_IDR_RXOR_SIZE   ,I2SC_IDR_RXOR_OFF  ) .msg = "Receiver Overrun Interrupt Disable"   },},
                                    { "TXRDY",     { I2SC_BITS(I2SC_IDR_TXRDY_SIZE  ,I2SC_IDR_TXRDY_OFF ) .msg = "Transmit Ready Interrupt Disable"     },},
                                    { "TXUR",      { I2SC_BITS(I2SC_IDR_TXUR_SIZE   ,I2SC_IDR_TXUR_OFF  ) .msg = "Transmit Underflow Interrupt Disable" },},
        },},},
        {"I2SC_IMR",    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                    { "RXRDY",     { I2SC_BITS(I2SC_IMR_RXRDY_SIZE  ,I2SC_IMR_RXRDY_OFF ) .msg = "Receiver Ready Interrupt Disable"     },},
                                    { "RXOR",      { I2SC_BITS(I2SC_IMR_RXOR_SIZE   ,I2SC_IMR_RXOR_OFF  ) .msg = "Receiver Overrun Interrupt Disable"   },},
                                    { "TXRDY",     { I2SC_BITS(I2SC_IMR_TXRDY_SIZE  ,I2SC_IMR_TXRDY_OFF ) .msg = "Transmit Ready Interrupt Disable"     },},
                                    { "TXUR",      { I2SC_BITS(I2SC_IMR_TXUR_SIZE   ,I2SC_IMR_TXUR_OFF  ) .msg = "Transmit Underflow Interrupt Disable" },},
        },},},

        {"I2SC_RHR" ,    {   .file = NULL,
                            .permission = "r",
                            .reg_val = 0,
                            .bits = {
                                    { "RHR", { I2SC_BITS(I2SC_RHR_SIZE ,0 ) .msg = "Receiver Holding Register" },},
        },},},
        {"I2SC_THR" ,    {   .file = NULL,
                            .permission = "w",
                            .reg_val = 0,
                            .bits = {
                                    { "THR", { I2SC_BITS(I2SC_THR_SIZE ,0 ) .msg = "Transmitter Holding Register" },},
        },},},
};
/*--- End of member ------------------------------------------------------------------------------*/

/*! Constructor.
 * @param controller_num : The number of I2SC controller : 0 or 1.
 */
I2SC::I2SC ( uint8_t controller_num )
: reg_path("/dev/null"), i2s_dev_name("/dev/null"), i2s_dev_fp(NULL)
{
    if          (controller_num == 0)
    {
        i2s_dev_name = string("/dev/i2s0");
        reg_path = sysfs_pth + i2s0_dir +  string("/regs/");
    }
    else if     (controller_num == 1)
    {
        i2s_dev_name = string("/dev/i2s1");
        reg_path = sysfs_pth + i2s1_dir +  string("/regs/");
    }
    else return;

    int error_cntr = 0;
    for (auto file_iter : i2s_regs)
    {
        string reg_file = reg_path + file_iter.first;
#if DEBUG_I2SC
        PRINTF(MAG, "Opening file : %s.\n", reg_file.c_str());
#endif
        i2s_regs[file_iter.first].file = fopen(reg_file.c_str(), i2s_regs[file_iter.first].permission.c_str() );
        if (!i2s_regs[file_iter.first].file)
        {
            error_cntr++;
            FPRINTF_EXT("Can't open file : %s.\n", reg_file.c_str());
        }
    } //end for (auto file_iter
    if (error_cntr) { goto error; }

    i2s_dev_fp = fopen(i2s_dev_name.c_str(), "w+");
    if (!i2s_dev_fp)
    {
        FPRINTF_EXT("Can't open '%s' file.\n", i2s_dev_name.c_str());
        goto error;
    }

    this->print_files();
    return;
error:
    delete this;
    exit( EXIT_FAILURE );
}
/*--- End of member ------------------------------------------------------------------------------*/


/*!
 *
 * @param reg_name  : Name of the register to write.
 * @param reg_val   : Value to write.
 * @return          -1, written bytes or 'errno'.
 */
int I2SC::write_reg ( string reg_name, uint32_t reg_val )
{
    char buf[32] = {0};
    sprintf (buf, "%d", reg_val);
    try
    {
        if (i2s_regs.at(reg_name).permission == "r")
        {
            PRINTF(MAG, "'%s' read only.\n", reg_name.c_str());
            return ( EXIT_SUCCESS );
        }
        if (i2s_regs.at(reg_name).file)
        {
#if DEBUG_I2SC_SEVERE
        PRINTF(MAG, "Writing %s to the register : %s.\n", buf, reg_name.c_str() );
#endif
            int ret_val = fputs ( buf, i2s_regs.at(reg_name).file );
            if (ret_val < 0) FPRINTF;
            rewind( i2s_regs.at(reg_name).file );
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

int I2SC::write_reg_bits ( string reg_name, string bits_name, uint32_t bits_val )
{
#if SAFE_VECTOR_TRAVERSING
    try
    {
        if (i2s_regs.at(reg_name).permission == "r")
        {
            PRINTF(MAG, "'%s' read only.\n", reg_name.c_str());
            return ( EXIT_SUCCESS );
        }
        uint32_t    reg_val     = 0;
        uint8_t     off         = i2s_regs.at(reg_name).bits.at(bits_name).off;
        uint32_t    pos_mask    = i2s_regs.at(reg_name).bits.at(bits_name).pos_mask;
        uint32_t    neg_mask    = i2s_regs.at(reg_name).bits.at(bits_name).neg_mask;

        i2s_regs.at(reg_name).bits.at(bits_name).bits_val = bits_val;   ///<Store new bits value.
        if (i2s_regs.at(reg_name).permission != "w")                    ///<Read present register state.
        {
            this->read_reg (reg_name, &reg_val);
            reg_val &=  pos_mask;                                       ///<Clear all target bits.
        }
        //All write only registers bits respond only at 1.
//        else
//        {
//            reg_val = i2s_regs.at(reg_name).reg_val;
//        }
        reg_val |=  neg_mask & ( bits_val << off );                     ///<Set new bits.
        i2s_regs.at(reg_name).reg_val = reg_val;                        ///<Save new register state locally.
        write_reg ( reg_name, reg_val );                                ///<Write new register state.
#if DEBUG_I2SC_SEVERE
            PRINTF(NORM, "%-48s set to : 0x%x.\n",
                    i2s_regs.at(reg_name).bits.at(bits_name).msg,
                    i2s_regs.at(reg_name).bits.at(bits_name).bits_val);
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
int I2SC::read_reg (const string & reg_name, uint32_t * reg_val)
{
    char buf[32] = {0};
    try
    {
        if (i2s_regs.at(reg_name).permission == "w")
        {
            PRINTF(MAG, "'%s' write only.\n", reg_name.c_str());
            *reg_val = i2s_regs.at(reg_name).reg_val;
            return ( EXIT_SUCCESS );
        }
        if (!i2s_regs.at(reg_name).file) { PRINT_ERR("File not opened.\n"); *reg_val = 0; return (-1); }

        char * ret_val = fgets(buf, sizeof(buf), i2s_regs.at(reg_name).file);   ///<Read from register file.
        if (!ret_val)
        {
            FPRINTF;
            *reg_val = 0;
            return ( -EXIT_FAILURE );
        };
#if DEBUG_I2SC_SEVERE
        PRINTF(MAG, "Value of the register %10s : %s.\n", reg_name.c_str(), buf );
#endif
        *reg_val = strtol(buf, NULL, 10);                                       ///<Convert string to number.
        i2s_regs.at(reg_name).reg_val = *reg_val;                               ///<Save new register state locally.
        rewind(i2s_regs.at(reg_name).file);                                     ///<Rewind file.
        return ( EXIT_SUCCESS );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register : %s.\n", reg_name.c_str());
        return ( -EXIT_FAILURE );
    }
}
/*--- End of member ------------------------------------------------------------------------------*/

int I2SC::read_reg_bits (string reg_name, string bits_name, uint32_t * bits_val)
{
    try
    {
        if (i2s_regs.at(reg_name).permission == "w")
        {
            PRINTF(MAG, "'%s' write only.\n", reg_name.c_str());
            *bits_val = i2s_regs.at(reg_name).bits.at(bits_name).bits_val;
            return ( EXIT_SUCCESS );
        }
        uint32_t    reg_val     = 0;
        uint8_t     off         = i2s_regs.at(reg_name).bits.at(bits_name).off;
        uint32_t    neg_mask    = i2s_regs.at(reg_name).bits.at(bits_name).neg_mask;
        uint32_t    bits_tmp    = 0;

        this->read_reg (reg_name, &reg_val);
        bits_tmp = (reg_val&neg_mask)>>off;
        if (bits_val) { *bits_val =  bits_tmp; }
        i2s_regs.at(reg_name).bits.at(bits_name).bits_val = bits_tmp;
        return ( EXIT_SUCCESS );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
        return ( -EXIT_FAILURE );
    }
}
/*--- End of member ------------------------------------------------------------------------------*/


uint32_t I2SC::show_reg (string reg_name)
{
    try
    {
        return ( i2s_regs.at(reg_name).reg_val );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register '%s'.\n", reg_name.c_str());
        return ( 0xffffffff );
    }
}

uint32_t I2SC::show_reg_bits (string reg_name, string bits_name)
{
    try
    {
        return ( i2s_regs.at(reg_name).bits.at(bits_name).bits_val );
    }
    catch (std::out_of_range)
    {
        PRINT_ERR("No register '%s' or bits '%s'.\n", reg_name.c_str(), bits_name.c_str());
        return ( 0xffffffff );
    }
}
/*--- End of member ------------------------------------------------------------------------------*/

void I2SC::read_all_regs ( void )
{
    for (auto iter : i2s_regs)
    {
        uint32_t reg_val = 0;
        int ret_val = this->read_reg(iter.first, &reg_val);
        PRINTF(CYN, "Value of the register %-10s : 0x%-8x.\n", iter.first.c_str(), reg_val);
        if (ret_val!=EXIT_SUCCESS) { FPRINTF_EXT("Can't read register : %s.\n", iter.first.c_str()); }
    }
}
/*--- End of member ------------------------------------------------------------------------------*/

I2SC::~I2SC ( void )
{
    for (auto iter : i2s_regs)
    {
        string reg_file = reg_path + iter.first;
#if DEBUG_I2SC
        PRINTF(MAG, "Closing file : %s.\n", reg_file.c_str());
#endif
        if (i2s_regs[iter.first].file)
        {
            fclose(i2s_regs[iter.first].file);
        }
        else
        {
            PRINT_ERR("File '%s' isn't opened.\n", reg_file.c_str());
        }
    } //end for
    if (i2s_dev_fp)
    {
        fclose(i2s_dev_fp);
    }
    else
    {
        PRINT_ERR("File '%s' isn't opened.\n", i2s_dev_name.c_str());
    }
}/* end ~I2SC */
/*--- End of member ------------------------------------------------------------------------------*/

void I2SC::print_files ( void )
{
//    for(auto iter : i2s_regs)
//    {
//        PRINTF(MAG, "File name : %10s. \tFile descriptor : %3i.\n", iter.first.c_str(), iter.second.fd);
//    }
}
/*--- End of member ------------------------------------------------------------------------------*/

int I2SC::write (void * buf, size_t size)
{
    int ret_val = fwrite ((char *) (buf), sizeof(char), size, i2s_dev_fp );
    return ( ret_val );
}

