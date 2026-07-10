# Temporary override for Nord SoC bring-up.
# Nord kernel support is available in the 'qcom-next' branch but some
# Critical fixes are mssing. Bring them via a bbappend for now.

SRC_URI:append:nord = " \
    file://0001-FROMLIST-dt-bindings-phy-qcom-sc8280xp-qmp-usb43dp-p.patch \
    file://0002-FROMLIST-dt-bindings-phy-qcom-snps-eusb2-phy-Add-Nor.patch \
    file://0003-FROMLIST-phy-phy-snps-eusb2-Enable-AHB2PHY-bus-clock.patch \
    file://0004-FIXUP-arm64-dts-qcom-nord-embedded-Add-AHB2PHY-clock.patch \
    file://0005-PENDING-arm64-dts-qcom-nord-ride-embedded-disable-md.patch \
    file://0006-FIXUP-ASoC-qcom-sc8280xp-fix-audio-capture-issue.patch \
    file://0007-FIXUP-arm64-dts-qcom-nord-ride-embedded-Fix-Audio-ca.patch \
    file://0008-FROMGIT-ASoC-qcom-Add-QAIF-AIF-MI2S-and-TDM-DAI-supp.patch \
    file://0009-FIXUP-arm64-dts-qcom-nord-Add-missing-vmids-for-adsp.patch \
    file://0010-PENDING-arm64-dts-qcom-nord-Fix-iris-AXI-clock-assig.patch \
    file://0011-FIXUP-arm64-dts-qcom-nord-ride-embedded-Add-WCN-DIG-.patch \
    file://0012-FROMLIST-arm64-dts-qcom-nord-ride-embedded-Add-WCN7.patch \
"
