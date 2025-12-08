# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iheaders

# Folders
SRC_DIR = source
HDR_DIR = headers
BUILD_DIR = build
TEST_DIR = test

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)

# Object files: build/file.o for each source/file.c
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRC))

# Output executables
TARGET = $(BUILD_DIR)/main
TEST_TARGET = $(BUILD_DIR)/test_hospital

# Default rule
all: $(TARGET)

# Rule to build the final program
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Rule to build test program
test: $(TEST_TARGET)

$(TEST_TARGET): $(filter-out $(BUILD_DIR)/main.o, $(OBJ)) $(TEST_OBJ)
	$(CC) $(filter-out $(BUILD_DIR)/main.o, $(OBJ)) $(TEST_OBJ) -o $(TEST_TARGET)

# Rule to build .o files inside build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all test clean
