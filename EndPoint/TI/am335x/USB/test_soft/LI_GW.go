package main
//package test
import "fmt"
import "unsafe"

//unsigned short CliraProtocol::getCRCFromData(unsigned char *msgPckg,
//                                             unsigned int msgPckgLen)
//{
//  unsigned short crc_calc = 0xAAAA;
//  unsigned short paket_word;
//  for (unsigned int i=0;i<msgPckgLen/2;i++)
//  {
//          paket_word = *(msgPckg+2*i)+(*(msgPckg+2*i+1)<<8);
//          crc_calc ^= paket_word;
//          crc_calc = ( (crc_calc<<1) &0xFFFE ) | ( (crc_calc>>15) &0x0001 );
//  }
//  return crc_calc;
//}

const LI_GW_HDR_SIZE = 14
const LI_GW_CRC_SIZE = 2
const LI_GW_MSG_ID_ALAW_DATA uint16 = 0x2000
const LI_GW_FLAG_PROTO_VER   uint16 = 1
const LI_GW_SYNC             uint32 = 0xE4E5AB2D

const PRINT_PCKG_DISASM  = false
const PRINT_PCKG_RAW_DAT = false
const PRINT_CRC_CALC     = false


///@param pckg_size - size of the package exclude CRC size
///                   audio data + LI_GW header
func calc_CRC( pckg[] uint8, pckg_size uint16 ) uint16 {
    var crc uint16 = 0xAAAA
    for i:=0; i<int(pckg_size/2); i++ {
        crc  ^= ( uint16(pckg[2*i]) ) | ( uint16(pckg[2*i+1])<<8)
        crc   = ( (crc<<1) & 0xFFFE ) | ( (crc>>15) & 0x0001 )
        if PRINT_CRC_CALC {
            fmt.Printf("0x%x ", crc)
        }
    }
    if PRINT_CRC_CALC {
        fmt.Printf("\n")
    }
    return crc
}

var send_audio_pckg_num uint32 = 1
///@param pckg_hldr - buffer where audiodata already
///                   have been placed in offset 'LI_GW_HDR_SIZE'
func LI_GW_pckgr ( pckg_hldr[] uint8 , adata_size uint16,
                   dev_type    uint16, dev_num    uint16, 
                   mcasp_idx   uint16, ts_idx     uint16 ) {
    pckg_cntr := 0
    *(*uint32)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) ) = LI_GW_SYNC
    pckg_cntr += 4
    *(*uint16)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) ) = (adata_size >> 1) + (adata_size % 2)
    pckg_cntr += 2
    *(*uint32)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) ) = send_audio_pckg_num
    send_audio_pckg_num = send_audio_pckg_num + 1
    pckg_cntr += 4
    *(*uint16)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) ) = LI_GW_MSG_ID_ALAW_DATA
    pckg_cntr += 2
    *(*uint16)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) ) = (uint16)( (LI_GW_FLAG_PROTO_VER<<14)|
                                                          (mcasp_idx           <<13)|
                                                          (ts_idx              <<12)|
                                                          (dev_num             <<8 )|
                                                          (dev_type            <<0 ) )
    *(*uint16)( unsafe.Pointer(&pckg_hldr[adata_size + LI_GW_HDR_SIZE]) ) = 
    calc_CRC(pckg_hldr, (adata_size + LI_GW_HDR_SIZE) )
    //LI_GW_disasm(pckg_hldr, adata_size + LI_GW_HDR_SIZE + LI_GW_CRC_SIZE)
    if PRINT_PCKG_RAW_DAT {
        fmt.Printf("%x\n", pckg_hldr)
    }
}

/*!
 * @return - timeslot index
 */
func LI_GW_disasm(pckg_hldr[] uint8, pckg_size uint16) uint16 {
    if pckg_size < (LI_GW_HDR_SIZE+LI_GW_CRC_SIZE){
        fmt.Printf("Wrong pckg. size.\n")
        return 0
    }
    pckg_cntr := 0

    var msgStart uint32 = *(*uint32)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) )
    if PRINT_PCKG_DISASM {
        fmt.Printf("Sync. const.: 0x%x\n", msgStart)
    }
    pckg_cntr += 4
    var msgLength uint16 = *(*uint16)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) ) << 1
    if PRINT_PCKG_DISASM {
        fmt.Printf("Pckg. size. : %d\n", msgLength)
    }
    if msgLength != pckg_size-(LI_GW_HDR_SIZE+LI_GW_CRC_SIZE) {
        fmt.Printf("Wrong pckg. size. Recv. %d. Decl. %d.\n", 
                    pckg_size-(LI_GW_HDR_SIZE+LI_GW_CRC_SIZE), msgLength)
        return 0
    }    
    pckg_cntr += 2
    var msgSeqNum uint32 = *(*uint32)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) )
    if PRINT_PCKG_DISASM {
        fmt.Printf("Pckg. num   : %d\n", msgSeqNum)
    }
    pckg_cntr += 4
    var msgID uint16 = *(*uint16)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) )
    if PRINT_PCKG_DISASM {
        fmt.Printf("Msg. ID     : 0x%x\n", msgID)
    }
    pckg_cntr += 2
    var msgFlags uint16 = *(*uint16)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) )
    if PRINT_PCKG_DISASM {
        fmt.Printf("Proto. ver. : 0x%x\n", msgFlags>>14)
        fmt.Printf("Stream idx. : 0x%x\n",(msgFlags>>12)&0x3)
        fmt.Printf("Dev. num.   : %d\n",  (msgFlags>>8 )&0x7)
        fmt.Printf("Dev. type   : %d\n",   msgFlags     &0xf)
    }
    pckg_cntr = int(pckg_size - 2)
    var crc uint16 = *(*uint16)( unsafe.Pointer(&pckg_hldr[pckg_cntr]) )
    if PRINT_PCKG_DISASM {
        fmt.Printf("CRC         : 0x%x\n", crc)
    }
    return (msgFlags>>12)&0x3;
}


func test(){
    var dev_type   uint16 = 0x12
    var dev_num    uint16 = 0xf
    var mcasp_idx  uint16 = 0x1 
    var ts_idx     uint16 = 0x1
    const adata_size uint16 = 2
    const pckg_size = adata_size + LI_GW_HDR_SIZE + LI_GW_CRC_SIZE
    pckg := [pckg_size] uint8 { 0 }
    
    LI_GW_pckgr(pckg[:pckg_size], adata_size, 
                dev_type, dev_num, mcasp_idx, ts_idx)
}
