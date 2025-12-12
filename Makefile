# Smart Hospital Simulator Makefile

CC = gcc
CFLAGS = -Wall -Wextra -I./include -pthread
LDFLAGS = -pthread -lrt

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/hospital_simulator

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	@$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build successful! Executable: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@rm -f hospital_simulation.log
	@echo "Cleanup complete!"

# Clean IPC resources (message queues, shared memory, semaphores)
clean-ipc:
	@echo "Cleaning IPC resources..."
	@ipcrm -a 2>/dev/null || true
	@rm -f /dev/shm/sem.sem_* 2>/dev/null || true
	@echo "IPC cleanup complete!"

# Run the simulator
run: $(TARGET)
	@echo "Running Smart Hospital Simulator..."
	@./$(TARGET)

# Full clean (build artifacts + IPC resources)
distclean: clean clean-ipc
	@echo "Full cleanup complete!"

# Display help
help:
	@echo "Smart Hospital Simulator - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all         - Build the simulator (default)"
	@echo "  clean       - Remove build artifacts"
	@echo "  clean-ipc   - Remove IPC resources (message queues, shared memory, semaphores)"
	@echo "  distclean   - Remove everything (build + IPC)"
	@echo "  run         - Build and run the simulator"
	@echo "  help        - Display this help message"

.PHONY: all clean clean-ipc distclean run help directories