DESCRIPTION = "CDT (Configuration Data Table) Firmware for Qualcomm Nord platform"


SRC_URI = " \
    file:///local/mnt/workspace/vkraleti/GitHub/nord_ridesx_cdt.bin;name=iq10-rrd-cdt \
    "
SRC_URI[iq10-rrd-cdt.sha256sum] = "1234"

QCOM_CDT_SUBDIR = "iq10-rrd/ufs"

include firmware-qcom-cdt-common.inc
