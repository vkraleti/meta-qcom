require trusted-firmware-a-qcom.inc

DEPENDS += "optee-os-qcom-nord"

TFA_PLATFORM = "nord"
FIP_ELF_ADDR = "0xaf000000"

# Fetch sources from Nord specific branch
SRC_URI = "git://github.com/qualcomm-linux/trusted-firmware-a.git;protocol=https;name=tfa-nord;branch=early/hwe/nord"
SRCREV_tfa-nord = "1eb5ada16bb5b500b633fcd1b4003e3846509f5c"


SRC_URI += " \
    file://0001-feat-plat-qti-cpu-add-scmi-reset-protocol-support.patch \
    file://0002-feat-drivers-qti-add-rvbar-config-changes-for-nord.patch \
    file://0003-feat-plat-qti-cpu-add-multicore-support-for-nord.patch \
    file://0004-feat-drivers-qti-hwmutex-Add-hwmutex-support-for-LPM.patch \
"
