FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
# To make the layer pass yocto-check-layer only inherit update-alternatives when building for qualcomm
ALTERNATIVES_CLASS = ""
ALTERNATIVES_CLASS:qcom = "update-alternatives"

SRC_URI:append:nord = " \
    https://artifactory-las.qualcomm.com/artifactory/lint-lv-local/nord-test/NORD_fw.zip;name=nordfw \
"
SRC_URI[nordfw.sha256sum] = "a4abe380596d61181288c519e0ae298c592503cf0e12cf63401ed822da547b3d"

PACKAGES:prepend:nord = "${PN}-qcom-nord "

LICENSE:${PN}-qcom-nord = "Firmware-qcom & Firmware-qcom-2"
RDEPENDS:${PN}-qcom-nord = "${PN}-qcom-license ${PN}-qcom-2-license"
FILES:${PN}-qcom-nord = " \
    ${firmwaredir}/qca/gn* \
    ${firmwaredir}/qca/hmt* \
    ${firmwaredir}/qcom/nord \
    ${firmwaredir}/qcom/sa8797p \
    ${datadir}/qcom/nord \
    ${datadir}/qcom/sa8797p \
"
INSANE_SKIP:${PN}-qcom-nord += "already-stripped"
SKIP_FILEDEPS:${PN}-qcom-nord = "1"

inherit_defer ${ALTERNATIVES_CLASS}

# firmware-ath6kl provides updated bdata.bin, which can not be accepted into main linux-firmware repo
ALTERNATIVE:${PN}-ath6k:qcom = "ar6004-hw13-bdata"
ALTERNATIVE_LINK_NAME[ar6004-hw13-bdata] = "${nonarch_base_libdir}/firmware/ath6k/AR6004/hw1.3/bdata.bin${@fw_compr_file_suffix(d)}"

do_install:append:nord() {
    install -d ${D}${firmwaredir}
    cp -R --no-dereference --preserve=mode,links ${UNPACKDIR}/NORD_fw/lib/firmware/. ${D}${firmwaredir}/
    install -d ${D}${datadir}/qcom
    cp -R --no-dereference --preserve=mode,links ${UNPACKDIR}/NORD_fw/usr/share/qcom/. ${D}${datadir}/qcom/

    for fw in $(find ${D}${firmwaredir}/qca ${D}${firmwaredir}/qcom/sa8797p -type f)
    do
        if [ "${fw%%jsn}" = "${fw}" ]
        then
            case "${FIRMWARE_COMPRESSION}" in
                zst | zstd)
                    zstd --compress --rm ${fw}
                    ;;
                xz)
                    xz --compress --check=crc32 ${fw}
                    ;;
            esac
        fi
    done
}

INSANE_SKIP:${PN}-qcom-nord += "already-stripped libdir file-rdeps textrel"
