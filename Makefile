DEFAULT_GOAL := all
.DEFAULT: all
SHELL := /bin/bash
ROOT_DIR := "$(abspath $(dir $(lastword $(MAKEFILE_LIST))))"
BUILD_DIR := "$(ROOT_DIR)/build"
SOURCES_DIR := "$(ROOT_DIR)/sources"
EXTERNALS_DIR := "$(ROOT_DIR)/externals"


# DEFAULT
###############################################################################

.PHONY: all
all: image


.PHONY: clean
clean: image-clean\
	@rm -rfv "$(BUILD_DIR)"


# KERNEL MODULE
###############################################################################

.PHONY: module
module:
	@make -C "$(SOURCES_DIR)/module"


.PHONY: module-clean
module-clean:
	@make -C "$(SOURCES_DIR)/module" clean


# UTILITY APPLICATION
###############################################################################

.PHONY: utility
utility:
	@mkdir -p "$(BUILD_DIR)/utility"
	@cmake -S "$(SOURCES_DIR)/utility" -B "$(BUILD_DIR)/utility"
	@cmake --build "$(BUILD_DIR)/utility"

.PHONY: utility-clean
utility-clean:
	@cmake --build "$(BUILD_DIR)/utility" -t clean


# FIRMWARE IMAGE
###############################################################################

.PHONY: image
image:
	@mkdir -p "$(BUILD_DIR)/image"
	@cp -r $(SOURCES_DIR)/image/conf $(BUILD_DIR)/image
	@( \
		source "$(EXTERNALS_DIR)/poky/oe-init-build-env" "$(BUILD_DIR)/image" ; \
		bitbake core-image-1602a \
	)


.PHONY: image-clean
image-clean:
	@rm -rfv "$(BUILD_DIR)/image"
