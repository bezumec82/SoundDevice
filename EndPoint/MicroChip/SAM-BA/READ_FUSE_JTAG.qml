import SAMBA 3.2
import SAMBA.Connection.Serial 3.2
import SAMBA.Connection.JLink 3.2
import SAMBA.Device.SAMA5D2 3.2
import "sama5d2-fuse.js" as Fuse

JLinkConnection {

	device: SAMA5D2 {
	}

	onConnectionOpened: {
		print("Enable Secure Fuse Controller clock in Power Management Controller")
		Fuse.enableSFC(this)

		print("Read and display the 24 fuse data registers")
		for (var i = 0; i < 24; i++)
			print("FUSE_DR" + i + " = " + Utils.hex(Fuse.readFuse(this, i), 8))

		print("Disable Secure Fuse Controller clock in Power Management Controller")
		Fuse.disableSFC(this)
	}
}
