SHELL = /bin/sh

## OpenCL setup:

## UBUNTU:
## sudo apt install opencl-headers ocl-icd-opencl-dev

## + if VirtualBox:
## sudo apt-get install libpocl2



# C++ compiler
CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17
LDFLAGS=-D CL_TARGET_OPENCL_VERSION=100 -lOpenCL

# C files
SRC=$(shell find $(SOURCEDIR) -name '*.c' | grep -v scripts)
OBJ=$(SRC:.c=.o)

# C compiler
CC=gcc
CCFLAGS=-Wall -Wextra
LDFLAGS=-lOpenCL -D CL_TARGET_OPENCL_VERSION=100

# binary name
OUT=app

# Define calls to make
.PHONY: all clean run
all: debug

debug:   CXXFLAGS += -ggdb
release: CXXFLAGS += -O2 -DNDEBUG
debug release: application

application: $(OUT) 

# Dependencies
$(OUT): $(OBJ) # app <------ .cpp files
	$(CC) $(CCFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(OBJ):  # .cpp files <------- compiled shaders + script execute
	$(CC) $(CCFLAGS) $(LDFLAGS) -c $(@:.o=.c) -o $@ 

run: all
	./$(OUT)

clean:
	rm -f $(OBJ) $(OUT) 