# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iincludes
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = includes
OBJ_DIR = obj

# Source files and object files
SOURCES = $(SRC_DIR)/func1.c $(SRC_DIR)/func2.c $(SRC_DIR)/func3.c main.c
OBJECTS = $(SOURCES:%.c=$(OBJ_DIR)/%.o)

# Output executable
TARGET = program

# Targets

# Default target to compile the program
all: $(TARGET)

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compile the program
$(TARGET): $(OBJ_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the object files and the executable
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# If arguments are passed, run the program with them
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Include rules for handling file dependencies
# This will automatically create the dependencies for the headers
-include $(OBJECTS:.o=.d)

# Create dependency files
%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@
