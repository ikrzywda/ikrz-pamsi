# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I packet-transfer/packets/include -I packet-transfer/packets -I packet-transfer/common/include -I packet-transfer/common -I packet-transfer/driver 
DEBUG_FLAGS = -g -fsanitize=address


# Source files
DRIVER_SRC = packet-transfer/driver/main.c \
			 packet-transfer/driver/driver.c
PACKET_SRC = packet-transfer/packets/src/message_transmitter.c \
			 packet-transfer/packets/src/message_receiver.c \
             packet-transfer/packets/src/packet.c \
             packet-transfer/packets/src/stack.c
COMMON_SRC = packet-transfer/common/src/env_logger.c \
			 packet-transfer/common/src/logger.c

# Object files
DRIVER_OBJ = build/main.o \
			 build/driver.o
PACKET_OBJ = build/message_transmitter.o \
			 build/message_receiver.o \
             build/packet.o \
             build/stack.o
COMMON_OBJ = build/env_logger.o \
			 build/logger.o

# Library
LIBRARY = build/libpackets.a


# Executable
EXECUTABLE = build/packet-transfer

# clang-format
CLANG_FORMAT = clang-format
FORMAT_FLAGS = -i -style=llvm

# Build target
all: $(EXECUTABLE)

# Create build directory
$(shell mkdir -p build)

# Linking the object files with the library to create the executable
$(EXECUTABLE): $(DRIVER_OBJ) $(COMMON_OBJ) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling the driver source file
build/%.o: packet-transfer/driver/%.c packet-transfer/packets/include/*  packet-transfer/common/include/*
	$(CC) $(CFLAGS) -c -o $@ $<

# Compiling the driver source files
build/%.o: packet-transfer/driver/%.c packet-transfer/driver/driver.h packet-transfer/packets/include/* packet-transfer/common/include/*
	$(CC) $(CFLAGS) -c -o $@ $<

# Compiling the packet source files
build/%.o: packet-transfer/packets/src/%.c packet-transfer/packets/include/* packet-transfer/common/include/*
	$(CC) $(CFLAGS) -c -o $@ $<

# Compiling the common source files
build/%.o: packet-transfer/common/src/%.c packet-transfer/common/include/*
	$(CC) $(CFLAGS) -c -o $@ $<

# Create library
$(LIBRARY): $(PACKET_OBJ)
	ar rcs $@ $^

# Format code using clang-format
format:
	$(CLANG_FORMAT) $(FORMAT_FLAGS) $(DRIVER_SRC) $(PACKET_SRC) packet-transfer/packets/include/*.h $(COMMON_SRC) packet-transfer/common/include/*.h 

# Clean target to remove object files, the executable, and the library
clean:
	rm -f $(EXECUTABLE) $(DRIVER_OBJ) $(PACKET_OBJ) $(COMMON_OBJ) $(LIBRARY)

run:
	LOG_LEVEL=DEBUG ./build/packet-transfer

build_debug: CFLAGS += $(DEBUG_FLAGS)
build_debug: clean all

