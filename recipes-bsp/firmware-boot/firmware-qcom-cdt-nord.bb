DESCRIPTION = "CDT (Configuration Data Table) Firmware for Qualcomm Nord platform"

# TODO: replace with the real CDT artifactory path, filename, and sha256sum for nord/iq10-rrd
SRC_URI = " \
    https://${CDT_ARTIFACTORY}/<PLACEHOLDER>/cdt/<PLACEHOLDER>.zip;downloadfilename=<PLACEHOLDER>_${PV}.zip;name=iq10-rrd-cdt \
    "
SRC_URI[iq10-rrd-cdt.sha256sum] = "<PLACEHOLDER>"

QCOM_CDT_SUBDIR = "iq10-rrd/ufs"

include firmware-qcom-cdt-common.inc
