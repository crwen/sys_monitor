
CC = /usr/bin/gcc
# CFLAGS = -Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function
CFLAGS = -Wall -g -O2 -std=gnu99 -Wno-unused-function

TARGET_HARDWARE = ./bin/test_hardware
TARGET_CACHE = ./bin/test_cache

SRC_DIR = ./src

# debug
COMMON = $(SRC_DIR)/common/print.c $(SRC_DIR)/common/convert.c

# hardware
CPU =$(SRC_DIR)/hardware/cpu/mmu.c $(SRC_DIR)/hardware/cpu/isa.c
MEMORY = $(SRC_DIR)/hardware/memory/dram.c
CACHE = $(SRC_DIR)/hardware/cpu/sram.c

# main
TEST_HARDWARE = $(SRC_DIR)/tests/test_hardware.c
TEST_CACHE = $(SRC_DIR)/tests/test_cache.c

.PHONY:hardware
hardware:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(COMMON) $(CPU) $(MEMORY) $(CACHE) $(TEST_HARDWARE) -o $(TARGET_HARDWARE)
	./$(TARGET_HARDWARE)

clean:
	rm -f *.o *~ $(TARGET_HARDWARE)


.PHONY:cache
cache:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(COMMON) $(CPU) $(MEMORY) $(CACHE) $(TEST_CACHE) -o $(TARGET_CACHE)
	./$(TARGET_CACHE)
clean:
	rm -f *.o *~ $(TARGET_CACHE)
