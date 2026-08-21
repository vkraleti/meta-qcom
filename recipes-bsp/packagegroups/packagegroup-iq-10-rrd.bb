SUMMARY = "Packages for the IQ-10-RRD platform"

inherit packagegroup

PACKAGES = " \
    ${PN}-firmware \
    ${PN}-hexagon-dsp-binaries \
"

RRECOMMENDS:${PN}-firmware = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'linux-firmware-ath12k-wcn7850', '', d)} \
    linux-firmware-qcom-nord \
"

RDEPENDS:${PN}-hexagon-dsp-binaries = " \
"
