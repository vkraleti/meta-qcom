DESCRIPTION = "QCOM NHLOS Firmware for Qualcomm IQ10-RRD platform"
LICENSE = "LICENSE.qcom-2"
LIC_FILES_CHKSUM = "file://${UNPACKDIR}/LICENSE.${BPN};md5=6e1bae7ef13289c332a27b917fb49764"

# TODO: replace with the real artifactory path and bundle name for nord/iq10-rrd
FW_ARTIFACTORY = "softwarecenter.qualcomm.com/nexus/generic/product/chip/tech-package/<PLACEHOLDER>"
BOOTBINARIES = "Nord_bootbinaries"

S = "${UNPACKDIR}/${BOOTBINARIES}/ufs"

SRC_URI = " \
    https://${FW_ARTIFACTORY}/${PV}/${BOOTBINARIES}_${PV}.zip;name=bootbinaries \
    https://${FW_ARTIFACTORY}/${PV}/LICENSE.txt;downloadfilename=LICENSE.${BPN};name=license \
    "
# TODO: replace with the real license sha256sum for nord/iq10-rrd
SRC_URI[license.sha256sum] = "<PLACEHOLDER>"

QCOM_BOOT_IMG_SUBDIR = "iq10-rrd/ufs"

include firmware-qcom-boot-common.inc
