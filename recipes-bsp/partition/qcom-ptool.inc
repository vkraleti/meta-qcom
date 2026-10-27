HOMEPAGE = "https://github.com/qualcomm-linux/qcom-ptool"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b0a8acd90d872086b279ead88af03369"

SRC_URI = "git://github.com/qualcomm-linux/qcom-ptool.git;branch=main;protocol=https"
SRCREV = "5da6896fdfa1a7cb142e53145c543021d69fd9c9"

SRC_URI += "file://0001-nord-rrd-open-boot-partitions.patch"
SRC_URI += "file://0001-platforms-add-partitioning-scheme-for-nord-rrd-open-.patch"
