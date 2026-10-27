require recipes-bsp/u-boot/u-boot-common.inc
require recipes-bsp/u-boot/u-boot.inc
require recipes-bsp/u-boot/u-boot-qcom-spl-fit.inc

DEPENDS += "bc-native dtc-native gnutls-native python3-pyelftools-native qtestsign-native xxd-native"

QCOM_UBOOT_SPL_FIT ?= "0"
QCOM_UBOOT_SPL_FIT_ATF ?= ""
QCOM_UBOOT_SPL_FIT_TEE ?= ""
QCOM_UBOOT_SPL_FIT_TZ_AC_CONFIG ?= ""
QCOM_UBOOT_SPL_FIT_EXTRA_LOADABLES ?= ""
QCOM_UBOOT_SPL_FIT_XBL_SEC ?= ""

# BL31 and OP-TEE go into the FIT, swiv annotates the SPL before signing.
DEPENDS += "${@bb.utils.contains('QCOM_UBOOT_SPL_FIT', '1', '${QCOM_UBOOT_SPL_FIT_ATF} ${QCOM_UBOOT_SPL_FIT_TEE} swiv-build-utility-native', '', d)}"

# tz_ac_config.bin, when set, is deployed rather than sysroot-staged, so pull
# it in via a task dependency instead of DEPENDS.
do_compile[depends] += "${@'%s:do_deploy' % d.getVar('QCOM_UBOOT_SPL_FIT_TZ_AC_CONFIG') if d.getVar('QCOM_UBOOT_SPL_FIT') == '1' and d.getVar('QCOM_UBOOT_SPL_FIT_TZ_AC_CONFIG') else ''}"

# QCOM_UBOOT_SPL_FIT_EXTRA_LOADABLES is a space-separated list of
# "recipe:filename" entries for further blobs (PMIC/DCB configs, SHRM,
# QcLib, ...) that ride along in the FIT as loadables. Like qc_sec.mbn
# above, each file is deploy-only, so pull in its providing recipe via a
# task dependency instead of DEPENDS.
do_compile[depends] += "${@' '.join(sorted(set('%s:do_deploy' % entry.split(':', 1)[0] for entry in d.getVar('QCOM_UBOOT_SPL_FIT_EXTRA_LOADABLES').split()))) if d.getVar('QCOM_UBOOT_SPL_FIT') == '1' else ''}"

# qc_sec.mbn is a prebuilt the SPL links in as the XBL-sec ELF segment; it is
# deploy-only (no sysroot staging), so fetch it from the firmware recipe's
# deploy directory instead of DEPENDS.
do_compile[depends] += "${@'%s:do_deploy' % d.getVar('QCOM_UBOOT_SPL_FIT_XBL_SEC') if d.getVar('QCOM_UBOOT_SPL_FIT_XBL_SEC') else ''}"

COMPATIBLE_MACHINE:aarch64 = "(qcom)"

PV = "2026.07+2026.10-rc1+git"

SRCREV = "e4d42e0cb2e595b66aa00e55dee49c1ad1701ad3"
SRCBRANCH = "branch=nord-staging-qcom-next"

SRC_URI = "git://github.com/qualcomm-linux/u-boot.git;${SRCBRANCH};protocol=https;name=uboot"
SRC_URI += " \
    file://0001-Add-support-for-OpenSSL-Provider-API.patch \
    file://disable-eficapsule-tool.cfg \
    file://efi-rt-volatile-store.cfg \
    ${@bb.utils.contains('MACHINE_FEATURES', 'optee', 'file://tfa-optee.cfg', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'kvm', 'file://gunyah-exit.cfg', '', d)} \
    ${@bb.utils.contains('SPL_SIGN_ENABLE', '1', 'file://spl-fit-signature.cfg', '', d)} \
"

python __anonymous() {
    ubootconfig = (d.getVar('UBOOT_CONFIG') or "").split()

    if len(ubootconfig) > 0:
        for config in ubootconfig:
            # Get the MBN header version for this specific config
            mbn_header = d.getVarFlag('BOARD_MBN_HEADER', config)

            if not mbn_header:
                mbn_header = ""

            d.appendVar('BOARD_MBN_HEADER', mbn_header + " ? ")
}

