#ifndef MCASP_CLASS
#define MCASP_CLASS

#include "main.h"
//using namespace std;

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
struct McASP_bits {
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
struct McASP_reg_file {
    FILE *                                      file;   ///< File stream.
    std::string                                 permission;
    uint32_t                                    reg_val;
    std::unordered_map < std::string,  McASP_bits >  bits;   ///< Bits description
};
/*--- End of struct ------------------------------------------------------------------------------*/

class McASP {
private:
    std::string sysfs_pth = "/sys/bus/platform/devices/";
    std::string McASP0_dir = "48038000.mcasp";
    std::string McASP1_dir = "4803C000.mcasp";
    std::string reg_path;

    FILE * McASP_THR_file = NULL; ///< To bypass associative container;
    FILE * McASP_RHR_file = NULL; ///< To bypass associative container;
    FILE * McASP_SR_file = NULL;  ///< To bypass associative container;

    static std::unordered_map  < std::string, McASP_reg_file >  McASP_regs;
public:
    McASP ( uint8_t ) ;           /*!< Default constructor */

    int write_reg ( std::string , uint32_t  );
    int write_reg_bits ( std::string , std::string , uint32_t );

    int read_reg ( const std::string & , uint32_t * );
    int read_reg_bits ( std::string , std::string , uint32_t * );

    uint32_t show_reg ( std::string);
    uint32_t show_reg_bits ( std::string , std::string );

    int write_data (uint32_t );

    void read_all_regs ( void );
    ~McASP ( void );
};
/*--- End of class -------------------------------------------------------------------------------*/

#endif /* __McASP_CLASS_H__ */
