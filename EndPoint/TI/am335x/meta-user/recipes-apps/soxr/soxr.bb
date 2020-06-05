SUMMARY = "RTP pre-built library copy to rootfs"
SECTION = "libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
 
SRC_URI = " \
    file://./include/soxr.h                 \
    file://./include/soxr-lsr.h             \
    file://./lib/libsoxr.so.0.1.0           \
    file://./lib/libsoxr-lsr.so.0.1.9       \
"

PR = "r1"

do_install() {
    install -d ${D}${libdir}    
    install -d ${D}${includedir}
    
    install -m 0755    ${WORKDIR}/include/*.h               ${D}${includedir}
    install -m 0755    ${WORKDIR}/lib/libsoxr.so.0.1.0      ${D}${libdir}
    install -m 0755    ${WORKDIR}/lib/libsoxr-lsr.so.0.1.9  ${D}${libdir}
}

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
INSANE_SKIP_${PN}               = "ldflags"
INSANE_SKIP_${PN}-dev           = "ldflags"
INSANE_SKIP_${PN}              += "already-stripped"

FILES_${PN} = "${libdir}/*.so.* ${includedir}/*"
FILES_${PN}-dev = "${libdir}/*.so"
