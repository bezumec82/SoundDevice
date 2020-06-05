DESCRIPTON = "Startup scripts"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS_append = " update-rc.d-native"
RDEPENDS_${PN} = "initd-functions"

SRC_URI = " file://ld_fix.sh \
          "

# Recipe revision - don't forget to 'bump' when a new revision is created !
PR = "r6"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit allarch systemd
inherit update-rc.d

INITSCRIPT_NAME         = "ld_fix.sh"
INITSCRIPT_PARAMS       = "start 98 S ."

do_install() {
        install -d ${D}/${sysconfdir}/init.d
        install -d ${D}/${sysconfdir}/rc5.d
         
        install -m 0755 ${WORKDIR}/ld_fix.sh     ${D}/${sysconfdir}/init.d/ld_fix.sh
        ln -sf ../init.d/ld_fix.sh               ${D}/${sysconfdir}/rc5.d/S98ld_fix.sh
        
        update-rc.d -r ${D} ld_fix.sh start 98 S .
}

FILES_${PN} += "${sysconfdir}/*"

