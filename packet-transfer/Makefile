# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I packet-transfer/packets/include -I packet-transfer/packets

# Source files
DRIVER_SRC = packet-transfer/driver/main.c
PACKET_SRC = packet-transfer/packets/src/message_transmitter.c \
             packet-transfer/packets/src/packet.c \
             packet-transfer/packets/src/stack.c

# Object files
DRIVER_OBJ = build/main.o
PACKET_OBJ = build/message_transmitter.o \
             build/packet.o \
             build/stack.o

# Library
LIBRARY = build/libpackets.a

# Executable
EXECUTABLE = build/packet-transfer

# clang-format
CLANG_FORMAT = clang-format
FORMAT_FLAGS = -i -style=file

# Build target
all: $(EXECUTABLE)

# Create build directory
$(shell mkdir -p build)

# Linking the object files with the library to create the executable
$(EXECUTABLE): $(DRIVER_OBJ) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling the driver source file
build/main.o: packet-transfer/driver/main.c packet-transfer/packets/include/*
	$(CC) $(CFLAGS) -c -o $@ $<

# Compiling the packet source files
build/%.o: packet-transfer/packets/src/%.c packet-transfer/packets/include/*
	$(CC) $(CFLAGS) -c -o $@ $<

# Create library
$(LIBRARY): $(PACKET_OBJ)
	ar rcs $@ $^

# Format code using clang-format
format:
	$(CLANG_FORMAT) $(FORMAT_FLAGS) $(DRIVER_SRC) $(PACKET_SRC) packet-transfer/packets/include/*.h

# Clean target to remove object files, the executable, and the library
clean:
	rm -f $(EXECUTABLE) $(DRIVER_OBJ) $(PACKET_OBJ) $(LIBRARY)
