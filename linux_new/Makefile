SHELL = /bin/sh

# C++ compiler
CXX=g++
CXXFLAGS=-Wall -pipe -std=c++17 -Wpessimizing-move
LDFLAGS=$(shell pkg-config --libs vulkan) $(shell pkg-config --libs glfw3) -lpthread -L$(VULKAN_SDK)/lib `pkg-config --static --libs glfw3`
#SRC=$(wildcard *.cpp)

# C compiler
CC=gcc
CCFLAGS=-Wall -g

# GLSL compiler
GLSLC=glslc
GLSLFLAGS=

# CPP files
SRC=$(shell find $(SOURCEDIR) -name '*.cpp' | grep -v scripts)
OBJ=$(SRC:.cpp=.o)

# dynamically_generated_headers
DYN_HEADERS=./include/vulkan/vDynamicShader.h
TMP_HEADERS=$(DYN_HEADERS:.h=.tmp)

# C files
SRC_SCRIPT=$(shell find $(SOURCEDIR) -name '*.c' | grep scripts)
OUT_SCRIPT=$(SRC_SCRIPT:.c=.exe)

# GLSL shaders to compile
VSHADERSRC=$(shell find $(SOURCEDIR) -name '*.vert')
FSHADERSRC=$(shell find $(SOURCEDIR) -name '*.frag')

TESESHADERSRC=$(shell find $(SOURCEDIR) -name '*.tese')
ALL_TESE_SHADERS=$(shell echo $(TESESHADERSRC) | tr ' ' '\n' | sort)

TESCSHADERSRC=$(shell find $(SOURCEDIR) -name '*.tesc')
ALL_TESC_SHADERS=$(shell echo $(TESCSHADERSRC) | tr ' ' '\n' | sort)

ALL_VERT_SHADERS=$(shell echo $(VSHADERSRC) | tr ' ' '\n' | sort)
ALL_FRAG_SHADERS=$(shell echo $(FSHADERSRC) | tr ' ' '\n' | sort)
# SOME INFO: we only compile shaders that have both .vert and .frag
TARGET_SHADERS=$(shell for item in $(ALL_VERT_SHADERS:.vert=); do \
	for item2 in $(ALL_FRAG_SHADERS:.frag=); do \
		if [ $$item2 = $$item ]; then\
			echo $$item; fi;\
	done; done) # only both .vert and .frag -> complete shaders

OBJ_TESE_SHADERS=$(shell for item in $(TARGET_SHADERS); do \
	for item2 in $(ALL_TESE_SHADERS:.tese=); do \
		if [ $$item2 = $$item ]; then\
			echo $$item2; fi;\
	done; done) # Add tese shaders
OBJ_TESC_SHADERS=$(shell for item in $(TARGET_SHADERS); do \
	for item2 in $(ALL_TESC_SHADERS:.tesc=); do \
		if [ $$item2 = $$item ]; then\
			echo $$item2; fi;\
	done; done) # Add tesc shaders
SHADEROBJ=$(shell for item in $(OBJ_TESE_SHADERS); do echo $$item".tese.spv"; done;)
SHADEROBJ+=$(shell for item in $(OBJ_TESC_SHADERS); do echo $$item".tesc.spv"; done;)
SHADEROBJ+=$(shell for item in $(TARGET_SHADERS); do echo $$item".vert.spv"; done;)
SHADEROBJ+=$(shell for item in $(TARGET_SHADERS); do echo $$item".frag.spv"; done;)

OUT=app



# Define calls to make
.PHONY: all debug release run clean app
all: debug clean_others

debug:   CXXFLAGS += -ggdb
debug:   CCFLAGS += -g
release: CXXFLAGS += -O2 -DNDEBUG
debug release: application

application: $(OUT) clean_others clean_scripts
shaders: $(SHADEROBJ) header clean_others clean_scripts
header: $(DYN_HEADERS) scripts clean_others clean_scripts
scripts: $(OUT_SCRIPT) clean_others


# Dependencies
$(OUT): $(OBJ) # app <------ .cpp files
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(OBJ): $(SHADEROBJ) # .cpp files <------- compiled shaders + script execute
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $(@:.o=.cpp)

$(SHADEROBJ) : $(DYN_HEADERS)

$(DYN_HEADERS): $(OUT_SCRIPT)
	for n_shader in $(TARGET_SHADERS); do\
		bash -c "find $(SOURCEDIR) -wholename '$$n_shader.*' | tr '\n' ' ' | \
		xargs ./scripts/shader_fetch/shader_fetch.exe"; \
	done	
	./scripts/shader_factory/shader_factory.exe
	rm -f $(TMP_HEADERS)

%.exe: %.c
	$(CC) $(CCFLAGS) $< -o $@

%.vert.spv: %.vert $(OUT_SCRIPT) 
	$(GLSLC) $(GLSLFLAGS) -fshader-stage=vert $< -o $@

%.frag.spv: %.frag $(OUT_SCRIPT) 
	$(GLSLC) $(GLSLFLAGS) -fshader-stage=frag $< -o $@

%.tese.spv: %.tese $(OUT_SCRIPT)
	$(GLSLC) $(GLSLFLAGS) -fshader-stage=tese $< -o $@
%.tesc.spv: %.tesc $(OUT_SCRIPT)
	$(GLSLC) $(GLSLFLAGS) -fshader-stage=tesc $< -o $@

run: all
	./$(OUT)

clean: clean_others clean_exe clean_dynamic clean_scripts clean_shaders

clean_dynamic:
	rm -f $(DYN_HEADERS)
clean_scripts:
	rm -f $(OUT_SCRIPT) 
clean_shaders:
	rm -f $(SHADEROBJ)
clean_others:
	rm -f $(OBJ) $(OBJ_SCRIPT) $(TMP_HEADERS)
clean_exe:
	rm -f $(OUT) 