SUMMARY = "RTP pre-built library copy to rootfs"
SECTION = "libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
 
SRC_URI = " \
    file://./include/jrtplib3/rtcpapppacket.h                 \
    file://./include/jrtplib3/rtcpbyepacket.h                 \
    file://./include/jrtplib3/rtcpcompoundpacket.h            \
    file://./include/jrtplib3/rtcpcompoundpacketbuilder.h     \
    file://./include/jrtplib3/rtcppacket.h                    \
    file://./include/jrtplib3/rtcppacketbuilder.h             \
    file://./include/jrtplib3/rtcprrpacket.h                  \
    file://./include/jrtplib3/rtcpscheduler.h                 \
    file://./include/jrtplib3/rtcpsdesinfo.h                  \
    file://./include/jrtplib3/rtcpsdespacket.h                \
    file://./include/jrtplib3/rtcpsrpacket.h                  \
    file://./include/jrtplib3/rtcpunknownpacket.h             \
    file://./include/jrtplib3/rtpabortdescriptors.h           \
    file://./include/jrtplib3/rtpaddress.h                    \
    file://./include/jrtplib3/rtpbyteaddress.h                \
    file://./include/jrtplib3/rtpcollisionlist.h              \
    file://./include/jrtplib3/rtpconfig.h                     \
    file://./include/jrtplib3/rtpdebug.h                      \
    file://./include/jrtplib3/rtpdefines.h                    \
    file://./include/jrtplib3/rtperrors.h                     \
    file://./include/jrtplib3/rtpexternaltransmitter.h        \
    file://./include/jrtplib3/rtpfaketransmitter.h            \
    file://./include/jrtplib3/rtphashtable.h                  \
    file://./include/jrtplib3/rtpinternalsourcedata.h         \
    file://./include/jrtplib3/rtpipv4address.h                \
    file://./include/jrtplib3/rtpipv4destination.h            \
    file://./include/jrtplib3/rtpipv6address.h                \
    file://./include/jrtplib3/rtpipv6destination.h            \
    file://./include/jrtplib3/rtpkeyhashtable.h               \
    file://./include/jrtplib3/rtplibraryversion.h             \
    file://./include/jrtplib3/rtpmemorymanager.h              \
    file://./include/jrtplib3/rtpmemoryobject.h               \
    file://./include/jrtplib3/rtppacket.h                     \
    file://./include/jrtplib3/rtppacketbuilder.h              \
    file://./include/jrtplib3/rtppollthread.h                 \
    file://./include/jrtplib3/rtprandom.h                     \
    file://./include/jrtplib3/rtprandomrand48.h               \
    file://./include/jrtplib3/rtprandomrands.h                \
    file://./include/jrtplib3/rtprandomurandom.h              \
    file://./include/jrtplib3/rtprawpacket.h                  \
    file://./include/jrtplib3/rtpsecuresession.h              \
    file://./include/jrtplib3/rtpselect.h                     \
    file://./include/jrtplib3/rtpsession.h                    \
    file://./include/jrtplib3/rtpsessionparams.h              \
    file://./include/jrtplib3/rtpsessionsources.h             \
    file://./include/jrtplib3/rtpsocketutil.h                 \
    file://./include/jrtplib3/rtpsourcedata.h                 \
    file://./include/jrtplib3/rtpsources.h                    \
    file://./include/jrtplib3/rtpstructs.h                    \
    file://./include/jrtplib3/rtptcpaddress.h                 \
    file://./include/jrtplib3/rtptcptransmitter.h             \
    file://./include/jrtplib3/rtptimeutilities.h              \
    file://./include/jrtplib3/rtptransmitter.h                \
    file://./include/jrtplib3/rtptypes.h                      \
    file://./include/jrtplib3/rtptypes_win.h                  \
    file://./include/jrtplib3/rtpudpv4transmitter.h           \
    file://./include/jrtplib3/rtpudpv6transmitter.h           \
    file://./lib/libjrtp.so.3.11.1                            \
"

PR = "r5" 

# If you need to create a symbolic link using the pre-built libraries you should use oe_soinstall.
# This copies libraries to "{TARGET_ROOTFS}/usr/lib" directory and create a symlink as
# lrwxrwxrwx libjson-c.so.4.0.0 -> libjson-c.so.4.0.0
# -rwxr-xr-x libjson-c.so.4.0.0



do_install() {
    install -d ${D}${libdir}    
    install -d ${D}${includedir}
    install -d ${D}${includedir}/jrtplib3
    
    install -m 0755    ${WORKDIR}/include/jrtplib3/*.h         ${D}${includedir}/jrtplib3
    
    #oe_soinstall      ${WORKDIR}/lib/libjrtp.so.3.11.1        ${D}${libdir}
    install -m 0755    ${WORKDIR}/lib/libjrtp.so.3.11.1        ${D}${libdir}
}

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
INSANE_SKIP_${PN}               = "ldflags"
INSANE_SKIP_${PN}-dev           = "ldflags"
#INSANE_SKIP_${PN}               = "already-stripped" # Gives errors
#INHIBIT_PACKAGE_STRIP           = "1"                # Gives errors
#INHIBIT_SYSROOT_STRIP           = "1"                # Gives errors
#INHIBIT_PACKAGE_DEBUG_SPLIT     = "1"                # Gives errors

FILES_${PN} = "${libdir}/*.so.* ${includedir}/jrtplib3/*"
FILES_${PN}-dev = "${libdir}/*.so"
