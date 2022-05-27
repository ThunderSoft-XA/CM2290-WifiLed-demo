inherit cmake

DESCRIPTION = "the demo for warnning driver`s dangerous action"
LICENSE = "BSD"
SECTION = "WifiLed"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE};md5=3775480a712fc46a69647678acb234cb"

# Dependencies.
DEPENDS := "mosquitto"
DEPENDS += "libnl zlib"
DEPENDS += "glib-2.0"
DEPENDS += "libutils"
DEPENDS += "libcutils"

FILESPATH =+ "${WORKSPACE}/example/wifi_led/mqtt/bin/:"

PN = "wifi_led"
SRC_URI = "file://wifi_led/mqtt/"
INSANE_SKIP_${PN}-dev += "ldflags dev-elf dev-deps"
PACKAGES = "${PN}-dbg ${PN} ${PN}-dev"
S = "${WORKSPACE}/example/wifi_led/mqtt"

# Install directries.
INSTALL_INCDIR := "${includedir}"
INSTALL_BINDIR := "${bindir}"
INSTALL_LIBDIR := "${libdir}"

EXTRA_OECMAKE += ""

FILES_${PN} += "${INSTALL_BINDIR}"
FILES_${PN} += "${INSTALL_LIBDIR}"

SOLIBS = ".so*"
FILES_SOLIBSDEV = ""
