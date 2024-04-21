TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8258

LIBS :=  -llt_8258

TEL_PATH := ../..

PROJECT_NAME := Mo

DOWNLOAD_PORT := /dev/ttyUSB0

PROJECT_PATH := .
OUT_PATH :=$(PROJECT_PATH)/out

ifneq ($(TEL_PATH)/make/makefile, $(wildcard $(TEL_PATH)/make/makefile))
$(error "Please check if the TEL_PATH is correct, 请检查 TEL_PATH 是否设置正确！！！")
endif

-include $(TEL_PATH)/make/makefile