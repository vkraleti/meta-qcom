# Configurations and variables for Nord SoC family.

SOC_FAMILY = "nord"
require conf/machine/include/qcom-base.inc
require conf/machine/include/qcom-common.inc

DEFAULTTUNE = "armv8-6a-crypto"
require conf/machine/include/arm/arch-armv8-6a.inc

MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS += " \
    packagegroup-qcom-boot-essential \
    packagegroup-machine-essential-qcom-nord-soc \
"

MACHINE_EXTRA_RRECOMMENDS += " \
    packagegroup-qcom-boot-additional \
"

# Memory layout needed by the SPL FIT boot flow.
QCOM_UBOOT_SPL_ENTRY ?= "0x0C219000"
UBOOT_FIT_ARM_TRUSTED_FIRMWARE_LOADADDRESS ?= "0xBC300000"
UBOOT_FIT_ARM_TRUSTED_FIRMWARE_ENTRYPOINT ?= "0xBC300000"
UBOOT_FIT_TEE_LOADADDRESS ?= "0xBC600000"
UBOOT_FIT_TEE_ENTRYPOINT ?= "0xBC600000"
UBOOT_FIT_UBOOT_LOADADDRESS ?= "0xD8D00000"
UBOOT_FIT_UBOOT_ENTRYPOINT ?= "0xD8D00000"
UBOOT_FIT_TZ_AC_CONFIG_LOADADDRESS ?= "0xBC000000"
UBOOT_FIT_TZ_AC_CONFIG_ENTRYPOINT ?= "0xBC000000"

# Recipes providing BL31 and OP-TEE for the FIT.
QCOM_UBOOT_SPL_FIT_ATF ?= "trusted-firmware-a-qcom-nord"
QCOM_UBOOT_SPL_FIT_TEE ?= "optee-os-qcom-nord"

# TZ Access Control Config blob is provided by firmware-qcom-boot-nord
# and loaded by the SPL alongside BL31/OP-TEE/U-Boot.
QCOM_UBOOT_SPL_FIT_TZ_AC_CONFIG ?= "firmware-qcom-boot-nord"

UBOOT_FIT_TZ_AC_CONFIG = "1"
