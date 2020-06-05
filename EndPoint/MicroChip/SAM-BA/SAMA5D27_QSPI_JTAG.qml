import SAMBA 3.2
import SAMBA.Connection.Serial 3.2
import SAMBA.Connection.JLink 3.2
import SAMBA.Device.SAMA5D2 3.2

//SerialConnection {
JLinkConnection {

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

		print ("Erasing all memory")
		applet.erase(0, applet.memorySize)

		print ("Write file 'boot.bin'.")
		applet.write(0x00000000, "boot.bin", true)
        print ("BootStrap is written at addr. 0x00000000.\r\n")
        
        print ("Write file 'u-boot.bin'.")
		applet.write(0x00010000, "u-boot.bin", true)
        print ("U-Boot is written at addr. 0x00010000.\r\n")
        
        //print ("Write file 'sama5d27_sound_device.dtb'.")
		//applet.write(0x000c0000, "sama5d27_sound_device.dtb", true)
        //print ("DTB is written at addr. 0x000c0000.\r\n")
        
        //print ("Write file 'zImage'.")
		//applet.write(0x000e0000, "zImage", true)
        //print ("Kernel is written at addr. 0x000e0000.\r\n")
        
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
