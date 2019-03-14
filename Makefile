# This project uses the build system included with the LUFA library

MCU = atmega32u4
ARCH = AVR8
BOARD = TEENSY2
F_CPU = 16000000
F_USB = $(F_CPU)
OPTIMIZATION = s
TARGET = keyboard
SRC = src/keyboard.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS) $(LUFA_SRC_PLATFORM) src/descriptors.c src/key_scan.c
LUFA_PATH = extern/lufa/LUFA
CC_FLAGS = -Wextra -DUSE_LUFA_CONFIG_HEADER -Isrc/config -std=gnu11
LD_FLAGS =

all:
	

DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

DMBS_PATH ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk


