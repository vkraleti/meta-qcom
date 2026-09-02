require trusted-firmware-a-qcom.inc

DEPENDS += "optee-os-qcom-nord"

TFA_PLATFORM = "nord"
FIP_ELF_ADDR = "0xaf000000"

# Fetch sources from Nord specific branch
SRC_URI = "git://github.com/qualcomm-linux/trusted-firmware-a.git;protocol=https;name=tfa-nord;branch=early/hwe/nord"
SRCREV_tfa-nord = "d6875d8bc0987b2c721f80b62d32ce36f1c8cedc"
