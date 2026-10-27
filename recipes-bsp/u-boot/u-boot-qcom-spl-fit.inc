# Copyright (c) 2026 Qualcomm Innovation Center, Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause-Clear
#
# FIT assembly for the SPL FIT boot flow. These functions replace the
# uboot-sign.bbclass ones of the same name, so this file has to be required
# after recipes-bsp/u-boot/u-boot.inc, which is what inherits that class.

# Qualcomm specific loadables packed into the FIT alongside atf/optee, one
# flag and /incbin/ filename per node, following the same shape as
# uboot-sign.bbclass's UBOOT_FIT_ARM_TRUSTED_FIRMWARE/UBOOT_FIT_TEE.
UBOOT_FIT_TZ_AC_CONFIG ?= "0"
UBOOT_FIT_TZ_AC_CONFIG_IMAGE ?= "tz_ac_config.bin"
UBOOT_FIT_PMIC_COCOS ?= "0"
UBOOT_FIT_PMIC_COCOS_IMAGE ?= "Pmic_Cocos.bin"
UBOOT_FIT_PMIC_WAILUA ?= "0"
UBOOT_FIT_PMIC_WAILUA_IMAGE ?= "Pmic_Wailua.bin"
UBOOT_FIT_DCB_LP4 ?= "0"
UBOOT_FIT_DCB_LP4_IMAGE ?= "900B_7_0100_0_dcb.bin"
UBOOT_FIT_DCB_LP5 ?= "0"
UBOOT_FIT_DCB_LP5_IMAGE ?= "900B_7_0100_1_dcb.bin"
UBOOT_FIT_SHRM ?= "0"
UBOOT_FIT_SHRM_IMAGE ?= "shrm.elf"
UBOOT_FIT_QCLIB ?= "0"
UBOOT_FIT_QCLIB_IMAGE ?= "QcLib.elf"
UBOOT_FIT_ECC ?= "0"
UBOOT_FIT_ECC_IMAGE ?= "Shikra_LCP.bin"

# Create the ITS file and FIT image for the U-boot FIT, with the layout the
# qcom U-Boot SPL expects: no separate "fdt" node ("uboot" already carries
# the dtb), "atf" not duplicated into "loadables" when it's already
# UBOOT_FIT_CONF_FIRMWARE, and a second "pre-ddr" configuration node for
# firmware (PMIC/DCB/SHRM/QcLib/ECC configs, ...) the SPL loads ahead of DDR
# training. Replaces uboot-sign.bbclass's uboot_fitimage_assemble()
uboot_fitimage_assemble() {
    conf_loadables="\"uboot\""
    conf_firmware=""
    rm -f ${UBOOT_ITS} ${UBOOT_FITIMAGE_BINARY}

    cat << EOF >> ${UBOOT_ITS}
/dts-v1/;

/ {
    description = "${UBOOT_FIT_DESC}";
    #address-cells = <${UBOOT_FIT_ADDRESS_CELLS}>;

    images {
        uboot {
            description = "U-Boot image";
            data = /incbin/("${UBOOT_BINARY}");
            type = "standalone";
            os = "u-boot";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_UBOOT_LOADADDRESS}>;
            entry = <${UBOOT_FIT_UBOOT_ENTRYPOINT}>;
EOF

    if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
        cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
    fi

    cat << EOF >> ${UBOOT_ITS}
        };
EOF

    if [ "${UBOOT_FIT_TEE}" = "1" ] ; then
        conf_loadables="${conf_loadables}, \"optee\""
        uboot_fitimage_optee
    fi

    if [ "${UBOOT_FIT_ARM_TRUSTED_FIRMWARE}" = "1" ] ; then
        uboot_fitimage_atf
    fi

    if [ "${UBOOT_FIT_TZ_AC_CONFIG}" = "1" ] ; then
        conf_loadables="${conf_loadables}, \"tz_ac_config\""
        uboot_fitimage_tz_ac_config
    fi

    if [ "${UBOOT_FIT_PMIC_COCOS}" = "1" ] ; then
        uboot_fitimage_pmic_cocos
    fi

    if [ "${UBOOT_FIT_PMIC_WAILUA}" = "1" ] ; then
        uboot_fitimage_pmic_wailua
    fi

    if [ "${UBOOT_FIT_DCB_LP4}" = "1" ] ; then
        uboot_fitimage_dcb_lp4
    fi

    if [ "${UBOOT_FIT_DCB_LP5}" = "1" ] ; then
        uboot_fitimage_dcb_lp5
    fi

    if [ "${UBOOT_FIT_SHRM}" = "1" ] ; then
        uboot_fitimage_shrm
    fi

    if [ "${UBOOT_FIT_QCLIB}" = "1" ] ; then
        uboot_fitimage_qclib
    fi

    if [ "${UBOOT_FIT_ECC}" = "1" ] ; then
        uboot_fitimage_ecc
    fi

    if [ -n "${UBOOT_FIT_CONF_FIRMWARE}" ] ; then
        conf_firmware="firmware = \"${UBOOT_FIT_CONF_FIRMWARE}\";"
    fi

    pre_ddr_loadables=""
    for loadable in ${QCOM_UBOOT_SPL_FIT_PRE_DDR_LOADABLES}; do
        if [ -n "${pre_ddr_loadables}" ]; then
            pre_ddr_loadables="${pre_ddr_loadables}, \"${loadable}\""
        else
            pre_ddr_loadables="\"${loadable}\""
        fi
    done

    cat << EOF >> ${UBOOT_ITS}
    };

    configurations {
        default = "post-ddr";
EOF

    if [ -n "${pre_ddr_loadables}" ] ; then
        cat << EOF >> ${UBOOT_ITS}
        pre-ddr {
            description = "pre-ddr";
            loadables = ${pre_ddr_loadables};
        };
EOF
    fi

    cat << EOF >> ${UBOOT_ITS}
        post-ddr {
            description = "${UBOOT_FIT_CONF_DESC}";
            ${conf_firmware}
            loadables = ${conf_loadables};
        };
    };
};
EOF

    #
    # Assemble the U-boot FIT image
    #
    ${UBOOT_MKIMAGE} \
        -E \
        -f ${UBOOT_ITS} \
        ${UBOOT_FITIMAGE_BINARY}

    if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
        if [ -n "${SPL_DTB_BINARY}" ] ; then
            #
            # Sign the U-boot FIT image and add public key to SPL dtb. Work
            # on a copy of the DTB so the compile output is never modified
            # in-place (see uboot-sign.bbclass's uboot_fitimage_assemble()).
            #
            cp ${SPL_DIR}/${SPL_DTB_BINARY} ${SPL_DIR}/${SPL_DTB_SIGNED}
            ${UBOOT_MKIMAGE_SIGN} \
                ${@'-D "${SPL_MKIMAGE_DTCOPTS}"' if len('${SPL_MKIMAGE_DTCOPTS}') else ''} \
                -F -k "${SPL_SIGN_KEYDIR}" \
                -K "${SPL_DIR}/${SPL_DTB_SIGNED}" \
                -r ${UBOOT_FITIMAGE_BINARY} \
                ${SPL_MKIMAGE_SIGN_ARGS}

            # Verify the U-boot FIT image and SPL dtb
            ${UBOOT_FIT_CHECK_SIGN} \
                -k "${SPL_DIR}/${SPL_DTB_SIGNED}" \
                -f ${UBOOT_FITIMAGE_BINARY}
        else
            # Sign the U-boot FIT image
            ${UBOOT_MKIMAGE_SIGN} \
                ${@'-D "${SPL_MKIMAGE_DTCOPTS}"' if len('${SPL_MKIMAGE_DTCOPTS}') else ''} \
                -F -k "${SPL_SIGN_KEYDIR}" \
                -r ${UBOOT_FITIMAGE_BINARY} \
                ${SPL_MKIMAGE_SIGN_ARGS}
        fi
    fi
}

