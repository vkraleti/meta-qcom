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
UBOOT_FIT_ARM_TRUSTED_FIRMWARE_LOADADDRESS ?= "0xBC300000"
UBOOT_FIT_ARM_TRUSTED_FIRMWARE_ENTRYPOINT ?= "0xBC300000"
UBOOT_FIT_TEE_LOADADDRESS ?= "0xBC600000"
UBOOT_FIT_TEE_ENTRYPOINT ?= "0xBC600000"
UBOOT_FIT_UBOOT_LOADADDRESS ?= "0xD8D00000"
UBOOT_FIT_UBOOT_ENTRYPOINT ?= "0xD8D00000"

# Recipes providing BL31 and OP-TEE for the FIT.
QCOM_UBOOT_SPL_FIT_ATF ?= "trusted-firmware-a-qcom-nord"
QCOM_UBOOT_SPL_FIT_TEE ?= "optee-os-qcom-nord"

# The Nord SPL loads a FIT with extra image data.
SPL_MKIMAGE_ARGS ?= "-E"

# The TZ access control config, is loaded along with BL31, OP-TEE and U-Boot.
QCOM_UBOOT_SPL_FIT_EXTRA_LOADABLES ?= "firmware-qcom-boot-nord:tz_ac_config.bin"

UBOOT_FIT_TZ_AC_CONFIG_LOADADDRESS ?= "0xBC000000"
UBOOT_FIT_TZ_AC_CONFIG_ENTRYPOINT ?= "0xBC000000"

UBOOT_FIT_USER_SETTINGS ?= "\
        tz_ac_config {\n\
            description = \"TZ Access Control Config\";\n\
            data = /incbin/(\"tz_ac_config.bin\");\n\
            type = \"firmware\";\n\
            arch = \"${UBOOT_ARCH}\";\n\
            load = <${UBOOT_FIT_TZ_AC_CONFIG_LOADADDRESS}>;\n\
            entry = <${UBOOT_FIT_TZ_AC_CONFIG_ENTRYPOINT}>;\n\
            compression = \"none\";\n\
${QCOM_UBOOT_SPL_FIT_IMAGE_SIGN}\
        };\n\
"

UBOOT_FIT_CONF_USER_LOADABLES ?= ", \"tz_ac_config\""
