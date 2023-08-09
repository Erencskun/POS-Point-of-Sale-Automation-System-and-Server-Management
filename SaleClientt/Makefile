# the name of your executable
TARGET = SALEDEMO
# launches your target in terminal
RUN = ./$(TARGET)

# directory for your source files
SRC_DIR = $(PWD)/src

# directory for your object files
BUILD_DIR = bin

# add compile
CC = gcc

# add more CompilerFLAGS as your project requires
CFLAGS = -std=c99 -Wall -Wextra -lpthread -ldl -lm

# add libraries for your project here
LDFLAGS = -pthread 

# add library linker commands here (start with -l)
LOADLIBS = -lsqlite3

# add library search paths here (start with -L)
LDLIBS = -L$(PWD)/main/src

# add include paths (start with -I)
INC = -I$(PWD)/main/inc

# finds all your objects that corrispond to your .cpp files, system agnostic version
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

.PHONY: all
# makes build directory, updates your objects, builds your executable
all: 
	mkdir -p $(BUILD_DIR)
	+$(MAKE) $(TARGET)

# updates your objects, builds your executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $@ $(LOADLIBS) $(LDFLAGS) $(LDLIBS) $(INC)

# builds your objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LOADLIBS) $(LDFLAGS) $(LDLIBS) $(INC)

# deletes your built objects and executable
.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(TARGET)

# makes build direcotry, updates your objects, builds your executable, launches your program
.PHONY: run
run: 
	+$(MAKE) all
	$(RUN)

# explains the only three options you should be using (unless you build more of your own)
.PHONY: help
help:
	@echo "`make`       - builds/updates everything, is ready to run with `$(RUN)` after completion"
	@echo "`make clean` - removes object file folder and executable"
	@echo "`make run`   - builds/updates everything, runs immediately"