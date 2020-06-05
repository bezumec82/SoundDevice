DESCRIPTON = "GREATing eagle"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " file://ssh_host_dsa_key          \
            file://ssh_host_dsa_key.pub      \
            file://ssh_host_ecdsa_key        \
            file://ssh_host_ecdsa_key.pub    \
            file://ssh_host_ed25519_key      \
            file://ssh_host_ed25519_key.pub  \
            file://ssh_host_rsa_key          \
            file://ssh_host_rsa_key.pub      \
          "
          
S = "${WORKDIR}"

do_install() {
         install -d ${D}/${sysconfdir}/ssh

         install -m 0400 ${S}/ssh_host_dsa_key          ${D}/${sysconfdir}/ssh/ssh_host_dsa_key        
         install -m 0400 ${S}/ssh_host_dsa_key.pub      ${D}/${sysconfdir}/ssh/ssh_host_dsa_key.pub    
         install -m 0400 ${S}/ssh_host_ecdsa_key        ${D}/${sysconfdir}/ssh/ssh_host_ecdsa_key      
         install -m 0400 ${S}/ssh_host_ecdsa_key.pub    ${D}/${sysconfdir}/ssh/ssh_host_ecdsa_key.pub  
         install -m 0400 ${S}/ssh_host_ed25519_key      ${D}/${sysconfdir}/ssh/ssh_host_ed25519_key
         install -m 0400 ${S}/ssh_host_ed25519_key.pub  ${D}/${sysconfdir}/ssh/ssh_host_ed25519_key.pub
         install -m 0400 ${S}/ssh_host_rsa_key          ${D}/${sysconfdir}/ssh/ssh_host_rsa_key
         install -m 0400 ${S}/ssh_host_rsa_key.pub      ${D}/${sysconfdir}/ssh/ssh_host_rsa_key.pub
}

FILES_${PN} = "${sysconfdir}/* ${sysconfdir}/ssh/*"
