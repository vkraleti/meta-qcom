require recipes-security/optee/optee-os.inc

PV = "4.11.0-qcom-20261106+git"

SRC_TAG = "tag=qcom-next-4.11-20261106"
SRC_URI = "git://github.com/qualcomm-linux/optee_os.git;protocol=https;name=optee;nobranch=1;${SRC_TAG}"
SRCREV_optee = "91784800638ecf20cd6de79eb5374756321ea488"

# Nord Bringup specific
SRC_URI:nord = " \
    git://github.com/qualcomm-linux/optee_os.git;protocol=https;name=optee-nord;branch=early/hwe/nord \
    file://0001-pta-qcom-pas-nord-update-IRIS-video-codec-bring-up.patch \
    file://0002-pta-qcom-pas-nord-fix-IRIS-fw_end-address-programmin.patch \
"
SRCREV_optee-nord = "bfc3b226d1eb9c90af51f416b1b9beba7781e3d4"

# Enable PKCS#11 TA as part of the OP-TEE build
EXTRA_OEMAKE:append = " CFG_PKCS11_TA=y"

# Copy of do_deploy from meta-arm until we get it fixed to support configurable deploy dir
OPTEE_DEPLOY ?= "optee"
do_deploy() {
    install -d ${DEPLOYDIR}/${MLPREFIX}${OPTEE_DEPLOY}
    install -m 644 ${D}${nonarch_base_libdir}/firmware/* ${DEPLOYDIR}/${MLPREFIX}${OPTEE_DEPLOY}

    install -d ${DEPLOYDIR}/${MLPREFIX}${OPTEE_DEPLOY}/ta
    install -m 644 ${B}/ta/*/*.elf ${DEPLOYDIR}/${MLPREFIX}${OPTEE_DEPLOY}/ta
}
