SECTION = "kernel"

DESCRIPTION = "Linux ${PV} kernel for QCOM devices"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel cml1

COMPATIBLE_MACHINE = "(qcom)"

LINUX_QCOM_FIT_DTB_COMPATIBLE = "conf/machine/include/fit-dtb-compatible-linux-qcom.inc"

LINUX_VERSION ?= "7.2-rc2"

PV = "${LINUX_VERSION}+git"

# RB tree
# SRCREV ?= "5586df3db6e0a3da8090d6ebee51d2748aade539"
# SRC_URI = "git://github.com/EmbeddedAndroid/linux.git;branch=early/hwe/nord-next-oss-firmware;protocol=https"

# Quest Tree
SRCREV ?= "d541a9577d436abda89e092c6a7eea482ba437b6"
SRC_URI = "git://github.com/qualcomm-linux/kernel-topics.git;branch=early/hwe/nord-next;protocol=https"

# Additional kernel configs.
SRC_URI += " \
    file://configs/bsp-additions.cfg \
    file://0001-arm64-dts-qcom-nord-Add-support-for-ADSP.patch \
    file://0002-arm64-dts-qcom-iq10-rrd-Enable-ADSP-remoteproc.patch \
    file://0003-remoteproc-qcom-pas-Add-support-for-Nord-CDSPs.patch \
    file://0004-dt-bindings-misc-qcom-fastrpc-Add-label-for-cdsp2-an.patch \
    file://0005-arm64-dts-qcom-nord-Add-support-for-CDSPs.patch \
    file://0006-DEBUG-remoteproc-Serialize-rproc-boot-start.patch \
    file://0001-dt-binding-qcom-snps-dwc3-Add-Nord-compatible-to-sup.patch \
    file://0002-dt-bindings-phy-qcom-sc8280xp-qmp-usb43dp-phy-Add-No.patch \
    file://0003-dt-bindings-phy-qcom-snps-eusb2-phy-Document-eUSB2-P.patch \
    file://0004-phy-qcom-qmp-combo-Add-qmp-configuration-for-Nord.patch \
    file://0005-arm64-dts-qcom-Add-changes-to-enable-primary-USB-con.patch \
    file://0006-remove-addressing-and-add-status-enbaled-in-actual-n.patch \
    file://0007-move-usb-node-from-under-ufshc-to-soc.patch \
    file://0008-WORKAROUND-Make-usb3_prim_phy_pipe_clk-as-BRANCH_HAL.patch \
"

S = "${UNPACKDIR}/${BP}"

KBUILD_DEFCONFIG ?= "defconfig"
KBUILD_DEFCONFIG:qcom-armv7a = "qcom_defconfig"

do_configure:prepend() {
    # Use a copy of the 'defconfig' from the actual repo to merge fragments
    cp ${S}/arch/${ARCH}/configs/${KBUILD_DEFCONFIG} ${B}/.config

    # Merge fragment for QCOM value add features
    ${S}/scripts/kconfig/merge_config.sh -m -O ${B} ${B}/.config ${KBUILD_CONFIG_EXTRA} ${@" ".join(find_cfgs(d))}
}

INSANE_SKIP:append = " buildpaths"
