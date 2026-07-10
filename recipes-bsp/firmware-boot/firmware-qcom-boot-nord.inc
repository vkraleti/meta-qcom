DESCRIPTION = "QCOM NHLOS Firmware for Qualcomm IQ10-RRD platform"
LICENSE = "LICENSE.qcom-2"
LIC_FILES_CHKSUM = "file://${UNPACKDIR}/${BOOTBINARIES}/LICENSE.qcom-2;md5=165287851294f2fb8ac8cbc5e24b02b0"

BOOTBINARIES = "NORD_bootbinaries"

S = "${UNPACKDIR}/${BOOTBINARIES}/ufs"

SRC_URI = " \
    file:///local/mnt/workspace/vkraleti/GitHub/NORD_bootbinaries.zip \
    "

QCOM_BOOT_IMG_SUBDIR = "iq10-rrd/ufs"

include firmware-qcom-boot-common.inc
