import QtQuick 2.3
import SAMBA.Device.SAMA5D2 3.2

/*!
	\qmltype SAMA5D27SOM1EK
	\inqmlmodule SAMBA.Device.SAMA5D2
*/
SAMA5D2 {
	name: "sama5d27-SoundDev"
	aliases: [ "sama5d27-SoundDev" ]
	description: "SAMA5D27-SoundDev"

	config {
		serial {
			instance: 1 /* UART1 */
			ioset: 1
		}

		// W971G16SG
		extram {
			preset: 8
		}

		// No such shit on SoundDev
		sdmmc {
		}

		// No SerialFlash on this board
		serialflash {
		}

		// No NAND on this board
		nandflash {
		}

		// QSPI1, I/O Set 2, 66MHz
		qspiflash {
			instance: 1
			ioset: 2
			freq: 66
		}
	}
}
