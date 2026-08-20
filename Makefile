XBE_TITLE = HBSX
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
NXDK_DIR ?= $(CURDIR)/../..
NXDK_SDL = y

all_local: cp_font all

include $(NXDK_DIR)/Makefile
include $(CURDIR)/libs/get/Makefile

cp_font:
	@mkdir -p $(OUTPUT_DIR)
	cp background.png $(OUTPUT_DIR)/
	cp background-480.png $(OUTPUT_DIR)/
	cp logo.png $(OUTPUT_DIR)/
	cp logo-480.png $(OUTPUT_DIR)/
	cp logo-720.png $(OUTPUT_DIR)/
	cp vegur-regular.ttf $(OUTPUT_DIR)/
	cp vegur-bold.ttf $(OUTPUT_DIR)/
