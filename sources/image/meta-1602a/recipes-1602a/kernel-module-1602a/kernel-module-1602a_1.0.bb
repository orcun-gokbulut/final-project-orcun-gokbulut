SUMMARY = "Linux kernel driver module for 1602a LCD Display"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS_prepend := "${TOPDIR}/../../sources:"
SRC_URI = "file://module/"

inherit module

