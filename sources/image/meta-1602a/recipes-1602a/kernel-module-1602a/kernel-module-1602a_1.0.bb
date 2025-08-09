SUMMARY = "Linux kernel driver module for 1602a LCD Display"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

LOCAL_SRC_DIR = "${TOPDIR}/../../sources/module"
require prepare-local-sources.inc

inherit module

