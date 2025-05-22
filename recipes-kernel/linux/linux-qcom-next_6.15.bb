SECTION = "kernel"

DESCRIPTION = "Linux ${PV} kernel for QCOM devices"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel

COMPATIBLE_MACHINE = "(qcom)"

# v6.15-rc7
SRCTAG ?= "qcom-next-6.15-rc7-20250530"
SRCREV = "d76c9c2e094bee4b40ed9c85b0d7fd8b025bff61"
PV = "6.15+git"

SRC_URI = "git://github.com/qualcomm-linux/kernel.git;protocol=https;branch=qcom-next;tag=${SRCTAG}"
SRC_URI += "file://0001-drivers-gpu-drm-msm-registers-improve-reproducibilit.patch"

# To build bleeding edge qcom-next kernel set preferred 
# provider of virtual/kernel to 'linux-qcom-next-tip'
BBCLASSEXTEND = "devupstream:target"
PN:class-devupstream = "linux-qcom-next-tip"
SRCREV:class-devupstream = "${AUTOREV}"

S = "${WORKDIR}/git"

KERNEL_CONFIG ?= "defconfig"

do_configure:prepend() {
    if [ ! -f "${S}/arch/${ARCH}/configs/${KERNEL_CONFIG}" ]; then
        bbfatal "KERNEL_CONFIG '${KERNEL_CONFIG}' was specified, but not present in the source tree"
    else
        cp '${S}/arch/${ARCH}/configs/${KERNEL_CONFIG}' '${B}/.config'
    fi
}
