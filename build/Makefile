# Don't use normal gcc, use the arm cross compiler
CC = arm-none-eabi-gcc 

# Set any constants based on the raspberry pi model. Version 1 has some differences to 2 and 3
ifeq ($(RASPI_MODEL),1)
    CPU = arm1176jzf-s
    DIRECTIVES = -D MODEL_1
else
    CPU = cortex-a7
endif

# Necessary compiler and linker flags in variables
CFLAGS = -mcpu=$(CPU) -fpic -ffreestanding $(DIRECTIVES)
CSRCFLAGS = -O2 -Wall -Wextra
LFLAGS = -ffreestanding -O2 -nostdlib

# Location of the files
KER_SRC = ../src/kernel
KER_HEAD = ../include
COMMON_SRC = ../src/common

# Specify location of a folder to store all binary object files
OBJ_DIR = objects

# Gets a list of all the source and header files
KERSOURCES = $(wildcard $(KER_SRC)/*.c)
COMMONSOURCES = $(wildcard $(COMMON_SRC)/*.c)
ASMSOURCES = $(wildcard $(KER_SRC)/*.S)

# Compiles a list of all object files that will be created
OBJECTS = $(patsubst $(KER_SRC)/%.c, $(OBJ_DIR)/%.o, $(KERSOURCES))
OBJECTS += $(patsubst $(COMMON_SRC)/%.c, $(OBJ_DIR)/%.o, $(COMMONSOURCES))
OBJECTS += $(patsubst $(KER_SRC)/%.S, $(OBJ_DIR)/%.o, $(ASMSOURCES))
HEADERS = $(wildcard $(KER_HEAD)/*.h)

# Name of the kernel binary
IMG_NAME = kernel.img

# Build target compiles the kernel (depends on objects and headers)
build: $(OBJECTS) $(HEADERS)
	echo $(OBJECTS)
	$(CC) -T linker.ld -o $(IMG_NAME) $(LFLAGS) $(OBJECTS)

# Target any object file that depends on a source file
$(OBJ_DIR)/%.o: $(KER_SRC)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)

# Target any object file that depends on an assembly file
$(OBJ_DIR)/%.o: $(KER_SRC)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -c $< -o $@

# Target any object file that depends on a common source file
$(OBJ_DIR)/%.o: $(COMMON_SRC)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)

# Removes all of the compiled binaries
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(IMG_NAME)

# Run with emulator
run: build
	qemu-system-arm -m 1024 -M raspi2b -serial stdio -kernel kernel.img