# Create a ITS file for the tee
uboot_fitimage_optee() {
	cat << EOF >> ${UBOOT_ITS}
        optee {
            description = "Trusted Execution Environment";
            data = /incbin/("${UBOOT_FIT_TEE_IMAGE}");
            type = "tee";
            arch = "${UBOOT_ARCH}";
            os = "tee";
            compression = "none";
            load = <${UBOOT_FIT_TEE_LOADADDRESS}>;
            entry = <${UBOOT_FIT_TEE_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the TZ Access Control Config blob (nord)
uboot_fitimage_tz_ac_config() {
	cat << EOF >> ${UBOOT_ITS}
        tz_ac_config {
            description = "TZ Access Control Config";
            data = /incbin/("${UBOOT_FIT_TZ_AC_CONFIG_IMAGE}");
            type = "tee";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_TZ_AC_CONFIG_LOADADDRESS}>;
            entry = <${UBOOT_FIT_TZ_AC_CONFIG_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the PMIC Cocos config (shikra)
uboot_fitimage_pmic_cocos() {
	cat << EOF >> ${UBOOT_ITS}
        pmic_cocos {
            description = "PMIC Cocos Config";
            data = /incbin/("${UBOOT_FIT_PMIC_COCOS_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_PMIC_COCOS_LOADADDRESS}>;
            entry = <${UBOOT_FIT_PMIC_COCOS_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the PMIC Wailua config (shikra)
uboot_fitimage_pmic_wailua() {
	cat << EOF >> ${UBOOT_ITS}
        pmic_wailua {
            description = "PMIC Wailua Config";
            data = /incbin/("${UBOOT_FIT_PMIC_WAILUA_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_PMIC_WAILUA_LOADADDRESS}>;
            entry = <${UBOOT_FIT_PMIC_WAILUA_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the DDR Config Block, LP4 variant (shikra)
uboot_fitimage_dcb_lp4() {
	cat << EOF >> ${UBOOT_ITS}
        dcb_lp4 {
            description = "DDR Config Block (LP4)";
            data = /incbin/("${UBOOT_FIT_DCB_LP4_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_DCB_LP4_LOADADDRESS}>;
            entry = <${UBOOT_FIT_DCB_LP4_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the DDR Config Block, LP5 variant (shikra)
uboot_fitimage_dcb_lp5() {
	cat << EOF >> ${UBOOT_ITS}
        dcb_lp5 {
            description = "DDR Config Block (LP5)";
            data = /incbin/("${UBOOT_FIT_DCB_LP5_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_DCB_LP5_LOADADDRESS}>;
            entry = <${UBOOT_FIT_DCB_LP5_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for SHRM (shikra)
uboot_fitimage_shrm() {
	cat << EOF >> ${UBOOT_ITS}
        shrm {
            description = "SHRM";
            data = /incbin/("${UBOOT_FIT_SHRM_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_SHRM_LOADADDRESS}>;
            entry = <${UBOOT_FIT_SHRM_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for QcLib (shikra)
uboot_fitimage_qclib() {
	cat << EOF >> ${UBOOT_ITS}
        qclib {
            description = "QcLib";
            data = /incbin/("${UBOOT_FIT_QCLIB_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_QCLIB_LOADADDRESS}>;
            entry = <${UBOOT_FIT_QCLIB_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the ECC config (shikra)
uboot_fitimage_ecc() {
	cat << EOF >> ${UBOOT_ITS}
        ecc {
            description = "ECC Config";
            data = /incbin/("${UBOOT_FIT_ECC_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_ECC_LOADADDRESS}>;
            entry = <${UBOOT_FIT_ECC_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the TZ Access Control Config blob (nord)
uboot_fitimage_tz_ac_config() {
	cat << EOF >> ${UBOOT_ITS}
        tz_ac_config {
            description = "TZ Access Control Config";
            data = /incbin/("${UBOOT_FIT_TZ_AC_CONFIG_IMAGE}");
            type = "tee";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_TZ_AC_CONFIG_LOADADDRESS}>;
            entry = <${UBOOT_FIT_TZ_AC_CONFIG_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the PMIC Cocos config (shikra)
uboot_fitimage_pmic_cocos() {
	cat << EOF >> ${UBOOT_ITS}
        pmic_cocos {
            description = "PMIC Cocos Config";
            data = /incbin/("${UBOOT_FIT_PMIC_COCOS_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_PMIC_COCOS_LOADADDRESS}>;
            entry = <${UBOOT_FIT_PMIC_COCOS_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the PMIC Wailua config (shikra)
uboot_fitimage_pmic_wailua() {
	cat << EOF >> ${UBOOT_ITS}
        pmic_wailua {
            description = "PMIC Wailua Config";
            data = /incbin/("${UBOOT_FIT_PMIC_WAILUA_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_PMIC_WAILUA_LOADADDRESS}>;
            entry = <${UBOOT_FIT_PMIC_WAILUA_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the DDR Config Block, LP4 variant (shikra)
uboot_fitimage_dcb_lp4() {
	cat << EOF >> ${UBOOT_ITS}
        dcb_lp4 {
            description = "DDR Config Block (LP4)";
            data = /incbin/("${UBOOT_FIT_DCB_LP4_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_DCB_LP4_LOADADDRESS}>;
            entry = <${UBOOT_FIT_DCB_LP4_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the DDR Config Block, LP5 variant (shikra)
uboot_fitimage_dcb_lp5() {
	cat << EOF >> ${UBOOT_ITS}
        dcb_lp5 {
            description = "DDR Config Block (LP5)";
            data = /incbin/("${UBOOT_FIT_DCB_LP5_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_DCB_LP5_LOADADDRESS}>;
            entry = <${UBOOT_FIT_DCB_LP5_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for SHRM (shikra)
uboot_fitimage_shrm() {
	cat << EOF >> ${UBOOT_ITS}
        shrm {
            description = "SHRM";
            data = /incbin/("${UBOOT_FIT_SHRM_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_SHRM_LOADADDRESS}>;
            entry = <${UBOOT_FIT_SHRM_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for QcLib (shikra)
uboot_fitimage_qclib() {
	cat << EOF >> ${UBOOT_ITS}
        qclib {
            description = "QcLib";
            data = /incbin/("${UBOOT_FIT_QCLIB_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_QCLIB_LOADADDRESS}>;
            entry = <${UBOOT_FIT_QCLIB_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}

# Create a ITS file for the ECC config (shikra)
uboot_fitimage_ecc() {
	cat << EOF >> ${UBOOT_ITS}
        ecc {
            description = "ECC Config";
            data = /incbin/("${UBOOT_FIT_ECC_IMAGE}");
            type = "firmware";
            arch = "${UBOOT_ARCH}";
            compression = "none";
            load = <${UBOOT_FIT_ECC_LOADADDRESS}>;
            entry = <${UBOOT_FIT_ECC_ENTRYPOINT}>;
EOF
	if [ "${SPL_SIGN_ENABLE}" = "1" ] ; then
		cat << EOF >> ${UBOOT_ITS}
            signature {
                algo = "${UBOOT_FIT_HASH_ALG},${UBOOT_FIT_SIGN_ALG}";
                key-name-hint = "${SPL_SIGN_KEYNAME}";
            };
EOF
	fi

	cat << EOF >> ${UBOOT_ITS}
        };
EOF
}
