DESCRIPTION = "QCOM NHLOS Firmware for Qualcomm IQ10 Nord platform"
LICENSE = "CLOSED"

FW_ARTIFACTORY = "qartifactory.qualcomm.com/artifactory/qsc-generic-stage/product/chip/software-product/NORD_IOT.LE.0.0/nord_bootbinaries.0.0-stg-private"
BOOTBINARIES = "nord_bootbinaries"
BOOTBINARIES_REVISION_DIR = "${@str(int(d.getVar('PV'))).zfill(3)}"

SRC_URI = " \
    https://${FW_ARTIFACTORY}/${BOOTBINARIES_REVISION_DIR}/${BOOTBINARIES}_${PV}.zip;name=bootbinaries \
    "

QCOM_BOOT_IMG_SUBDIR = "nord"

include firmware-qcom-boot-common.inc

S = "${UNPACKDIR}"
