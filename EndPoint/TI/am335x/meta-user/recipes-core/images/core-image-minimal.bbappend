IMAGE_INSTALL_append = " eagle "
IMAGE_INSTALL_append = " ssh-keys "
IMAGE_INSTALL_append = " jrtp "
IMAGE_INSTALL_append = " start "
IMAGE_INSTALL_append = " soxr "

addtask create_my_symlinks \
after do_rootfs \
before do_image
do_create_my_symlinks () {
    ln -s /usr/lib/libjrtp.so.3.11.1    ${IMAGE_ROOTFS}/usr/lib/libjrtp.so
    ln -s /usr/lib/libjrtp.so.3.11.1    ${IMAGE_ROOTFS}/lib/libjrtp.so
    ln -s /lib/libsoxr.so.0.1.0         ${IMAGE_ROOTFS}/lib/libsoxr.so
    ln -s /lib/libsoxr-lsr.so.0.1.9     ${IMAGE_ROOTFS}/lib/libsoxr-lsr.so
    ln -s /lib/ld-linux-armhf.so.3      ${IMAGE_ROOTFS}/lib/ld-linux.so.3
}
