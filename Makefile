# Paths to include headers and libraries
INC = -Iinclude
LIB = -lpthread

# Directory paths
SRC = src
OBJ = obj
INCLUDE = include

# Compiler and flags
CC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Source and header file search paths
vpath %.c $(SRC)
vpath %.h $(INCLUDE)

# Command for compilation
MAKE = $(CC) $(INC)

# Object files for different modules
MEM_OBJ = $(addprefix $(OBJ)/, paging.o mem.o cpu.o loader.o)
OS_OBJ = $(addprefix $(OBJ)/, cpu.o mem.o loader.o queue.o os.o sched.o timer.o mm-vm.o mm.o mm-memphy.o)
SCHED_OBJ = $(addprefix $(OBJ)/, cpu.o loader.o)
HEADER = $(wildcard $(INCLUDE)/*.h)

# Targets
all: os

# Compile memory management modules
mem: $(MEM_OBJ)
	$(MAKE) $(LFLAGS) $(MEM_OBJ) -o mem $(LIB)

# Compile scheduler
sched: $(SCHED_OBJ)
	$(MAKE) $(LFLAGS) $(SCHED_OBJ) -o sched $(LIB)

# Compile the whole OS simulation
os: $(OS_OBJ)
	$(MAKE) $(LFLAGS) $(OS_OBJ) -o os $(LIB)

# Rule for creating object files
$(OBJ)/%.o: %.c ${HEADER} $(OBJ)
	$(MAKE) $(CFLAGS) $< -o $@

# Create the obj directory if it doesn't exist
$(OBJ):
	mkdir -p $(OBJ)

# Clean up object files and executables
clean:
	rm -f $(OBJ)/*.o os sched mem
	rm -r $(OBJ)
