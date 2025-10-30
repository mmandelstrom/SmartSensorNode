# Variables
CC=gcc
OPTIMIZE=-ffunction-sections -fdata-sections -O2 -flto -Wno-unused-result -fno-strict-aliasing
DEBUG_FLAGS=-g -O0 -Wfatal-errors -Werror
LIBS=-luuid -lcurl -pthread -lm -lbsd
INCLUDES = 

#   -DWALLOCATOR_DEBUG -DWALLOCATOR_DEBUG_BORDERCHECK
# -fsanitize=address -fno-omit-frame-pointer

# Build mode: release (default) or debug
MODE ?= release

# Base warnings/defs
CFLAGS_BASE=-Wall -Wno-psabi -Wfatal-errors -Werror -Ilibs

# Select flags per mode (OPTIMIZE goes into CFLAGS in release; LTO linked only in release)
ifeq ($(MODE),debug)
  CFLAGS=$(CFLAGS_BASE) $(DEBUG_FLAGS)
  LDFLAGS=
else
  CFLAGS=$(CFLAGS_BASE) #$(OPTIMIZE)
  LDFLAGS=
endif

# Directories
SRC_DIR=.
BUILD_DIR=build

# Find all .c files (following symlinks)
SOURCES=$(shell find -L $(SRC_DIR) -type f -name '*.c')

# Per-target object lists in separate dirs (server removed)
CLIENT_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/client/%.o,$(SOURCES))

# Executables (server removed)
EXECUTABLES=client

# Default target
all: $(EXECUTABLES)
	@echo "Build complete ($(MODE))."

# Debug helpers (server removed)
debug-client:
	@$(MAKE) MODE=debug --no-print-directory clean client
	@-rm -f WADEBUG.txt
	gdb client -ex run

# Link rules (server removed)
client: $(CLIENT_OBJECTS)
	@echo "Linking $@..."
	@$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

# Compile rules with per-target defines (server pattern removed)
$(BUILD_DIR)/client/%.o: $(SRC_DIR)/%.c
	@echo "Compiling (client) $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -DTCPCLIENT -c $< -o $@

# Specific file compilation (build into client tree)
FILE=
compile:
	@echo "Compiling $(FILE) (client defs)..."
	@mkdir -p $(BUILD_DIR)/client/$(dir $(FILE))
	$(CC) $(CFLAGS) $(INCLUDES) -DTCPCLIENT -c $(FILE) -o $(BUILD_DIR)/client/$(FILE:.c=.o)

# Clean
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR) $(EXECUTABLES)

.PHONY: all clean compile debug-client

