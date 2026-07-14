SUMMARY = "Packages for the IQ10-RRD platform"

inherit packagegroup

PACKAGES = " \
    ${PN}-firmware \
    ${PN}-hexagon-dsp-binaries \
"

RRECOMMENDS:${PN}-firmware = " \
    linux-firmware-qcom-nord \
"

# TODO: confirm hexagon DSP binaries recipes exist under this name for iq10-rrd
# RDEPENDS:${PN}-hexagon-dsp-binaries = " \
#    hexagon-dsp-binaries-qcom-iq10-rrd-adsp \
#    hexagon-dsp-binaries-qcom-iq10-rrd-cdsp \
# "
