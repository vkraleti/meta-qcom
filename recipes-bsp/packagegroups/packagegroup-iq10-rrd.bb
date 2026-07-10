SUMMARY = "Packages for the IQ10-RRD platform"

inherit packagegroup

PACKAGES = " \
    ${PN}-firmware \
    ${PN}-hexagon-dsp-binaries \
"

# TODO: replace with the real linux-firmware-qcom-* packages for nord's
# GPU/WLAN/BT/audio/video once available.
RRECOMMENDS:${PN}-firmware = " \
    ${@bb.utils.contains_any('DISTRO_FEATURES', 'opencl opengl vulkan', 'linux-firmware-qcom-adreno-nord', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'bluetooth', 'linux-firmware-qca-wcn7850', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'linux-firmware-ath12k-wcn7850', '', d)} \
    linux-firmware-qcom-nord-audio \
    linux-firmware-qcom-nord-compute \
    linux-firmware-qcom-vpu \
"

# TODO: confirm hexagon DSP binaries recipes exist under this name for iq10-rrd
RDEPENDS:${PN}-hexagon-dsp-binaries = " \
    hexagon-dsp-binaries-qcom-iq10-rrd-adsp \
    hexagon-dsp-binaries-qcom-iq10-rrd-cdsp \
"