# xbl_sec.elf is linked from qc_sec.mbn at the top of the source tree, so it
# has to be in place before uboot_compile_config's first oe_runmake call.
do_compile:prepend() {
    if [ -n "${QCOM_UBOOT_SPL_FIT_XBL_SEC}" ]; then
        install -m 0644 ${DEPLOY_DIR_IMAGE}/${QCOM_BOOT_FILES_SUBDIR}/qc_sec.mbn ${S}/qc_sec.mbn
    fi
}

uboot_compile_config:append() {
    config_mbn_header=$(uboot_config_get_indexed_value "${BOARD_MBN_HEADER}" $i)

    if [ "${QCOM_UBOOT_SPL_FIT}" = "1" ]; then
        # Where uboot-sign's /incbin/ defaults expect them.
        install -m 0644 ${RECIPE_SYSROOT}/firmware/${QCOM_UBOOT_SPL_FIT_ATF}/bl31.bin ${B}/${builddir}/bl31.bin
        install -m 0644 ${RECIPE_SYSROOT}${nonarch_base_libdir}/firmware/tee-raw.bin ${B}/${builddir}/tee-raw.bin
        for fit_extra_loadable in ${QCOM_UBOOT_SPL_FIT_EXTRA_LOADABLES}; do
            fit_extra_loadable_file="${fit_extra_loadable#*:}"
            install -m 0644 ${DEPLOY_DIR_IMAGE}/${QCOM_BOOT_FILES_SUBDIR}/${fit_extra_loadable_file} ${B}/${builddir}/${fit_extra_loadable_file}
        done
        if [ -n "${QCOM_UBOOT_SPL_FIT_TZ_AC_CONFIG}" ]; then
            install -m 0644 ${DEPLOY_DIR_IMAGE}/${QCOM_BOOT_FILES_SUBDIR}/tz_ac_config.bin ${B}/${builddir}/tz_ac_config.bin
        fi
    elif [ -n "${config_mbn_header}" ]; then
        export CRYPTOGRAPHY_OPENSSL_NO_LEGACY=1
        qtestsign -${config_mbn_header} aboot -o ${B}/${builddir}/u-boot.mbn ${B}/${builddir}/u-boot.elf
    fi
}

# Rebuild the SPL ELF after uboot-sign, add the SWIV segment and sign it as TZ.
uboot_assemble_fitimage_helper:append() {
    if [ "${QCOM_UBOOT_SPL_FIT}" = "1" ]; then
        mbn_header=$(uboot_config_get_indexed_value "${BOARD_MBN_HEADER}" $i)
        [ -n "${mbn_header}" ] || mbn_header="v6"

        rm -f spl/u-boot-spl.elf
        (unset LDFLAGS CFLAGS; oe_runmake -C ${S} O=${B}/${builddir} ${UBOOT_MAKE_OPTS} spl/u-boot-spl.elf)

        export CRYPTOGRAPHY_OPENSSL_NO_LEGACY=1
        if [ -n "${QCOM_FW_SWIV_PLATFORM}" ]; then
            swiv_build_utility u-boot-spl-swiv.elf spl/u-boot-spl.elf ${QCOM_FW_SWIV_PLATFORM}
            qtestsign -${mbn_header} tz -o u-boot-spl.mbn u-boot-spl-swiv.elf
            rm -f u-boot-spl-swiv.elf
        else
            qtestsign -${mbn_header} ${QCOM_SPL_FW_TYPE} -o u-boot-spl.mbn spl/u-boot-spl.wrap-elf
        fi
    fi
}

uboot_deploy_config:append() {
    if [ "${QCOM_UBOOT_SPL_FIT}" = "1" ]; then
        if [ -f ${B}/${builddir}/u-boot-spl.mbn ]; then
            install -m 0644 ${B}/${builddir}/u-boot-spl.mbn ${DEPLOYDIR}/u-boot-spl-${type}.mbn
        fi
    elif [ -f ${B}/${builddir}/u-boot.mbn ]; then
        install -m 0644 ${B}/${builddir}/u-boot.mbn ${DEPLOYDIR}/u-boot-${type}.mbn
    fi
}
