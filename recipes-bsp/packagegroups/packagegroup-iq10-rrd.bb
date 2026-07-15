SUMMARY = "Packages for the IQ10-RRD platform"

inherit packagegroup

PACKAGES = " \
    ${PN}-firmware \
    ${PN}-hexagon-dsp-binaries \
"

RRECOMMENDS:${PN}-firmware = " \
    linux-firmware-qcom-nord \
"

RDEPENDS:${PN}-hexagon-dsp-binaries = " \
    hexagon-dsp-binaries-qcom-iq10-rrd-cdsp \
    hexagon-dsp-binaries-qcom-iq10-rrd-cdsp1 \
    hexagon-dsp-binaries-qcom-iq10-rrd-cdsp2 \
    hexagon-dsp-binaries-qcom-iq10-rrd-cdsp3 \
"
