# Copyright (c) 2026 Qualcomm Innovation Center, Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause-Clear
#
# FIT assembly for the SPL FIT boot flow. These functions replace the
# uboot-sign.bbclass ones of the same name, so this file has to be required
# after recipes-bsp/u-boot/u-boot.inc, which is what inherits that class.

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

    if [ -n "${UBOOT_FIT_USER_SETTINGS}" ] ; then
        printf "%b" "${UBOOT_FIT_USER_SETTINGS}" >> ${UBOOT_ITS}
    fi

    if [ -n "${UBOOT_FIT_CONF_USER_LOADABLES}" ] ; then
        conf_loadables="${conf_loadables}${UBOOT_FIT_CONF_USER_LOADABLES}"
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
