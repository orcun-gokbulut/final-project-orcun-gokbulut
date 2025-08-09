SUMMARY = "Utility application for controlling 1602a kernel module."
DESCRIPTION = "Utility application for controlling 1602a kernel module."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

LOCAL_SRC_DIR = "${TOPDIR}/../../sources/utility"
require prepare-local-sources.inc

inherit pkgconfig cmake
