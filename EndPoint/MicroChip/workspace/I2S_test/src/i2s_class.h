#ifndef __I2SC_CLASS_H__
#define __I2SC_CLASS_H__

#include "main.h"

template <uint8_t width, uint8_t offset>
struct negMask {
    static const uint32_t neg_mask = negMask<width-1, offset>::neg_mask|(1<<(width + offset - 1));
};

/* Partial specialization */
template<uint8_t offset>
struct negMask <0, offset> {
    static const uint32_t neg_mask = 0;
};

template <uint8_t width, uint8_t offset>
struct posMask {
    static const uint32_t pos_mask = ~negMask<width, offset>::neg_mask;
};
/*------------------------------------------------------------------------------------------------*/

/*! Describes bits */
struct I2SC_bits {
    uint8_t                     size : 6;   ///<Size of the register.
    uint8_t                     off  : 6;   ///<Offset from 0.
    uint32_t volatile           bits_val;   ///<Value of the register.
    uint32_t                    neg_mask;
    uint32_t                    pos_mask;
    const char *                msg;
};
/*--- End of struct ------------------------------------------------------------------------------*/

/*! Each register file
 * has file descriptor and bits of each register.
 */
struct I2SC_reg_file {
    FILE *                                      file;       ///< File stream.
    std::string                                 permission;
    uint32_t                                    reg_val;
    unordered_map < std::string,  I2SC_bits >   bits;       ///< Bits description
};
/*--- End of struct ------------------------------------------------------------------------------*/

class I2SC {
private:
    string sysfs_pth = "/sys/bus/platform/devices/";

    string i2s0_dir = "f8050000.i2s";
    string i2s1_dir = "fc04c000.i2s";
    string reg_path;

    string i2s_dev_name;
    FILE * i2s_dev_fp;

    static std::unordered_map  < std::string, I2SC_reg_file >  i2s_regs;
    //char buf[32] = { 0 };
public:
    I2SC ( uint8_t ) ;           /*!< Default constructor */

    int write_reg ( std::string , uint32_t  );
    int write_reg_bits ( std::string , std::string , uint32_t );

    int write (void * , size_t );

    int read_reg ( const std::string & , uint32_t * );
    int read_reg_bits (std::string , std::string , uint32_t * );

    uint32_t show_reg (std::string);
    uint32_t show_reg_bits (std::string , std::string );



    void read_all_regs ( void );
    void print_files ( void );
    ~I2SC ( void );
};
/*--- End of class -------------------------------------------------------------------------------*/

#endif /* __I2SC_CLASS_H__ */
