# Proyecto secscanner (Port Scanner + Banner Grab)
# Compilar: make
# Ejecutar ejemplo: make run
# Depurar (VS Code): Ejecutar tarea build y usar la configuración LLDB incluida.

CC := clang
CFLAGS := -std=c11 -Wall -Wextra -Werror -O0 -g -Iinclude
SRC_DIR := src
BUILD_DIR := build
TARGET := secscanner

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: all
	./$(TARGET) -t scanme.nmap.org -p 22,80 -T 300 -v

test: all
	chmod +x tests/test_basic.sh
	tests/test_basic.sh

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
