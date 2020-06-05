import SAMBA 3.2
import SAMBA.Connection.Serial 3.2
import SAMBA.Connection.JLink 3.2
import SAMBA.Device.SAMA5D2 3.2

//JLinkConnection {
SerialConnection {
    port: "ttyACM0"
    
	device: SAMA5D2 {
		config {
			qspiflash {
				instance: 1
				ioset: 2
				freq: 66
			}
		}
	}

	onConnectionOpened: {
		print ("Initializing QSPI applet.")
		initializeApplet("qspiflash")
		print ("'qspiflash' applet is initialized.\r\n")

		// print ("Erasing all memory")
		// applet.erase(0, applet.memorySize)

		print ("Write file 'at91bootstrap.bin'.")
		applet.write(0x00000, "at91bootstrap.bin", true)
        print ("'at91bootstrap.bin' is written.\r\n")
        
		print ("initialize 'bootconfig' applet")
		initializeApplet("bootconfig")
        print ("'bootconfig' applet is initialized.\r\n")

		print ("Use BUREG0 as boot configuration word")
		applet.writeBootCfg(BootCfg.BSCR, BSCR.fromText("VALID,BUREG0"))
        
		print ("Enable external boot only on QSPI1 IOSET2")
		applet.writeBootCfg(BootCfg.BUREG0,
			BCW.fromText("EXT_MEM_BOOT,UART1_IOSET1,JTAG_IOSET3," +
						 "SDMMC1_DISABLED,SDMMC0_DISABLED," +
						 "SPI1_DISABLED,SPI0_DISABLED," +
						 "QSPI0_DISABLED,QSPI1_IOSET2," +
                         "NFC_DISABLED"))
	}
}
