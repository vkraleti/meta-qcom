DESCRIPTION = "CDT (Configuration Data Table) Firmware for Qualcomm Nord platform"

CDT_ARTIFACTORY_NORD = "artifactory-las.qualcomm.com/artifactory/lint-lv-local/nord-test"

SRC_URI = " \
    https://${CDT_ARTIFACTORY_NORD}/NORD_RIDESX_CDT.zip;downloadfilename=cdt-nord_${PV}.zip;name=cdt-nord \
    "
SRC_URI[cdt-nord.sha256sum] = "645f21677f27f6c2ca897183ef8a5a82e2181ba83e4e6cf06a07e45906130a9e"

QCOM_CDT_SUBDIR = "nord"

include firmware-qcom-cdt-common.inc
