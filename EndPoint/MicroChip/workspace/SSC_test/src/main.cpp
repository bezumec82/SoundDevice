#include "main.h"



#define REG_W_ERR   PRINT_ERR("Can't change SSC reg. bits.\n")

#define PERIOD_BITS(n)  n/2 - 1
#define DATLEN_BITS(n)  n - 1
#define DATNB(n)        n - 1
#define FSLEN(n)        n-1

int main(/*int argc, char * argv[]*/) {
    cout << "Starting SSC test." << endl;

    SSC * ssc = new SSC(0);


    /* SSC Control Register */
    if (ssc->write_reg_bits("SSC_CR", "SWRST",          0x1     ) < 0)  { REG_W_ERR; };
    if (ssc->write_reg_bits("SSC_CR", "RXDIS",          0x1     ) < 0)  { REG_W_ERR; };
    if (ssc->write_reg_bits("SSC_CR", "TXDIS",          0x1     ) < 0)  { REG_W_ERR; };
    /* SSC Clock Mode Register */
    if (ssc->write_reg_bits("SSC_CMR", "DIV",           0x0     ) < 0)  { REG_W_ERR; }; /*!< Clock Divider */ /* 0 -> The Clock Divider is not active.*/

    /* SSC Receive Clock Mode Register */
    if (ssc->write_reg_bits("SSC_RCMR", "CKS",          0x2                 ) < 0)  { REG_W_ERR; }; /*!< Receive Clock Selection */ /* 2 -> RK pin */
    if (ssc->write_reg_bits("SSC_RCMR", "CKO",          0x0                 ) < 0)  { REG_W_ERR; }; /*!< Receive Clock Output Mode Selection */ /* 0 -> None, RK pin is an input */
    if (ssc->write_reg_bits("SSC_RCMR", "CKI",          0x1                 ) < 0)  { REG_W_ERR; }; /*!< Receive Clock Inversion */ /* 1 -> The data inputs are sampled on Receive Clock rising edge */
    if (ssc->write_reg_bits("SSC_RCMR", "CKG",          0x0                 ) < 0)  { REG_W_ERR; }; /*!< Receive Clock Gating Selection */ /* None */
    if (ssc->write_reg_bits("SSC_RCMR", "START",        0x5                 ) < 0)  { REG_W_ERR; }; /*!< Receive Start Selection */ /* Continuous, as soon as the receiver is enabled */
    if (ssc->write_reg_bits("SSC_RCMR", "STOP",         0x0                 ) < 0)  { REG_W_ERR; }; /*!< Receive Stop Selection */
    if (ssc->write_reg_bits("SSC_RCMR", "STTDLY",       63                  ) < 0)  { REG_W_ERR; }; /*!< Receive Start Delay */
    if (ssc->write_reg_bits("SSC_RCMR", "PERIOD",       PERIOD_BITS(64)     ) < 0)  { REG_W_ERR; }; /*!< Receive Period Divider Selection */ /* If not 0, a PERIOD signal is generated each 2 x (PERIOD + 1) Receive Clock. */
    /* SSC Receive Frame Mode Register */
    if (ssc->write_reg_bits("SSC_RFMR", "DATLEN",       DATLEN_BITS(16)     ) < 0)  { REG_W_ERR; }; /*!< Data Length */ /* The bit stream contains DATLEN + 1 data bits. */
    if (ssc->write_reg_bits("SSC_RFMR", "LOOP",         0x0                 ) < 0)  { REG_W_ERR; }; /*!< Loop Mode */
    if (ssc->write_reg_bits("SSC_RFMR", "MSBF",         0x1                 ) < 0)  { REG_W_ERR; }; /*!< Most Significant Bit First */
    if (ssc->write_reg_bits("SSC_RFMR", "DATNB",        DATNB(4)            ) < 0)  { REG_W_ERR; }; /*!< Data Number per Frame */ /* Number of data to be transferred for each start event, which is equal to (DATNB + 1). */
    if (ssc->write_reg_bits("SSC_RFMR", "FSLEN",        0x1                 ) < 0)  { REG_W_ERR; }; /*!< Receive Frame Sync Length */
    if (ssc->write_reg_bits("SSC_RFMR", "FSOS",         0x2                 ) < 0)  { REG_W_ERR; }; /*!< Frame Sync Edge Detection */ /* 2 -> Positive Pulse, RF pin is an output */

    /* SSC Transmit Clock Mode Register */
    if (ssc->write_reg_bits("SSC_TCMR", "CKS",          0x1     ) < 0)  { REG_W_ERR; }; /*!< Transmit Clock Selection */ /* 1 -> RK Clock signal */
    if (ssc->write_reg_bits("SSC_TCMR", "CKO",          0x1     ) < 0)  { REG_W_ERR; }; /*!< Transmit Clock Output Mode Selection */ /* 1 -> Continuous Transmit Clock, TK pin is an output */

    /* 1 -> The data outputs (Data and Frame Sync signals) are shifted out on Transmit Clock rising edge.
     * 0 -> The data outputs (Data and Frame Sync signals) are shifted out on Transmit Clock falling edge. */
    if (ssc->write_reg_bits("SSC_TCMR", "CKI",          0x1     ) < 0)  { REG_W_ERR; }; /*!< Transmit Clock Inversion */

    if (ssc->write_reg_bits("SSC_TCMR", "CKG",          0x0     ) < 0)  { REG_W_ERR; }; /*!< Transmit Clock Gating Selection */ /* 0 -> None */

    /* 4 : TF_FALLING : Detection of a falling edge on TF signal
     * 2 : TF_LOW : Detection of a low level on TF signal
     * 5 : TF_RISING : Detection of a rising edge on TF signal
     * 6 : TF_LEVEL : Detection of any level change on TF signal
     * */
    if (ssc->write_reg_bits("SSC_TCMR", "START",        0x4     ) < 0)  { REG_W_ERR; }; /*!< Transmit Start Selection */
    if (ssc->write_reg_bits("SSC_TCMR", "STTDLY",       0x0     ) < 0)  { REG_W_ERR; }; /*!< Transmit Start Delay */ /* If STTDLY != 0, a delay of STTDLY clock cycles is inserted */

    /* If not 0, a period signal is generated at each 2 × (PERIOD + 1) Transmit Clock.
     * Period signal is generated at each 2 × (PERIOD + 1) Transmit Clock */
    if (ssc->write_reg_bits("SSC_TCMR", "PERIOD",       PERIOD_BITS(64)  ) < 0)  { REG_W_ERR; }; /*!< Transmit Period Divider Selection */
    /* SSC Transmit Frame Mode Register */
    if (ssc->write_reg_bits("SSC_TFMR", "DATLEN",       DATLEN_BITS(16)  ) < 0)  { REG_W_ERR; }; /*!< Data Length */ /* The bit stream contains DATLEN + 1 data bits. */
    if (ssc->write_reg_bits("SSC_TFMR", "DATDEF",       0x0     ) < 0)  { REG_W_ERR; }; /*!< Data Default Value */
    if (ssc->write_reg_bits("SSC_TFMR", "MSBF",         0x1     ) < 0)  { REG_W_ERR; }; /*!< Most Significant Bit First */

    /* Number of data to be transferred for each start event, which is equal to (DATNB + 1). */
    if (ssc->write_reg_bits("SSC_TFMR", "DATNB",        DATNB(4)     ) < 0)  { REG_W_ERR; }; /*!< Data Number per Frame */

    /* 2 -> Positive Pulse, TF pin is an output */
    if (ssc->write_reg_bits("SSC_TFMR", "FSOS",         0x2     ) < 0)  { REG_W_ERR; }; /*!< Transmit Frame Sync Output Selection */
    if (ssc->write_reg_bits("SSC_TFMR", "FSDEN",        0x0     ) < 0)  { REG_W_ERR; }; /*!< Frame Sync Data Enable */

    /* Determines which edge on frame synchronization will generate the interrupt TXSYN */
    if (ssc->write_reg_bits("SSC_TFMR", "FSEDGE",       0x1     ) < 0)  { REG_W_ERR; }; /*!< Frame Sync Edge Detection */

    /* Pulse length is equal to FSLEN + (FSLEN_EXT × 16) + 1 Transmit Clock period. */
    if (ssc->write_reg_bits("SSC_TFMR", "FSLEN",        0x0     ) < 0)  { REG_W_ERR; }; /*!< Transmit Frame Sync Length */
    if (ssc->write_reg_bits("SSC_TFMR", "FSLEN_EXT",    0x0     ) < 0)  { REG_W_ERR; }; /*!< Extends FSLEN field */

    /* SSC Interrupt Disable Register */
    if (ssc->write_reg_bits("SSC_IDR", "TXRDY",         0x1     ) < 0)  { REG_W_ERR; }; /*!< Transmit Ready Interrupt Disable */
    if (ssc->write_reg_bits("SSC_IDR", "TXEMPTY",       0x1     ) < 0)  { REG_W_ERR; }; /*!< Transmit Empty Interrupt Disable */
    if (ssc->write_reg_bits("SSC_IDR", "RXRDY",         0x1     ) < 0)  { REG_W_ERR; }; /*!< Receive Ready Interrupt Disable */
    if (ssc->write_reg_bits("SSC_IDR", "OVRUN",         0x1     ) < 0)  { REG_W_ERR; }; /*!< Receive Overrun Interrupt Disable */
    if (ssc->write_reg_bits("SSC_IDR", "CP0",           0x1     ) < 0)  { REG_W_ERR; }; /*!< Compare 0 Interrupt Disable */
    if (ssc->write_reg_bits("SSC_IDR", "CP1",           0x1     ) < 0)  { REG_W_ERR; }; /*!< Compare 1 Interrupt Disable */
    if (ssc->write_reg_bits("SSC_IDR", "TXSYN",         0x1     ) < 0)  { REG_W_ERR; }; /*!< Tx Sync Interrupt Disable */
    if (ssc->write_reg_bits("SSC_IDR", "RXSYN",         0x1     ) < 0)  { REG_W_ERR; }; /*!< Rx Sync Interrupt Disable */

    /* SSC Control Register */
    if (ssc->write_reg_bits("SSC_CR", "RXEN",           0x1     ) < 0)  { REG_W_ERR; };
    if (ssc->write_reg_bits("SSC_CR", "TXEN",           0x1     ) < 0)  { REG_W_ERR; };
    ssc->read_all_regs();

#if (0)
    uint32_t txrdy = 0;
    for (int i = 0; i < 1024*16; i++)
    {
        ssc->read_reg_bits("SSC_SR", "TXRDY", &txrdy);
        if (txrdy)
        {
            if (ssc->write_reg_bits("SSC_THR", "TDAT", 0xAAAA) < 0)  { REG_W_ERR; };
        }
        else
        {
            usleep(1); printf("!");
        }
    } //end for
#else
    for (int i = 0; i < 1024; i++)
    {
        ssc->write_data(0xffffaaaa);
    }
#endif




    ssc->read_all_regs();
    delete ssc;
    return 0;
}
