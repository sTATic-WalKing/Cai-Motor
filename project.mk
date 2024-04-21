OBJS += \
$(OUT_PATH)/app_att.o \
$(OUT_PATH)/app_ble.o \
$(OUT_PATH)/app_light.o \
$(OUT_PATH)/app_uart.o \
$(OUT_PATH)/main.o

$(OUT_PATH)/%.o: ./%.c
	@echo 'Building file: $<'
	@tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"