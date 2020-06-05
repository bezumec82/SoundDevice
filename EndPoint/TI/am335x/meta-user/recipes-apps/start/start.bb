#
# This file is the netscript recipe.
#

SUMMARY = "Initialization procedures"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " file://start.sh               \
            file://max9867_i2c_mod.ko     \
            file://mcasp_pad_clk.ko       \
            file://mcasp_rpmsg.ko         \
            file://pru0_mcasp0_rpcmsg.out \
            file://pru1_mcasp1_rpcmsg.out \
          "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

#inherit systemd
inherit update-rc.d
#inherit kernel

INITSCRIPT_NAME   = "start.sh"
INITSCRIPT_PARAMS = "start 99 S ."

S = "${WORKDIR}"

PR = "r8"

INSANE_SKIP_${PN}     += "arch"
INSANE_SKIP_${PN}-dbg += "arch"

do_install() {
        install -d ${D}/lib
        install -d ${D}/${sysconfdir}/init.d
        install -d ${D}/${sysconfdir}/rc5.d

        install -m 0755 ${WORKDIR}/start.sh ${D}/${sysconfdir}/init.d/start.sh
        ln -sf           ../init.d/start.sh ${D}/${sysconfdir}/rc5.d/S99start.sh

        #mkdir   -p ${D}/lib/modules
        mkdir   -p ${D}/lib/modules/audio
        cp  ${S}/max9867_i2c_mod.ko ${D}/lib/modules/audio/max9867_i2c_mod.ko
        cp  ${S}/mcasp_pad_clk.ko   ${D}/lib/modules/audio/mcasp_pad_clk.ko
        cp  ${S}/mcasp_rpmsg.ko     ${D}/lib/modules/audio/mcasp_rpmsg.ko

        mkdir   -p ${D}/lib/firmware
        cp ${S}/pru0_mcasp0_rpcmsg.out ${D}/lib/firmware
        cp ${S}/pru1_mcasp1_rpcmsg.out ${D}/lib/firmware
}

FILES_${PN} += "${sysconfdir}/* \
                /lib/*          \
               "
