SUMMARY = "Packages for the IQ-10-RRD platform"

inherit packagegroup

PACKAGES = " \
    ${PN}-firmware \
    ${PN}-hexagon-dsp-binaries \
"

RRECOMMENDS:${PN}-firmware = " \
"

RDEPENDS:${PN}-hexagon-dsp-binaries = " \
"
