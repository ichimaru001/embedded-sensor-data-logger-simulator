# Makefile for compiling logging-sensor-readings.c

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -std=c11

# Output executable name
TARGET = sensor_logger.exe

# Source file
SRC = logging-sensor-readings.c

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean rule to remove executable
clean:
	del /Q $(TARGET)

# .PHONY to prevent conflicts with files named 'all' or 'clean'
.PHONY: all clean