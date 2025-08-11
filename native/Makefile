# Name of the executable
TARGET = test/build/test_runner

# Compiler and common flags
CC = gcc
CFLAGS = -Itest -Isrc -Wall -Wextra -pedantic

# Specific flags for src/miniaudio/miniaudio.c
MINIAUDIO_CFLAGS = -Itest -Isrc -Wextra

# Source files
SRCS = test/test_runner.c \
       test/unity/unity.c \
       src/audio_context.c \
	   src/waveform.c \
       src/logger.c \
       src/miniaudio.c \
	   src/audio_device.c \
	   src/playback_device.c \
	   src/audio_context_private.c \
	   src/internal.c \
	   src/encoder.c

# Build directory
BUILD_DIR = test/build

# Object files with paths in the build directory
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# Rule for linking the executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

# Rule for compiling source files into the build directory
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@) # Ensure the build directory exists
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for src/miniaudio/miniaudio.c
$(BUILD_DIR)/src/miniaudio.o: src/miniaudio.c
	@mkdir -p $(dir $@)
	$(CC) $(MINIAUDIO_CFLAGS) -c $< -o $@

# Run the tests
test: $(TARGET)
	./$(TARGET)

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: test clean
