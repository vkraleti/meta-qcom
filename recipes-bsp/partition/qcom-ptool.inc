HOMEPAGE = "https://github.com/qualcomm-linux/qcom-ptool"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b0a8acd90d872086b279ead88af03369"

FILESEXTRAPATHS:prepend := "${THISDIR}/qcom-ptool:"

SRC_URI = "git://github.com/qualcomm-linux/qcom-ptool.git;branch=main;protocol=https"
SRCREV = "8e85e196d2478e67b32558dc833be3beceeea512"

SRC_URI:append = " file://0001-platforms-add-partition-scheme-for-iq10-rrd-board.patch"
