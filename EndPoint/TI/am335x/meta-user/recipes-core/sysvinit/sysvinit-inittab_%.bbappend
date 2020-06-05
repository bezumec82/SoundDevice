FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI +=  "file://inittab \
             file://autologin"

PR .= ".2"

do_install_append() {
        install -d ${D}${sysconfdir}
        install -d ${D}$/bin
     
        install -m 0755 ${WORKDIR}/inittab      ${D}${sysconfdir}/inittab
        install -m 0755 ${WORKDIR}/autologin    ${D}/bin/autologin
}

FILES_${PN} += "/bin"

