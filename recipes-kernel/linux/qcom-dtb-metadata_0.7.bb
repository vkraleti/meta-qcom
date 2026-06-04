SUMMARY = "Device Tree metadata blob for QCOM platforms"
HOMEPAGE = "https://github.com/qualcomm-linux/qcom-dtb-metadata"

LICENSE = "BSD-3-Clause-Clear"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=2998c54c288b081076c9af987bdf4838"

DEPENDS += "dtc-native"

SRC_URI = "git://github.com/qualcomm-linux/qcom-dtb-metadata.git;branch=main;protocol=https;tag=v${PV}"

SRC_URI:append = " \
                   file://0001-qcom-metadata-add-support-for-ITP-platform.patch \
                   file://0002-qcom-metadata-remove-hyphens-from-Shikra-SoC-names.patch \
		 "

SRCREV = "e08e4e175779587c6ed91f3f4fd620ba722f5ae2"

INHIBIT_DEFAULT_DEPS = "1"

do_configure[noexec] = "1"

inherit deploy

do_deploy() {
    install -m 0644 ${B}/qcom-metadata.dtb -D ${DEPLOYDIR}/qcom-metadata.dtb
}

addtask deploy after do_compile before do_build
