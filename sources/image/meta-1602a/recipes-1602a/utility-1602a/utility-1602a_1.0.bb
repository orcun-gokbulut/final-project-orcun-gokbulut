SUMMARY = "Utility application for controlling 1602a kernel module."
DESCRIPTION = "Utility application for controlling 1602a kernel module."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS_prepend := "${TOPDIR}/../../sources:"
SRC_URI = "file://utility/"

inherit pkgconfig cmake
