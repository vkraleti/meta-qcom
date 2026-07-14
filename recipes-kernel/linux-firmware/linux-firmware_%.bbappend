FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
# To make the layer pass yocto-check-layer only inherit update-alternatives when building for qualcomm
ALTERNATIVES_CLASS = ""
ALTERNATIVES_CLASS:qcom = "update-alternatives"

PATCHTOOL:qcom = "git"

SRC_URI:append:qcom = " \
    file://0001-qcom-sa8775p-update-signature-on-cdsp1-firmware.patch \
"

inherit_defer ${ALTERNATIVES_CLASS}

# firmware-ath6kl provides updated bdata.bin, which can not be accepted into main linux-firmware repo
ALTERNATIVE:${PN}-ath6k:qcom = "ar6004-hw13-bdata"
ALTERNATIVE_LINK_NAME[ar6004-hw13-bdata] = "${nonarch_base_libdir}/firmware/ath6k/AR6004/hw1.3/bdata.bin${@fw_compr_file_suffix(d)}"


# Temporary Nord Bringup changes
SRC_URI:append:nord = " \
    https://artifactory-las.qualcomm.com/artifactory/lint-lv-local/nord-test/NORD_fw.zip;name=nordfw \
"
SRC_URI[nordfw.sha256sum] = "a4abe380596d61181288c519e0ae298c592503cf0e12cf63401ed822da547b3d"

do_install:append:nord() {
    # firmware
    find ${UNPACKDIR}/NORD_fw/lib/firmware/ -type f | while read f ; do
        relpath="${f#${UNPACKDIR}/NORD_fw/lib/firmware/}"
        echo "installing $relpath into ${D}${libdir}/firmware/$relpath"
        install -Dm 0644 "$f" ${D}${libdir}/firmware/$relpath
    done
    # hex-dsp
    find ${UNPACKDIR}/NORD_fw/usr/share/ -type f | while read f ; do
        relpath="${f#${UNPACKDIR}/NORD_fw/usr/share/}"
        echo "installing $relpath into ${D}${datadir}/$relpath"
        install -Dm 0644 "$f" ${D}${datadir}/$relpath
    done
}

INSANE_SKIP:${PN}-qcom-nord += "already-stripped libdir file-rdeps textrel"
PACKAGES:prepend:nord = "${PN}-qcom-nord "

FILES:${PN}-qcom-nord = " \
    ${firmwaredir}/qca/gn* \
    ${firmwaredir}/qca/hmt* \
    ${firmwaredir}/qcom/nord/* \
    ${datadir}/qcom/sa8797p/* \
"
SKIP_FILEDEPS:${PN}-qcom-nord = "1"
