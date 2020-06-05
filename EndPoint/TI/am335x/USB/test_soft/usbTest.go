// export GOPATH=$PWD
// go get -v github.com/zaf/g711/...
// go run ./usbTest.go ./LI_GW.go

package main

import (
    "time"
    "fmt"
    //"io/ioutil"
    "sync"
    "os"
    "math"
    "github.com/zaf/g711"
    gtm "github.com/buger/goterm"
    "github.com/guptarohit/asciigraph"
)

const SIN_AMP      float64 = 15000
const DISCRET_FREQ float64 = 8000
const SIN_FREQ     float64 = 1111

//const LI_GW_HDR_SIZE = 14
//const LI_GW_CRC_SIZE = 2
const FTDI_MAGIC_SIZE    = 2
const ADATA_20_MS        = 160
const IN_PCKG_SIZE       = ADATA_20_MS + LI_GW_HDR_SIZE + LI_GW_CRC_SIZE + FTDI_MAGIC_SIZE
const OUT_PCKG_SIZE      = ADATA_20_MS + LI_GW_HDR_SIZE + LI_GW_CRC_SIZE

const DEV_TYPE uint16 = 0x1234
const MCASP0   uint16 = 0
const MCASP1   uint16 = 1
const TS0      uint16 = 0
const TS1      uint16 = 1

const SHOW_PCKG = false

const usbFileName string = "/dev/lira/8009/Headset_#1"
//const usbFileName string = "/dev/lira/8004/Speaker_#6"

func cacl_sin( sample_num float64) int16 {
    return int16(
                SIN_AMP *
                math.Sin( 2.0 * float64(math.Pi) *
                sample_num/(DISCRET_FREQ/SIN_FREQ) ) )
}

func check(e error) {
    if e != nil {
        panic(e)
    }
}

var wr_mutex = &sync.Mutex{}

func writeStream ( mcasp_idx uint16, ts_idx uint16, usbFile *os.File, period float64) {
    ticker     := time.Tick(time.Duration(period * 1e6) * time.Nanosecond)
    sin_arr    := [ADATA_20_MS]   int16 { 0 }
    pckg_arr   := [OUT_PCKG_SIZE] byte  { 0 }
    sample_num := 0.0
    for {
        for i:=0; i<ADATA_20_MS; i++ {
            sin_arr [i] = cacl_sin(sample_num)
            sample_num  = sample_num + 1.0
            pckg_arr[LI_GW_HDR_SIZE + i] = g711.EncodeAlawFrame(sin_arr[i]) //encodes a 16bit LPCM frame to G711 A-law PCM
        }
        LI_GW_pckgr ( pckg_arr[:OUT_PCKG_SIZE], ADATA_20_MS, 0, 0, mcasp_idx, ts_idx)
        <-ticker
        wr_mutex.Lock()
            _, err := usbFile.Write(pckg_arr[:OUT_PCKG_SIZE])
        wr_mutex.Unlock()
        check(err)
    }
}

func readStream(usbFile *os.File) (string, uint16){
    pckg_arr  := [IN_PCKG_SIZE] byte    { 0 }
    print_arr := [ADATA_20_MS]  float64 { 0.0 }
    var err error
    var pckg_size int
    pckg_size, err = usbFile.Read(pckg_arr[0:IN_PCKG_SIZE])
    check(err)
    if SHOW_PCKG {
        fmt.Println("recv : ", pckg_size)
    }
    ts_idx := LI_GW_disasm(pckg_arr[:IN_PCKG_SIZE], uint16(IN_PCKG_SIZE-FTDI_MAGIC_SIZE))
    if SHOW_PCKG {
        fmt.Println("pckg : ", pckg_arr)
    }

    for i:=0; i<( ADATA_20_MS ); i++ {
        print_arr[i] = float64(g711.DecodeAlawFrame(pckg_arr[LI_GW_HDR_SIZE+i]))/5000.0
        //size = i
    }
    //fmt.Println("pcm : ", print_arr, size)
    
    
    graph := asciigraph.Plot(print_arr[:ADATA_20_MS])
    return graph, ts_idx
}

func main() {
    gtm.Clear()
    gtm.MoveCursor(0, 0)
    usbFile, err := os.OpenFile(usbFileName, os.O_RDWR, 0755)
    check(err)
    
    go writeStream(MCASP0, TS0, usbFile, 20+0.1)
    go writeStream(MCASP0, TS1, usbFile, 20+0.1)
    go writeStream(MCASP1, TS0, usbFile, 20+0.1)
    go writeStream(MCASP1, TS1, usbFile, 20+0.1)
    
    var graph0 string
    var graph1 string
    
    refresh_rate := 0
    
    for {
        //readStream(usbFile)
        graph, stream_idx:=readStream(usbFile)
        if stream_idx == 0 {
            graph0 = graph
        }
        if stream_idx == 2 {
            graph1 = graph
        }
        if refresh_rate == 20 {
            refresh_rate = 0
            gtm.Clear()
            gtm.MoveCursor(0, 0)
            gtm.Println(graph0)
            gtm.Println(graph1)
            gtm.Flush()
        } else {
            refresh_rate = refresh_rate + 1
        }
    }
    err=usbFile.Close()
    check(err)
}
